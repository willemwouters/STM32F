#include "stdio.h"
#include "ctype.h"

#include "platform.h"
#include "mxchipWNET.h"
#include "http_process.h"
#include "flash_configurations.h"

#define APP_INFO          "mxchipWNet Demo: Web server and OTA"


config_t configParas;
network_InitTypeDef_st wNetConfig;
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
      printf("Wi-Fi up \r\n");
      break;
    case MXCHIP_WIFI_DOWN:
      printf("Wi-Fi down \r\n");
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

int main(void)
{
	
	mxchipInit();
	UART_Init();
  printf("\r\n%s\r\nmxchipWNet library version: %s\r\n", APP_INFO, system_lib_version());
  
	readConfig(&configParas);
	
#ifdef LowPowerMode
  enable_ps_mode(SLEEP_UNIT_MS, 100, 100);
#endif	
	
  memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));	
	wNetConfig.wifi_mode = Soft_AP;
	strcpy(wNetConfig.wifi_ssid, configParas.wifi_ssid);
	strcpy(wNetConfig.wifi_key, configParas.wifi_key);
	sprintf(wNetConfig.local_ip_addr, "192.168.1.1");
	sprintf(wNetConfig.net_mask, "255.255.255.0");
	sprintf(wNetConfig.gateway_ip_addr, "192.168.1.1");
	sprintf(wNetConfig.dnsServer_ip_addr, "192.168.1.1");
	sprintf(wNetConfig.address_pool_start, "192.168.1.100");
	sprintf(wNetConfig.address_pool_end, "192.168.1.200");
	wNetConfig.dhcpMode = DHCP_Server;
	StartNetwork(&wNetConfig);
  printf("Establish soft AP: %s.....\r\n", wNetConfig.wifi_ssid);
	
  set_tcp_keepalive(3, 60);
	http_init();
	
  while(1) {
    mxchipTick();	
		http_tick();
	}
}

