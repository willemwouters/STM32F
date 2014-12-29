#include <stdio.h>
#include "flash_configurations.h"


void readConfig(config_t *pConfig)
{
	u32 configInFlash;
	configInFlash = PARA_START_ADDRESS;
	memcpy(pConfig, (void *)configInFlash, sizeof(config_t));
	if(pConfig->magicNumber != MAGIC_FLAG){
		restoreConfig();
		NVIC_SystemReset();	
	}
}


void updateConfig(config_t *pConfig)
{
	uint32_t paraStartAddress, paraEndAddress;
	
	paraStartAddress = PARA_START_ADDRESS;
	paraEndAddress = PARA_END_ADDRESS;
	
	FLASH_If_Init();
	FLASH_If_Erase(paraStartAddress, paraEndAddress);		// 擦除16K 数据保存区;	
	FLASH_If_Write(&paraStartAddress, (u32 *)pConfig, sizeof(config_t));
	FLASH_Lock();
}


void restoreConfig(void)
{	
	config_t	config;
	net_para_st netPara;
	uint32_t paraStartAddress, paraEndAddress;
	paraStartAddress = PARA_START_ADDRESS;
	paraEndAddress = PARA_END_ADDRESS;
	
	getNetPara(&netPara, Station);
	memset(&config, 0x0, sizeof(config_t));
	config.magicNumber = MAGIC_FLAG;
	//sprintf((char*)config.wifi_ssid , "MXCHIP_%s", netPara.mac+6);	
	sprintf((char*)config.wifi_ssid , "");	
	sprintf((char*)config.wifi_key , "");	
	
	FLASH_If_Init();
	FLASH_If_Erase(paraStartAddress, paraEndAddress);		// 擦除16K 数据保存区;	
	FLASH_If_Write(&paraStartAddress, (void *)&config, sizeof(config_t));
	FLASH_Lock();
}
