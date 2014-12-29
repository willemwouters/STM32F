#include "string.h"
#include "stm32f2xx.h"
#include "platform.h"
#include "flash_if.h"
#include "mxchipWNet_HA.h"


#define MAGIC_FLAG    0x454D0380  

void restoreConfiguration(void)
{ 
  mxchipWNet_HA_st  mxchipWNet_HA_Init;
  uint32_t paraStartAddress, paraEndAddress;
  
  paraStartAddress = PARA_START_ADDRESS;
  paraEndAddress = PARA_END_ADDRESS;
  memset(&mxchipWNet_HA_Init, 0x0, sizeof(mxchipWNet_HA_st));
  mxchipWNet_HA_Init.conf.magicNumber = MAGIC_FLAG;
  mxchipWNet_HA_Init.conf.versionNumber = CONFIGURATION_VERSION;
  mxchipWNet_HA_Init.conf.dhcp_enable = 1;
  strcpy((char*)mxchipWNet_HA_Init.conf.sta_ssid, DEFAULT_STA_SSID);
  strcpy((char*)mxchipWNet_HA_Init.conf.sta_key, DEFAULT_STA_KEY);
  strcpy((char*)mxchipWNet_HA_Init.conf.uap_ssid, DEFAULT_UAP_SSID);
  strcpy((char*)mxchipWNet_HA_Init.conf.uap_key, DEFAULT_UAP_KEY);
  strcpy((char*)mxchipWNet_HA_Init.conf.server_domain, DEFAULT_REMOTE_ADDR);
  mxchipWNet_HA_Init.conf.server_port = DEFAULT_REMOTE_PORT;
  mxchipWNet_HA_Init.conf.fastLinkConf.availableRecord = 0;

  FLASH_If_Init();
  FLASH_If_Erase(paraStartAddress, paraEndAddress);  
  FLASH_If_Write(&paraStartAddress, (void *)&mxchipWNet_HA_Init, sizeof(mxchipWNet_HA_config_st));
  FLASH_Lock();
}


void readConfiguration(mxchipWNet_HA_st *configData)
{
  u32 configInFlash;
  configInFlash = PARA_START_ADDRESS;
  memcpy(&configData->conf, (void *)configInFlash, sizeof(mxchipWNet_HA_config_st));
  if(configData->conf.magicNumber != MAGIC_FLAG||configData->conf.versionNumber != CONFIGURATION_VERSION){
    restoreConfiguration();
    NVIC_SystemReset(); 
  }
}

void updateConfiguration(mxchipWNet_HA_st *configData)
{
  uint32_t paraStartAddress, paraEndAddress;
  
  paraStartAddress = PARA_START_ADDRESS;
  paraEndAddress = PARA_END_ADDRESS;
  
  FLASH_If_Init();
  FLASH_If_Erase(paraStartAddress, paraEndAddress);   
  FLASH_If_Write(&paraStartAddress, (u32 *)&configData->conf, sizeof(mxchipWNet_HA_config_st));
  FLASH_Lock();
}



