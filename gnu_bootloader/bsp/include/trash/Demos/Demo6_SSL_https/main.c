#include "stdio.h"
#include "ctype.h"

#include "stm32f2xx.h"
#include "platform.h"
#include "mxchipWNET.h"

#define DynamicMemAlloc          
#define AP_NAME           "William's Airport"
#define AP_PASSWORD       "mx099555"
#define WEB_SERVER				"persons.shgjj.com"
#define APP_INFO          "mxchipWNet Demo:SSL https"

network_InitTypeDef_st wNetConfig;
lib_config_t libConfig;
int wifi_up = 0;
int serverConnectted = 0;
int sslConnectted = 0;
int https_server_addr = 0;
int dns_pending = 0;
extern u32 MS_TIMER;
const char httpRequest[]="GET / HTTP/1.1\r\n\
Accept: text/html, application/xhtml+xml, */*\r\n\
Accept-Language: en\r\n\
User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; WOW64; Trident/6.0)\r\n\
Host: persons.shgjj.com\r\n\
Connection: Keep-Alive\r\n\r\n";

void print_msg(void);

/* ========================================
	User provide callback functions 
    ======================================== */
void system_version(char *str, int len)
{
  snprintf( str, len, "%s", APP_INFO);
}  

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

void ApListCallback(ScanResult *pApList)
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

void dns_ip_set(u8 *hostname, u32 ip)
{
	char ipstr[16];
	https_server_addr = ip;
  dns_pending = 0;
	if(serverConnectted == -1)
		printf("DNS test: %s failed \r\n", WEB_SERVER);
	else{
		inet_ntoa(ipstr, ip);
		printf("DNS test: %s address is %s \r\n", WEB_SERVER, ipstr);
	}
}


void socket_connected(int fd)
{
	serverConnectted = 1;
	

}

int main(void)
{
  int fd_client = -1;
  char *buf;
  int con = -1;
	int opt = 0;
  fd_set readfds, exceptfds;
  struct timeval_t t;
  struct sockaddr_t addr;

  buf = (char*)malloc(3*1024);
	
	mxchipInit();
	UART_Init();
  printf("\r\n%s\r\nmxchipWNet library version: %s\r\n", APP_INFO, system_lib_version());

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
  setSslMaxlen(6*1024);
	
  while(1) {
    mxchipTick();	
		
		/*If wifi is established, connect to www.baidu.com, and send a http request*/

		if(https_server_addr == 0 && dns_pending == 0){  //DNS function
			https_server_addr = dns_request(WEB_SERVER);
			if(https_server_addr == -1)
				printf("DNS test: %s failed. \r\n", WEB_SERVER); 
			else if (https_server_addr == 0) //DNS pending, waiting for callback
				dns_pending = 1;
		}

		if( fd_client == -1 && (u32)https_server_addr>0){
			fd_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			setsockopt(fd_client,0,SO_BLOCKMODE,&opt,4);
			addr.s_ip = https_server_addr; 
			addr.s_port = 443;
			if (connect(fd_client, &addr, sizeof(addr))!=0) 
      printf("Connect to %s failed.\r\n", WEB_SERVER); 
		}
    
    if(serverConnectted&&sslConnectted==0){
      printf("Connect to web server success! Setup SSL ecryption...\r\n");
      if (setSSLmode(1, fd_client)!= MXCHIP_SUCCESS){
         printf("SSL connect fail\r\n");
         close(fd_client);
				 fd_client = -1;
         serverConnectted = 0;
      } else {
        printf("SSL connect, send HTTP GET\r\n");
        send(fd_client, httpRequest, strlen(httpRequest), 0);
        sslConnectted = 1;
      }
    }
				
		
		/*Check status on erery sockets */
		FD_ZERO(&readfds);

		if(sslConnectted)
		  FD_SET(fd_client, &readfds);
		
		select(1, &readfds, NULL, &exceptfds, &t);
		
		/*Read html data from www.baidu.com */
		if(sslConnectted){
			if(FD_ISSET(fd_client, &readfds))
			{
				con = recv(fd_client, buf, 2*1024, 0);
				if(con > 0)
					printf("Get %s data successful! data length: %d bytes\r\n", WEB_SERVER, con);
				else{
					close(fd_client);
					serverConnectted = 0;
          sslConnectted = 0;
					fd_client = -1;
					printf("Web connection closed.\r\n");
				}
			}
	  }
	}
}

