/************************************************************
*															*
*			AVEO Technology Corp.							*
*		Copyright (c) 2012.All rights Reserved				*
*															*
*	Project		:   stm32 BootCode					        *
*	Device		:   stm32 All Types						    *
*	File		:   stm32_config_Codec.c				    *
*	Contents	:   Main FrameWork							*
*	Written by	:   Paul zhang								*
*	Date		:   2012-10-30								*
*	Release Ver	:   Version 001.000							*
*															*
*************************************************************/
#include "stm32f2xx.h"
#include "stm32_SPI3_CMD_Port.h"
#include "stm32_Config_Codec.h"
#include "stm32_SPI1_RxData.h"
#include <stdio.h>

#include "mxchipWNET.h"

void DMA_SetCurrDataBuffer(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t addr)
{
  /* Check the parameters */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));

  /* Write the number of data units to be transferred */
  DMAy_Streamx->M0AR = addr;
}


extern u8 transfer_complete;
extern uint8_t CodecSendBuffer[];

void Set_VideoFraneStart(u8 u8FrameIndex,u16 u16DMALen)
{
	u16 i = 100;
	if(u8FrameIndex == QQVGA)
		SPICmd_SendCMD(Codec_Video_Capture_Start,YUV422,u8FrameIndex,u16DMALen);
	else
		SPICmd_SendCMD(Codec_Video_Capture_Start,YUV420,u8FrameIndex,u16DMALen);
	while(i--)
		SPICmd_Delay(2000);
}
void Set_Read_VideoLen(u16 u16Len)
{
	SPICmd_SendCMD(Codec_Send_Video_Data,0,1,u16Len);
}

int Set_Read_CodecData(u32 blockcnt)
{
	//set receive data buffer
	DMA_SetCurrDataBuffer(SPIDATA_SLAVE_Rx_DMA_STREAM, blockcnt);
	//set data count
	DMA_SetCurrDataCounter(SPIDATA_SLAVE_Rx_DMA_STREAM, BLOCK_CNT*BLOCK_LEN);
  	SPIData_DMA_Enable(ENABLE);

	Set_Read_VideoLen(BLOCK_LEN);
    while(!DMA_GetFlagStatus(SPIDATA_SLAVE_Rx_DMA_STREAM, SPIDATA_SLAVE_Rx_DMA_FLAG));
	DMA_ClearFlag(SPIDATA_SLAVE_Rx_DMA_STREAM, SPIDATA_SLAVE_Rx_DMA_FLAG);

	SPIData_DMA_Enable(DISABLE);
	return 0;
}

char *pRecvAddr;

void Set_Read_CodecDataA(u32 addr)
{
	//set receive data buffer
	pRecvAddr = (char *)addr;
	DMA_SetCurrDataBuffer(SPIDATA_SLAVE_Rx_DMA_STREAM, addr);
	//set data countSPIDATA_SLAVE_Rx_DMA_STREAM
	DMA_SetCurrDataCounter(SPIDATA_SLAVE_Rx_DMA_STREAM, BLOCK_CNT*BLOCK_LEN);

  	SPIData_DMA_Enable(ENABLE);
	Set_Read_VideoLen(BLOCK_CNT*BLOCK_LEN);
}


void Set_Read_CodecDataB(u32 addr)
{
	//set receive data buffer
	pRecvAddr = (char *)addr;
	DMA_SetCurrDataBuffer(SPIDATA_SLAVE_Rx_DMA_STREAM, addr);
	//set data countSPIDATA_SLAVE_Rx_DMA_STREAM
	DMA_SetCurrDataCounter(SPIDATA_SLAVE_Rx_DMA_STREAM, BLOCK_LEN);

  	SPIData_DMA_Enable(ENABLE);
	Set_Read_VideoLen(BLOCK_LEN);
}


void Set_Read_CodecDataC()
{
	//set data count
	DMA_SetCurrDataCounter(SPIDATA_SLAVE_Rx_DMA_STREAM, BLOCK_CNT*BLOCK_LEN);
  	SPIData_DMA_Enable(ENABLE);

	Set_Read_VideoLen(BLOCK_CNT*BLOCK_LEN);

}

#define CAMERA_TIMEOUT		100

int CheckReadFinish()
{
	int iTimeOut;
	int num;

	iTimeOut = CAMERA_TIMEOUT;
	num = DMA_GetCurrDataCounter(SPIDATA_SLAVE_Rx_DMA_STREAM);

	if(num<(BLOCK_CNT*BLOCK_LEN))
	{
		while(iTimeOut)
		{
	    	if(DMA_GetFlagStatus(SPIDATA_SLAVE_Rx_DMA_STREAM, SPIDATA_SLAVE_Rx_DMA_FLAG)!=RESET)
	    	{
				DMA_ClearFlag(SPIDATA_SLAVE_Rx_DMA_STREAM, SPIDATA_SLAVE_Rx_DMA_FLAG);
				iTimeOut = 1;
				break;
	    	}

			iTimeOut--;
			msleep(2);
		}
	}

	else
		iTimeOut = 0;
	
	SPIData_DMA_Enable(DISABLE);

	return iTimeOut;
}


void Set_AudioFraneStart(u8 u8FrameIndex,u16 u16DMALen)
{
	u16 i = 100;
	
	SPICmd_SendCMD(Codec_Audio_Capture_Start,0,0,u16DMALen);

	while(i--)
		SPICmd_Delay(2000);
}


void SPI_CMD_SEND_DMA_ISR()
{
	if(DMA_GetITStatus(SPICMD_Tx_DMA_STREAM, DMA_IT_TCIF4))
	{
		//SPICmd_DMA_Enable(DISABLE);

		DMA_ClearITPendingBit(SPICMD_Tx_DMA_STREAM, (DMA_IT_TCIF4|DMA_IT_TEIF4));

		SPICmd_Delay(200);	
		SPICMD_CS_HIGH;

	}
}

