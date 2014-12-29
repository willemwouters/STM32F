#include "stdio.h"
#include "ctype.h"

#include "stm32f2xx.h"
#include "platform.h"
#include "mdns.h"
#include "mxchipWNET.h"

#define AP_NAME           "William's Airport"
#define AP_PASSWORD       "mx099555"
#define APP_INFO          "mxchipWNet Demo: mDNS and Bonjour"

#define IPPORT_MDNS 5353
#define MDNS_ADDR 0xE00000FB //"224.0.0.251"

/**************************************************************************************************************
 * STRUCTURES
 **************************************************************************************************************/
network_InitTypeDef_st wNetConfig;

extern u32 MS_TIMER;

u32 my_ip_addr = 0;
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
	my_ip_addr = inet_addr(pnet->ip);
	printf("IP address: %s \r\n", pnet->ip);
	printf("NetMask address: %s \r\n", pnet->mask);
	printf("Gateway address: %s \r\n", pnet->gate);
	printf("DNS server address: %s \r\n", pnet->dns);
  printf("MAC address: %s \r\n", pnet->mac);
}

int main(void)
{
  int mDMS_fd = 0;
  char *buf;
  int con = -1;
  fd_set readfds, exceptfds;
  struct timeval_t t;
  struct sockaddr_t addr;
  socklen_t addrLen;
	
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
	
	mdns_init();
	
  t.tv_sec = 0;
  t.tv_usec = 100;
	
  set_tcp_keepalive(3, 60);
	
	mDMS_fd = socket(AF_INET, SOCK_DGRM, IPPROTO_UDP);
  addr.s_port = IPPORT_MDNS;
  bind(mDMS_fd, &addr, sizeof(addr));
  addr.s_ip = MDNS_ADDR;
  if (connect(mDMS_fd, &addr, sizeof(addr)) < 0) {
		close(mDMS_fd);
		mDMS_fd = 0;
	}

  while(1) {
    mxchipTick();			
		
		/*Check status on erery sockets */
		FD_ZERO(&readfds);
		FD_SET(mDMS_fd, &readfds);	
		select(1, &readfds, NULL, &exceptfds, &t);
    
		/*Read data from udp and send data back */ 
		if (FD_ISSET(mDMS_fd, &readfds)) {
      con = recvfrom(mDMS_fd, buf, 3*1024, 0, &addr, &addrLen);  
      mdns_handler(mDMS_fd, (u8 *)buf, con);
		}
	}
}

