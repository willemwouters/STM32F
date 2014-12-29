#include "stdio.h"
#include "ctype.h"

#include "platform.h"
#include "mxchipWNET.h"
#include "menu.h"

network_InitTypeDef_st wNetConfig;
int configSuccess = 0;
int easylink = 0;
int menu_enable = 1;

#define APP_INFO          "mxchipWNet Demo: WPS and EasyLink"


const char *securityString[8] = {"Open system", "WEP", "WPA TKIP", "WPA AES", "WPA2 TKIP",
                           "WPA2 AES",  "WPA2 MIXED", "Auto"};
const char menu[] =
   "\n"
   "+***************(C) COPYRIGHT 2013 MXCHIP corporation************+\n"
   "|          EMW316x WPS and EasyLink configuration demo           |\n"
   "+ command ----------------+ function ----_-----------------------+\n"
   "| 1:WPS                   | WiFi Protected Setup                 |\n"
   "| 2:EasyLink              | One step configuration from MXCHIP   |\n"
   "| 3:EasyLink_V2           | One step configuration from MXCHIP   |\n"
   "| 4:REBOOT                | Reboot                               |\n"
   "| ?:HELP                  | displays this help                   |\n"
   "+-------------------------+--------------------------------------+\n"
   "|                           By William Xu from MXCHIP M2M Team   |\n"
   "+----------------------------------------------------------------+\n";

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
	if(nwkpara == NULL){
		printf("Configuration failed\r\n");
		printf ("\nMXCHIP> ");
		menu_enable = 1;
	}
	else{
		configSuccess = 1;
		memcpy(&wNetConfig, nwkpara, sizeof(network_InitTypeDef_st));
		printf("Configuration is successful, SSID:%s, Key:%s\r\n", \
																		wNetConfig.wifi_ssid,\
																		wNetConfig.wifi_key);
	}
}
		
void easylink_user_data_result(int datalen, char* data)
{
   net_para_st para;
   getNetPara(&para, Station);
   if(!datalen){
    printf("No user input. %s\r\n", data);
   }
   else{
    printf("User input is %s\r\n", data);
   }
}    
    
void userWatchDog(void)
{
}

void WifiStatusHandler(int event)
{
  switch (event) {
    case MXCHIP_WIFI_UP:
      printf("Station up \r\n");
		  printf ("\nMXCHIP> ");
			menu_enable = 1;
      //easylink = 1;
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
  lib_config_t libConfig;
  libConfig.tcp_buf_dynamic = mxEnable;
  libConfig.tcp_max_connection_num = 12;
  libConfig.tcp_rx_size = 2048;
  libConfig.tcp_tx_size = 2048;
  libConfig.hw_watchdog = 0;
  libConfig.wifi_channel = WIFI_CHANNEL_1_13;
	lib_config(&libConfig);
  
  mxchipInit();
	UART_Init();
  printf("\r\n%s\r\n mxchipWNet library version: %s\r\n", APP_INFO, system_lib_version());
	printf(menu);
	printf ("\nMXCHIP> ");
	
	
  while(1){
    mxchipTick();	
		if(menu_enable)
			Main_Menu();
		if(configSuccess){
			wNetConfig.wifi_mode = Station;
			wNetConfig.dhcpMode = DHCP_Client;
			StartNetwork(&wNetConfig);
			printf("connect to %s.....\r\n", wNetConfig.wifi_ssid);
			configSuccess = 0;
			printf ("\nMXCHIP> ");
			menu_enable = 1;
		}
    if(easylink){
      OpenEasylink2(60);	
      easylink = 0;
    }
	}
}

