#include "stdio.h"
#include "ctype.h"

#include "stm32f2xx.h"
#include "platform.h"
#include "mxchipWNET.h"

/*
Enable IEEE powersave mode,
*/
//#define LowPowerMode

/*
Under this configuration, you can allocate every socket's send/recv buffer for application's requirement.
The more buffer is allocated, faster the speed would be, but the max connection number would be less.
If there is not enough memory for a connection:
	TCP client: function: connect would return fail
	TCP server: Any connection to the server would reset
If the buffer is not setted, stack would use the default size: 2048bytes.
*/
#define DynamicMemAlloc          
#define AP_NAME           "MXCHIP_RD"
#define AP_PASSWORD       "stm32f215"
#define WEB_SERVER	  "www.baidu.com"

//#define BlockMode

network_InitTypeDef_st wNetConfig;
lib_config_t libConfig;
int wifi_up = 0;
int webserverTest = 1;
int cloud_ip_addr = 0;
int dns_pending = 0;
extern u32 MS_TIMER;
const char httpRequest[]="GET / HTTP/1.1\r\n\
Host: www.baidu.com\r\n\
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_8_4) AppleWebKit/536.30.1 (KHTML, like Gecko) Version/6.0.5 Safari/536.30.1\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n\
Accept-Language: zh-cn\r\n\
Accept-Encoding: \r\n\
Connection: keep-alive\r\n\
\r\n\
";



/* ========================================
	User provide callback functions 
    ======================================== */
void RptConfigmodeRslt(network_InitTypeDef_st *nwkpara)
{
}
		
void userWatchDog(void)
{
}

void WifiStatusHandler(int event)
{
  switch (event) {
    case MXCHIP_WIFI_UP:
			wifi_up = 1;
      printf("Station up \r\n");
      break;
    case MXCHIP_WIFI_DOWN:
      printf("Station down \r\n");
      break;
    default:
      break;
  }
  return;
}

void ApListCallback(UwtPara_str *pApList)
{
}

void NetCallback(net_para_st *pnet)
{
	printf("IP address: %s \r\n", pnet->ip);
	printf("NetMask address: %s \r\n", pnet->mask);
	printf("Gateway address: %s \r\n", pnet->gate);
	printf("DNS server address: %s \r\n", pnet->dns);
  printf("MAC address: %s \r\n", pnet->mac);
}

#ifndef BlockMode
void dns_ip_set(u8 *hostname, u32 ip)
{
	char ipstr[16];
	cloud_ip_addr = ip;
  dns_pending = 0;
	if(cloud_ip_addr == -1)
		printf("DNS test: %s failed \r\n", WEB_SERVER);
	else{
		inet_ntoa(ipstr, ip);
		printf("DNS test: %s address is %s \r\n", WEB_SERVER, ipstr);
	}
}


void socket_connected(int fd)
{
	printf("Connect to web server success! Reading web pages...\r\n");
	send(fd, httpRequest, sizeof(httpRequest), 0);
}
#endif

int main(void)
{
  int i, j, fd_listen = -1, fd_udp = -1, fd_client = -1;
  char *buf, ip_address[16],ipstr[32];
  int len;
  int con = -1;
	int opt = 0;
  int clientfd[8];
  fd_set readfds, exceptfds;
  struct timeval_t t;
  struct sockaddr_t addr;
  socklen_t addrLen;
	
#ifdef	DynamicMemAlloc
	int bufferSize;
  libConfig.tcp_buf_dynamic = mxEnable;
	lib_config(&libConfig);
#endif

  for(i=0;i<8;i++) 
    clientfd[i] = -1;

  buf = (char*)malloc(3*1024);
	
	mxchipInit();
	UART_Init();
	
#ifdef LowPowerMode
  enable_ps_mode(SLEEP_UNIT_MS, 100, 100);
#endif	
	
  memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));
	wNetConfig.wifi_mode = Station;
	strcpy((char*)wNetConfig.wifi_ssid, AP_NAME);
	strcpy((char*)wNetConfig.wifi_key, AP_PASSWORD);
	wNetConfig.dhcpMode = DHCP_Client;
	StartNetwork(&wNetConfig);
  printf("Connect to %s.....\r\n", wNetConfig.wifi_ssid);
	
  t.tv_sec = 0;
  t.tv_usec = 100;
	
  set_tcp_keepalive(3, 60);
	
  while(1) {
    mxchipTick();	
		
		/*If wifi is established, connect to www.baidu.com, and send a http request*/
#ifdef BlockMode
		if(wifi_up&&webserverTest){
			webserverTest = 0;
			gethostbyname(WEB_SERVER, (u8 *)ipstr, 32);	
			if(strcmp(ipstr, "255.255.255.255") == 0){
				printf("DNS test: %s failed \r\n", WEB_SERVER);	
				webserverTest = 0;
				continue;
			}
			printf("DNS test: %s address is %s \r\n", WEB_SERVER, ipstr);
			
			fd_client	= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			addr.s_ip = inet_addr(ipstr); 
      addr.s_port = 80;
			if (connect(fd_client, &addr, sizeof(addr))!=0) {
        close(fd_client);
        fd_client = -1;
        printf("Connect to web server failed! \r\n");
      }
			else{
				printf("Connect to web server success! Reading web pages...\r\n");
				send(fd_client, httpRequest, sizeof(httpRequest), 0);
			}
		}
#else
		if(cloud_ip_addr == 0 && dns_pending == 0){  //DNS function
			cloud_ip_addr = dns_request(WEB_SERVER);
			if(cloud_ip_addr == -1)
				printf("DNS test: %s failed. \r\n", WEB_SERVER); 
			else if (cloud_ip_addr == 0) //DNS pending, waiting for callback
				dns_pending = 1;
		}

		if( fd_client == -1 && (u32)cloud_ip_addr>0&&webserverTest){
			fd_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			setsockopt(fd_client,0,SO_BLOCKMODE,&opt,4);
			addr.s_ip = cloud_ip_addr; 
			addr.s_port = 80;
			if (connect(fd_client, &addr, sizeof(addr))!=0) 
				printf("Connect to %s failed.\r\n", WEB_SERVER); 
			webserverTest = 0;
		}
#endif
			
		/*Establish a TCP server that accept the tcp clients connections*/
		if (fd_listen==-1) {
      fd_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#ifdef	DynamicMemAlloc				
			  bufferSize = 5*1024;
				setsockopt(fd_listen,0,SO_RDBUFLEN,&bufferSize,4);
				bufferSize = 5*1024;
				setsockopt(fd_listen,0,SO_WRBUFLEN,&bufferSize,4);
#endif	
      addr.s_port = 8080;
      bind(fd_listen, &addr, sizeof(addr));
      listen(fd_listen, 0);
      //printf("TCP server established at port: %d \r\n", addr.s_port);
    }
		
		/*Establish a UDP port to receive any data sent to this port*/
		if (fd_udp==-1) {
      fd_udp = socket(AF_INET, SOCK_DGRM, IPPROTO_UDP);
      addr.s_port = 8090;
      bind(fd_udp, &addr, sizeof(addr));
      //printf("Open UDP port %d\r\n", addr.s_port);
    }
		
		/*Check status on erery sockets */
		FD_ZERO(&readfds);
		FD_SET(fd_listen, &readfds);	
		FD_SET(fd_udp, &readfds);	
		if(fd_client!=-1)
		  FD_SET(fd_client, &readfds);
		for(i=0;i<8;i++) {
			if (clientfd[i] != -1)
				FD_SET(clientfd[i], &readfds);
		}
		
		select(1, &readfds, NULL, &exceptfds, &t);
    
    /*Check tcp connection requests */
		if(FD_ISSET(fd_listen, &readfds))
		{
			j = accept(fd_listen, &addr, &len);
			if (j > 0) {
			  inet_ntoa(ip_address, addr.s_ip );
			  printf("Client %s:%d connected\r\n", ip_address, addr.s_port);
			  for(i=0;i<8;i++) {
				  if (clientfd[i] == -1) {
					  clientfd[i] = j;
					  break;
				  }
			  }
			}
		}
		
		/*Read html data from www.baidu.com */
		if(fd_client != -1){
			if(FD_ISSET(fd_client, &readfds))
			{
				con = recv(fd_client, buf, 2*1024, 0);
				if(con > 0)
					printf("Get %s data successful! data length: %d bytes\r\n", WEB_SERVER, con);
				else{
					close(fd_client);
					fd_client = -1;
					printf("Web connection closed.");
				}
			}
	  }
    
   /*Read data from tcp clients and send data back */ 
	 for(i=0;i<8;i++) {
      if (clientfd[i] != -1) {
        if (FD_ISSET(clientfd[i], &readfds)) {
          con = recv(clientfd[i], buf, 1*1024, 0);
          if (con > 0) 
            send(clientfd[i], buf, con, 0);
          else {
            close(clientfd[i]);
            clientfd[i] = -1;
          }
        }
        else if (FD_ISSET(clientfd[i], &exceptfds))
          clientfd[i] = -1;
      }
    }
		
		/*Read data from udp and send data back */ 
		if (FD_ISSET(fd_udp, &readfds)) {
      con = recvfrom(fd_udp, buf, 3*1024, 0, &addr, &addrLen);
      sendto(fd_udp, buf, con, 0, &addr, sizeof(struct sockaddr_t));
    }
	}
}

