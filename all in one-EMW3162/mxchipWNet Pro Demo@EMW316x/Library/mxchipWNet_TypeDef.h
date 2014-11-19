#ifndef __MXCHIPWNET_TYPEDEF_H__
#define __MXCHIPWNET_TYPEDEF_H__

#define FALSE 0
#define TRUE 1

enum {
  mxDisable,
  mxEnable
} ;



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
