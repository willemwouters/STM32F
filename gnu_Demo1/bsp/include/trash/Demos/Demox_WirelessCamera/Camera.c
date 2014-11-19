
#include "stm32f2xx.h"
#include "stm32_SPI3_CMD_Port.h"
#include "stm32_SPI1_RxData.h"

#include "mxchipWNET.h"

#include "stm32_Config_Codec.h"

void IO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(AV2862_POWER_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = AV2862_POWER_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(AV2862_POWER_GPIO_PORT, &GPIO_InitStructure);

	AV2862_POWER_ON();

	RCC_AHB1PeriphClockCmd(AV2862_RESET_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = AV2862_RESET_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(AV2862_RESET_GPIO_PORT, &GPIO_InitStructure);
}

void ResetCamera()
{
	AV2862_RESET_ON();
	msleep(300);
	AV2862_RESET_OFF();
	
	msleep(300);
	Set_VideoFraneStart(VGA, BLOCK_LEN);
	msleep(100);
}


uint8_t CodecSendBuffer[BLOCK_CNT*(BLOCK_LEN + 2)] = { 0 };
uint8_t CodecRxBuffer[BLOCK_CNT*BLOCK_LEN] = { 0 };

void InitCameraData()
{
	int ucI, size;
	
	for(ucI=0;ucI < BLOCK_CNT; ucI++)
	{
		size = ucI * (BLOCK_LEN+2);
		CodecSendBuffer[size] = 0xff;
		CodecSendBuffer[size+1] = 0xff;
		CodecSendBuffer[size+2] = 0xff;
		CodecSendBuffer[size+3] = 0xa0;
	}
}


void InitCamera(void)
{
	IO_Init();
	
#if	USE_SPI_CMD_DMA
	SPICmdChn_Init();
#else
	SPICmd_Init();
#endif

	SPIData_Init();

	ResetCamera();

	InitCameraData();
	
	//Set_VideoFraneStart(VGA, BLOCK_LEN);
	//Set_VideoFraneStart(QVGA, BLOCK_LEN);
}
