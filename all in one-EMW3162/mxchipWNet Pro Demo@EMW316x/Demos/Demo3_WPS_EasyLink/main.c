#include "stdio.h"
#include "ctype.h"

#include "platform.h"
#include "mxchipWNET.h"
#include "menu.h"
#include "mico_rtos.h"

#define APP_INFO          "mxchipWNet Pro Demo: WPS and EasyLink demo"

network_InitTypeDef_st wNetConfig;
int configSuccess = 0;
int menu_enable = 1;

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

void userWatchDog(void)
{
}

void WifiStatusHandler(int event)
{
  switch (event) {
  case MXCHIP_WIFI_UP:
    debug_out("Station up \r\n");
    debug_out ("\nMXCHIP> ");
    menu_enable = 1;
    break;
  case MXCHIP_WIFI_DOWN:
    debug_out("Station down \r\n");
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
  debug_out("IP address: %s \r\n", pnet->ip);
  debug_out("NetMask address: %s \r\n", pnet->mask);
  debug_out("Gateway address: %s \r\n", pnet->gate);
  debug_out("DNS server address: %s \r\n", pnet->dns);
  debug_out("MAC address: %s \r\n", pnet->mac);
   debug_out ("\nMXCHIP> ");
}

int application_start(void)
{
  mxchipInit();
  Platform_Init();
  UART_Init();
  
#ifdef RFLowPowerMode
  ps_enable();
#endif	
  
#ifdef MCULowPowerMode
  mico_mcu_powersave_config(mxEnable);
#endif	
  
  debug_out("\r\n%s\r\nmxchipWNet library version: %s\r\n", APP_INFO, system_lib_version());
  
  debug_out (menu);
  debug_out ("\nMXCHIP> ");
  
  while(1) {
      Main_Menu();
    if(configSuccess){
      wNetConfig.wifi_mode = Station;
      wNetConfig.dhcpMode = DHCP_Client;
      wNetConfig.wifi_retry_interval = 100;
      StartNetwork(&wNetConfig);
      debug_out("connect to %s.....\r\n", wNetConfig.wifi_ssid);
      configSuccess = 0;
      debug_out ("\nMXCHIP> ");
    }
  }
}
