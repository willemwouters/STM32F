
#include "ctype.h"

#include "stdio.h"

#include "stm32f2xx.h"
#include "platform.h"
#include "mxchipWNET.h"
#include "http_process.h"
#include "flash_configurations.h"

#include "stm32_Config_Codec.h"
#include "Camera.h"

#define DynamicMemAlloc          

#define LowPowerMode
#define AP_NAME           "MXCHIP_RD"
#define AP_PASSWORD       "stm32f215"
#define AP_PASSWORD_LEN   9

u8 wifiModeSwitch = 0;
u8 softAPStarted = 0;
network_InitTypeDef_st wNetConfig;
lib_config_t libConfig;
config_t configParas;

int wifi_up = 0;
int configSuccess = 0;

void WifiStatusHandler(int event)
{
  switch (event) {
    case MXCHIP_WIFI_UP:
			wifi_up = 1;
      break;
    case MXCHIP_WIFI_DOWN:
      break;
    default:
      break;
  }
  return;
}


void RptConfigmodeRslt(network_InitTypeDef_st *nwkpara)
{
	if(nwkpara == NULL){
		configSuccess = 0;
	}
	else{
		configSuccess = 1;
		memcpy(wNetConfig.wifi_ssid, nwkpara->wifi_ssid, 32);
		memcpy(wNetConfig.wifi_key, nwkpara->wifi_key, 32);
	}
}

	

int SetupSoftAP(void)
{
	wNetConfig.wifi_mode = Soft_AP;
	strcpy((char*)wNetConfig.wifi_ssid, "IP Camera");
	strcpy((char*)wNetConfig.wifi_key, "");
	
	strcpy((char*)wNetConfig.local_ip_addr, "192.168.117.186");
	strcpy((char*)wNetConfig.address_pool_start, "192.168.117.190");
	strcpy((char*)wNetConfig.address_pool_end, "192.168.177.222");

	strcpy((char*)wNetConfig.gateway_ip_addr, "192.168.117.186");
	
	wNetConfig.dhcpMode = DHCP_Server;
	return StartNetwork(&wNetConfig);
}



int SetupStation()
{
	wNetConfig.wifi_mode = Station;
	//strcpy((char*)wNetConfig.wifi_ssid, "MXCHIP_RD");
	//strcpy((char*)wNetConfig.wifi_key, "stm32f215");
	
	strcpy((char*)wNetConfig.local_ip_addr, "192.168.117.188");
	strcpy((char*)wNetConfig.gateway_ip_addr, "192.168.117.1");
	
	wNetConfig.dhcpMode = DHCP_Disable;
	return StartNetwork(&wNetConfig);
}

char buf[1024];

int DoUDPCmd(char *pCmd)
{
	if((pCmd[0]=='C') && (pCmd[1]=='K'))
	{
		return sprintf(pCmd, "ID:  WIFI Camera\nSN:  LWS00001\nVer: 0W.01..030\n");
	}

	return 0;
}

void InitNetData()
{
	memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));

	strcpy((char*)wNetConfig.net_mask, "255.255.255.0");
	
}


static const char cSendBack[] = 
"HTTP/1.0 200 OK\nContent-Type:multipart/x-mixed-replace;boundary=\n\n--\n";

void MainTask(void)
{
	int fd = -1;
	int fd_udp = -1;
	int len;
	int i, j;
	int clientfd[8];
	int so = -1;
	
	struct sockaddr_t addr;
	fd_set readfds, exceptfds;
	struct timeval_t t;

	socklen_t addrLen;
	int con = -1;

	u8 *pSend;
	u8 *pBlock;
	u8 iErrCnt = 0;

	char bConnect = 0;
	
#ifdef	DynamicMemAlloc
	int bufferSize;
	libConfig.tcp_buf_dynamic = mxEnable;
	lib_config(&libConfig);
#endif
	
	mxchipInit();
	InitNetData();
		
	readConfig(&configParas);

	//OpenEasylink(60);

	if(0)//configSuccess)
	{
		int cnt = 12;
		
		SetupStation();

		while(wifi_up==0)
		{
			msleep(500);
			cnt--;
			if(cnt==0)
				break;
		}

		if(cnt==0)
			SetupSoftAP();
		else
		{
			//save SSID & Key from EasyLink to FLASH
			memcpy(configParas.wifi_ssid, wNetConfig.wifi_ssid, 32);
			memcpy(configParas.wifi_key, wNetConfig.wifi_key, 32);
			updateConfig(&configParas);
		}
	}

	else
	{
		if(configParas.wifi_ssid[0]==0)
			SetupSoftAP();
		else
		{
			int cnt = 20;
			
			strcpy((char*)wNetConfig.wifi_ssid, configParas.wifi_ssid);
			strcpy((char*)wNetConfig.wifi_key, configParas.wifi_key);

			SetupStation();

			while(wifi_up==0)
			{
				msleep(500);
				cnt--;
				if(cnt==0)
					break;
			}

			if(cnt==0)
			{
				wlan_disconnect();
				SetupSoftAP();
			}
		}
	}
	
	InitCamera();
	  
	for(i=0;i<8;i++) 
		clientfd[i] = -1;
	
	set_tcp_keepalive(3, 60);

	http_init();

	while(1)
	{
		mxchipTick();
		http_tick();

		if (fd==-1)
		{
			fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#ifdef	DynamicMemAlloc				
			bufferSize = 5*1024;
			//setsockopt(fd,0,SO_RDBUFLEN,&bufferSize,4);
			bufferSize = 8*1024;
			setsockopt(fd,0,SO_WRBUFLEN,&bufferSize,4);
#endif	
			addr.s_port = 8421;
			bind(fd, &addr, sizeof(addr));
			listen(fd, 0);
		}

		/*Establish a UDP port to receive any data sent to this port*/
		if (fd_udp==-1)
		{
			fd_udp = socket(AF_INET, SOCK_DGRM, IPPROTO_UDP);
			addr.s_port = 8090;
			bind(fd_udp, &addr, sizeof(addr));
		}
			
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds); 
		FD_SET(fd_udp, &readfds);	
		
		for(i=0;i<8;i++)
		{
			if (clientfd[i] != -1)
				FD_SET(clientfd[i], &readfds);
		}

		t.tv_sec = 0;
		t.tv_usec = 0;
		select(1, &readfds, NULL, &exceptfds, &t);

		if(FD_ISSET(fd, &readfds))
		{
			j = accept(fd, &addr, &len);
			if (j > 0)
			{
				for(i=0;i<8;i++)
				{
					if (clientfd[i] == -1)
					{
						clientfd[i] = j;
						break;
					}
				}
			}
		}

		for(i=0;i<8;i++)
		{
			if (clientfd[i] != -1)
			{
				if (FD_ISSET(clientfd[i], &readfds))
				{
					con = recv(clientfd[i], buf, 1*1024, 0);
					if (con > 0) 
					{
						if((buf[0]=='G') && (buf[1]=='E') &&(buf[2]=='T') & (bConnect==0))
						{
							if(send(clientfd[i], cSendBack, strlen(cSendBack), 0)>0)
							{
								so = clientfd[i];
								bConnect = 1;
								Set_Read_CodecDataC();
								msleep(2);
							}
						}
					}
					
					else
					{
						close(clientfd[i]);
						if(clientfd[i]==so)
						{
							so = -1;
							bConnect = 0;
						}
						
						clientfd[i] = -1;
					}
				}

				else if (FD_ISSET(clientfd[i], &exceptfds))
				{
					if(clientfd[i]==so)
					{
						bConnect = 0;
						so = -1;
					}
					
					clientfd[i] = -1;
				}
			}
		}

		
		if(bConnect!=0)
		{
			if(CheckReadFinish()>0)
			{
				pSend = CodecSendBuffer + 3;
				
				pBlock = CodecRxBuffer;
				for(i=0;i < BLOCK_CNT; i++)
				{
					if(*pBlock==0xFF)
					{
						if(pBlock[1]==0)
							*pSend = 0xA0;
						else
							*pSend = 0xA1;
					}
					
					
					pSend++;
					pBlock += 2;

					memcpy(pSend,pBlock,BLOCK_LEN - 2);
					
					pSend += (BLOCK_LEN + 1);
					pBlock += BLOCK_LEN - 2;
				}

				Set_Read_CodecDataC();

				if(so!=-1)
				{
					send(so, CodecSendBuffer, sizeof(CodecSendBuffer), 0);
				}
				
				iErrCnt = 0;

			}
			else
			{
				iErrCnt++;
#if 0				
				if(iErrCnt>20)
				{
					ResetCamera();

					iErrCnt = 0;
				}
				
#endif				
				Set_Read_CodecDataC();

			}
		}

		/*Read data from udp and send data back */ 
		if (FD_ISSET(fd_udp, &readfds))
		{
			con = recvfrom(fd_udp, buf, sizeof(buf), 0, &addr, &addrLen);
			con = DoUDPCmd(buf);
			if(con)
				sendto(fd_udp, buf, con, 0, &addr, sizeof(struct sockaddr_t));
		}

	}

}


