#ifndef __MXCHIPWNET_HA_H
#define __MXCHIPWNET_HA_H

#include "stm32f2xx.h"
#include "mxchipWNet.h"

#define FW_VERSION 0x31621101

#define CONFIGURATION_VERSION 0x0000003 // if changed default configuration, add this num

#define DEFAULT_UAP_SSID "MXCHIP_HA"
#define DEFAULT_UAP_KEY ""
#define DEFAULT_STA_SSID "William's Airport"
#define DEFAULT_STA_KEY "mx099555"
#define DEFAULT_REMOTE_ADDR "172.16.2.5"
#define DEFAULT_REMOTE_PORT 10000
#define TCP_SERVER_PORT 10000 //WIFI create a TCP server listen on this port
#define UDP_BROADCAST_PORT 8090 // UDP broadcast port

#define UAP_IP_ADDRESS "10.10.10.1" // This is the uAP IP address
#define UAP_MASK_ADDRESS "255.255.255.0" 
#define UAP_GW_ADDRESS "10.10.10.1" 
#define UAP_DNS_ADDRESS "10.10.10.1"
#define UAP_DHCP_SERVER_START "10.10.10.100" 
#define UAP_DHCP_SERVER_END "10.10.10.200"

#define CLOUD_ENABLE 1
#define CLOUD_RETRY 30*ONE_SECOND
#define UAP_START_TIME 20*ONE_SECOND // uAP start time. If wifi module can't connect to AP for 20 seconds, uAP is startup.

enum {
  CMD_READ_VERSION = 1, 
  CMD_READ_CONFIG, 
  CMD_WRITE_CONFIG, 
  CMD_SCAN, 
  CMD_OTA, 
  CMD_NET2COM,
  CMD_COM2NET, 
  CMD_GET_STATUS, 
  CMD_CONTROL,    
  CMD_SEARCH, 
};

typedef struct _mxchip_cmd_head {
  u16 flag; // Allways BB 00
  u16 cmd; // commands, return cmd=cmd|0x8000
  u16 cmd_status; //return result
  u16 datalen; 
  u8 data[1]; 
}mxchip_cmd_head_t;

#pragma pack(1)
typedef struct _current_state_ {
  u32 uap_state;
  u32 sta_state;
  u32 tcp_client;
  u32 signal;
  u8 ip[16];
  u8 mask[16];
  u8 gw[16];
  u8 dns[16];
  u8 mac[18];
}current_state_t;
#pragma pack()

typedef struct _mxchip_state_ {
  u16 flag; 
  u16 cmd; 
  u16 cmd_status; 
  u16 datalen; 
  current_state_t status;
  u16 cksum;
}mxchip_state_t;

typedef struct _fast_link_st {
  int availableRecord;
  ApList_adv_t ap_info;
  char key[64];
  int key_len;
}fast_link_st;

typedef struct _mxchipWNet_config_st {
  /*OTA options*/
  boot_table_t bootTable;
  /*User options*/
  u32 magicNumber;
  u32 versionNumber;

  char uap_ssid[32];
  char uap_key[64];
  char sta_ssid[32];  
  char sta_key[64]; 
 
  u8 server_domain[64];
  u32 server_port;
  u32 dhcp_enable;
  u8 ip[16];
  u8 mask[16];
  u8 gw[16];
  u8 dns[16];
  
  fast_link_st fastLinkConf;
}mxchipWNet_HA_config_st;



#define CONFIG_DATA_SIZE (sizeof(mxchipWNet_HA_config_st)-8-sizeof(boot_table_t)-sizeof(fast_link_st))

typedef struct _mxchipWNet_st
{
  mxchipWNet_HA_config_st conf; 
  
  current_state_t status;
  int *tcpClient_fd;
  int tcpServer_fd;
  int cloud_fd;
  int udpSearch_fd;
}  mxchipWNet_HA_st;



/* ======MXCHIP lib API ==========*/
/* Sysytem initilize */
void mxchipWNet_HA_init(void) ;
/* main tick */
void mxchipWNet_HA_tick(void);
/* Get system status */
void get_status(mxchip_state_t*cmd);
/* Reload*/
void system_reload(void);
/* Back to default configuration and reload */
void system_default(void);
/* Start WPS*/
int wps_pbc_start(void);
/* Wifi Power down*/
void wifi_disable(void);
/* Output data by wifi */
void wifi_output(u8*buf, int len);

void delay_reload(void);


/*=====User define callback function ====*/
void status_changed_cb(mxchip_state_t*cmd);
void wps_fail(void);
void wps_success(u8*ssid, u8*key);
void wifi_input(u8*buf, int len);

#endif
