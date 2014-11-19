#include "stdio.h"
#include "ctype.h"

#include "stm32f2xx.h"
#include "platform.h"
#include "mxchipWNET.h"
#include "mico_rtos.h"
#include "flash_configurations.h"
#include "main.h"

#define APP_INFO          "mxchipWNet Pro Demo: TCP IP Stack demo"

static mico_thread_t tcp_server_thread_handler;
static mico_thread_t udp_echo_thread_handler;
static mico_thread_t tcp_client_thread_handler;
static mico_thread_t udp_multicast_thread_handler;
static mico_thread_t web_server_thread_handler;

static mico_thread_t uart_recv_thread_handler;
static void uart_recv_thread(void *arg);

network_InitTypeDef_st wNetConfig;

int wifi_up = 0;

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
    debug_out("Station up \r\n");
    break;
  case MXCHIP_WIFI_DOWN:
    wifi_up = 0;
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

void NetCallback(net_para_st *pnet)
{
  debug_out("IP address: %s \r\n", pnet->ip);
  debug_out("NetMask address: %s \r\n", pnet->mask);
  debug_out("Gateway address: %s \r\n", pnet->gate);
  debug_out("DNS server address: %s \r\n", pnet->dns);
  debug_out("MAC address: %s \r\n", pnet->mac);
}

void application_start(void)
{ 
  Platform_Init();
  mxchipInit();
  UART_Init();
  
  debug_out("\r\n%s\r\nmxchipWNet library version: %s\r\n", APP_INFO, system_lib_version());
  
#ifdef RFLowPowerMode
  ps_enable();
#endif

#ifdef MCULowPowerMode
  mico_mcu_powersave_config(mxEnable);
#endif	
  
  config_t flash_config;
  restoreConfig();
  readConfig(&flash_config);
  
  memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));
  wNetConfig.wifi_mode = Station;
  strcpy((char*)wNetConfig.wifi_ssid, flash_config.wifi_ssid);
  strcpy((char*)wNetConfig.wifi_key, flash_config.wifi_key);
  wNetConfig.dhcpMode = DHCP_Client;
  wNetConfig.wifi_retry_interval = 5000;
  StartNetwork(&wNetConfig);
  debug_out("Connect to %s.....\r\n", wNetConfig.wifi_ssid);

  mico_rtos_create_thread(&tcp_server_thread_handler, MICO_APPLICATION_PRIORITY, "TCP_SERVER", tcp_server_thread, 0x500, NULL );
  mico_rtos_create_thread(&udp_echo_thread_handler, MICO_APPLICATION_PRIORITY, "UDP_ECHO", udp_echo_thread, 0x500, NULL );
  mico_rtos_create_thread(&tcp_client_thread_handler, MICO_APPLICATION_PRIORITY, "TCP_CLIENT", tcp_client_thread, 0x500, NULL );
  mico_rtos_create_thread(&uart_recv_thread_handler, MICO_APPLICATION_PRIORITY, "UART_RECV", uart_recv_thread, 0x500, NULL );
  mico_rtos_create_thread(&udp_multicast_thread_handler, MICO_APPLICATION_PRIORITY, "UDP_MULTICAST", udp_multicast_thread, 0x500, NULL );
  mico_rtos_create_thread(&web_server_thread_handler, MICO_APPLICATION_PRIORITY, "WEB SERVER", web_server_thread, 0x500, NULL );
    
  mico_rtos_delete_thread(NULL);
}

static void uart_recv_thread(void *arg)
{
  char c;
  while(UART_Recv((u8 *)&c, 1, MICO_NEVER_TIMEOUT) == 0)
    UART_Send((u8 *)&c, 1); 
}

