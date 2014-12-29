#include "stdio.h"
#include "ctype.h"

#include "stm32f2xx.h"
#include "platform.h"
#include "mxchipWNET.h"

#define LowPowerMode
#define AP_NAME           "MXCHIP_RD"
#define AP_PASSWORD       "stm32f215"
#define APP_INFO          "mxchipWNet Demo: Wi-Fi Link"

network_InitTypeDef_st wNetConfig;
network_InitTypeDef_adv_st wNetConfigAdv;
net_para_st para;

const char *securityString[8] = {"Open system", "WEP", "WPA TKIP", "WPA AES", "WPA2 TKIP",
                           "WPA2 AES",  "WPA2 MIXED", "Auto"};

/* ========================================
	User provide callback functions 
    ======================================== */
void system_version(char *str, int len)
{
  snprintf( str, len, "%s", APP_INFO);
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
    case MXCHIP_UAP_UP:
      printf("uAP up \r\n");
      getNetPara(&para, Soft_AP);
      printf("Soft AP mode: IP address: %s \r\n", para.ip);
      printf("Soft AP mode: NetMask address: %s \r\n", para.mask);
      printf("Soft AP mode: MAC address: %s \r\n", para.mac);
      break;
    case MXCHIP_UAP_DOWN:
      printf("uAP down \r\n");
      break;
    default:
      break;
  }
  return;
}

void ApListCallback(ScanResult *pApList)
{
	int i;
  printf("Find %d APs: \r\n", pApList->ApNum);
  for (i=0;i<pApList->ApNum;i++)
    printf("    SSID: %s, Signal: %d%%\r\n", pApList->ApList[i].ssid, pApList->ApList[i].ApPower);
}

void NetCallback(net_para_st *pnet)
{
	printf("Station mode: IP address: %s \r\n", pnet->ip);
	printf("Station mode: NetMask address: %s \r\n", pnet->mask);
	printf("Station mode: Gateway address: %s \r\n", pnet->gate);
	printf("Station mode: DNS server address: %s \r\n", pnet->dns);
  printf("Station mode: MAC address: %s \r\n", pnet->mac);
}

void connected_ap_info(apinfo_adv_t *ap_info, char *key, int key_len)  //callback, return connected AP info
{
  /*Update fastlink record*/
  char *tempString;
  tempString = calloc(100, 1);
  memcpy(tempString, key, key_len+1);  
  
  printf("AP connected\r\nSSID: %s \r\n", ap_info->ssid);
  printf("Channel: %d \r\n", ap_info->channel);
  printf("Security: %s \r\n", securityString[ap_info->security]);
  printf("Key or PMK: %s \r\n", tempString);
 
  free(tempString);
}

void stationModeStart(void)
{
	int ret;
  memset(&wNetConfigAdv, 0x0, sizeof(network_InitTypeDef_adv_st));
	
	strcpy((char*)wNetConfigAdv.ap_info.ssid, AP_NAME);
  wNetConfigAdv.ap_info.channel = 0;  //Auto
  wNetConfigAdv.ap_info.security = SECURITY_TYPE_AUTO;  //Auto
  strcpy((char*)wNetConfigAdv.key, AP_PASSWORD);
  wNetConfigAdv.key_len = strlen(AP_PASSWORD);
	wNetConfigAdv.dhcpMode = DHCP_Client;
  wNetConfigAdv.wifi_retry_interval = 1000;
	ret = StartAdvNetwork(&wNetConfigAdv);
	printf("connect to %s....., return %d\r\n", wNetConfigAdv.ap_info.ssid, ret);
}

void softAPModeStart(void)
{
  int ret;
  memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));
	
	wNetConfig.wifi_mode = Soft_AP;
  strcpy((char*)wNetConfig.wifi_ssid, "Soft AP test");
  strcpy((char*)wNetConfig.wifi_key, "");
	strcpy((char*)wNetConfig.local_ip_addr, "192.168.0.1");
  strcpy((char*)wNetConfig.net_mask, "255.255.255.0");
	strcpy((char*)wNetConfig.address_pool_start, "192.168.0.10");
	strcpy((char*)wNetConfig.address_pool_end, "192.168.0.177");
	wNetConfig.dhcpMode = DHCP_Server;
	ret = StartNetwork(&wNetConfig);
	printf("Setup soft AP: %s, return %d\r\n", wNetConfig.wifi_ssid, ret);
}

vs32 tick = 0;
vs32 button_irq = 0;

void RptConfigmodeRslt(network_InitTypeDef_st *nwkpara)
{
	if(nwkpara == NULL){
		printf("Configuration failed\r\n");
	}
	else{
		printf("Configuration is successful, SSID:%s, Key:%s\r\n", \
																		nwkpara->wifi_ssid,\
																		nwkpara->wifi_key);
	}
}

int main(void)
{
  mxchipInit();
  UART_Init();
	Button_Init();
  printf("\r\n%s\r\nmxchipWNet library version: %s\r\n", APP_INFO, system_lib_version());
  printf("Start scan\r\n");
	mxchipStartScan();
  stationModeStart();
  softAPModeStart();

  while(1){
    mxchipTick();
  }
}


void Button_irq_handler(void *arg)
{
  printf("Start scanning by user...\r\n");
  mxchipStartScan();		
}
