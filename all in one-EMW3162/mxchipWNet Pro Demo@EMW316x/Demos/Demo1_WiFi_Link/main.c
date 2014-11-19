#include "stdio.h"
#include "ctype.h"

#include "stm32f2xx.h"
#include "platform.h"
#include "mxchipWNET.h"
#include "mico_rtos.h"

#define AP_NAME           "UAT_AP"
#define AP_PASSWORD       "uat12345"

#define APP_INFO          "mxchipWNet Pro Demo: Wi-Fi Link demo"

int wifi_up=0;
static mico_semaphore_t user_srart_scan;
static mico_semaphore_t user_srart_uap;

static mico_thread_t uap_thread_handler;
static void uap_thread(void *arg);

network_InitTypeDef_st wNetConfig;

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
      debug_out("Station up \r\n");
      mico_rtos_set_semaphore(&user_srart_uap);
      break;
    case MXCHIP_WIFI_DOWN:
      debug_out("Station down \r\n");
      break;
    case MXCHIP_UAP_UP:
      debug_out("uAP up \r\n");
      break;
    case MXCHIP_UAP_DOWN:
      debug_out("uAP down \r\n");
      break;
    default:
      break;
  }
  return;
}

void ApListCallback(UwtPara_str *pApList)
{
  int i;
  debug_out("Find %d APs: \r\n", pApList->ApNum);
  for (i=0;i<pApList->ApNum;i++)
    debug_out("    SSID: %s, Signal: %d%%\r\n", pApList->ApList[i].ssid, pApList->ApList[i].ApPower);
}

void NetCallback(net_para_st *pnet)
{
  debug_out("IP address: %s \r\n", pnet->ip);
  debug_out("NetMask address: %s \r\n", pnet->mask);
  debug_out("Gateway address: %s \r\n", pnet->gate);
  debug_out("DNS server address: %s \r\n", pnet->dns);
  debug_out("MAC address: %s \r\n", pnet->mac);
}

void stationModeStart(void)
{
  memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));
	
  wNetConfig.wifi_mode = Station;
  strcpy((char*)wNetConfig.wifi_ssid, AP_NAME);
  strcpy((char*)wNetConfig.wifi_key, AP_PASSWORD);
  wNetConfig.dhcpMode = DHCP_Client;
  wNetConfig.wifi_retry_interval = 20000;
  StartNetwork(&wNetConfig);
  ps_enable();
  debug_out("connect to %s.....\r\n", wNetConfig.wifi_ssid);
}

void softAPModeStart(void)
{
  memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));
	
  wNetConfig.wifi_mode = Soft_AP;
  strcpy((char*)wNetConfig.wifi_ssid, "Soft AP Test");
  strcpy((char*)wNetConfig.wifi_key, "");
  strcpy((char*)wNetConfig.local_ip_addr, "10.10.10.1");
  strcpy((char*)wNetConfig.net_mask, "255.255.255.0");
  strcpy((char*)wNetConfig.gateway_ip_addr, "10.10.10.1");
  strcpy((char*)wNetConfig.address_pool_start, "10.10.10.10");
  strcpy((char*)wNetConfig.address_pool_end, "10.10.10.177");
  wNetConfig.dhcpMode = DHCP_Server;
  StartNetwork(&wNetConfig);
  debug_out("Setup soft AP: %s\r\n", wNetConfig.wifi_ssid);
}

void application_start(void)
{
  mxchipInit(); 
  Platform_Init();
  Button_Init();
  
#ifdef RFLowPowerMode
  ps_enable();
#endif	
  
#ifdef MCULowPowerMode
  mico_mcu_powersave_config(mxEnable);
#endif	
  
  debug_out("\r\n%s\r\nmxchipWNet library version: %s\r\n", APP_INFO, system_lib_version());
  
  mico_rtos_init_semaphore(&user_srart_scan, 1);
  mico_rtos_init_semaphore(&user_srart_uap, 1);
  mico_rtos_create_thread( &uap_thread_handler, MICO_APPLICATION_PRIORITY, "UAP", uap_thread, 0x200, NULL );

  debug_out("Start scan\r\n");
  mxchipStartScan();
  stationModeStart();
  
  while(1){
    if(mico_rtos_get_semaphore(&user_srart_scan, MICO_WAIT_FOREVER)==MXCHIP_SUCCESS){
      debug_out("Starting a user scan......");
      mxchipStartScan();
    }
  }
}

static void uap_thread(void *arg)
{
  if(mico_rtos_get_semaphore(&user_srart_uap, MICO_WAIT_FOREVER)==MXCHIP_SUCCESS){
    softAPModeStart();
  }
  mico_rtos_delete_thread(NULL);
}

void Button_irq_handler(void *arg)
{
  debug_out("User external interrupt!\r\n");
  mico_rtos_set_semaphore(&user_srart_scan);
}


