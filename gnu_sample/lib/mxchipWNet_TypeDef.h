#ifndef __MXCHIPWNET_TYPEDEF_H__
#define __MXCHIPWNET_TYPEDEF_H__

#define FALSE 0
#define TRUE 1

enum {
	mxDisable,
	mxEnable
} ;

typedef enum {
    WIFI_CHANNEL_1_11 = 0,
    WIFI_CHANNEL_1_13,
    WIFI_CHANNEL_1_14,
} WIFI_CHANNEL;

typedef  enum {
  SECURITY_TYPE_NONE = 0,
  SECURITY_TYPE_WEP,
  SECURITY_TYPE_WPA_TKIP,
  SECURITY_TYPE_WPA_AES,
  SECURITY_TYPE_WPA2_TKIP,
  SECURITY_TYPE_WPA2_AES,
  SECURITY_TYPE_WPA2_MIXED,
  SECURITY_TYPE_AUTO,
} SECURITY_TYPE_E; 

typedef enum {
    LED_INIT = 1,
    LED_STATUS = 2,
    LED_BOTH = 3,
} LED_TYPE;

typedef enum {
  Soft_AP,
	Station
} WiFi_Interface;

typedef enum {
  DHCP_Disable,
  DHCP_Client,
  DHCP_Server
} DHCPOperationMode;


#endif
