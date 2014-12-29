/************************************************************
*															*
*			AVEO Technology Corp.							*
*		Copyright (c) 2012.All rights Reserved				*
*															*
*	Project		:   stm32 BootCode					        *
*	Device		:   stm32 All Types						    *
*	File		:   stm32_SPI3_CMD_Port.c				    *
*	Contents	:   Main FrameWork							*
*	Written by	:   Paul zhang								*
*	Date		:   2012-10-30								*
*	Release Ver	:   Version 001.000							*
*															*
*************************************************************/

#include "stm32f2xx.h"
#include "stm32_SPI3_CMD_Port.h"
#include <stdio.h>

/**
* @brief  Main program
* @param  u16Cnt,delay loop count
* @retval None
*/
void SPICmd_Delay(u16 u16Cnt)
{
	uint8_t i;
	uint16_t j;

	for (j = 0; j < u16Cnt; j++)
		for (i = 0; i < 16; i++);
}

void SPICmd_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(SPICMD_GPIO_CLK, ENABLE);		/* enable SPI3 I/O clock*/
	GPIO_InitStructure.GPIO_Pin = SPICMD_CS_PIN | SPICMD_SCK_PIN | SPICMD_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(SPICMD_GPIO_PORT, &GPIO_InitStructure);

	SPICMD_CS_HIGH;
}


void SPICmd_WriteByte(u8 ucValue)
{
	u8 i;
	for (i = 0; i < 8; i++)
	{
		SPICMD_SCK_LOW;  SPICmd_Delay(20);
		if (ucValue & 0x80)	SPICMD_MOSI_HIGH;
		else				SPICMD_MOSI_LOW;
		SPICMD_SCK_HIGH; SPICmd_Delay(20);
		ucValue <<= 1;
	}
}


void SPICmd_brustWrite(u8 *pBuff, u8 len)
{
	u8 i;
	SPICMD_CS_LOW;SPICmd_Delay(200);
	for (i = 0; i < len; i++) {
		SPICmd_WriteByte(*pBuff++);SPICmd_Delay(80);
	}
	SPICmd_Delay(200);
	SPICMD_CS_HIGH;
}


#if	USE_SPI_CMD_DMA
void SPICmdWrite(u8 *pBuff, u8 len)
{
	DMA_SetCurrDataBuffer(SPICMD_Tx_DMA_STREAM, pBuff);
	//set data count
	DMA_SetCurrDataCounter(SPICMD_Tx_DMA_STREAM, len);
	SPICMD_CS_LOW;
	SPICmd_Delay(200);	
	SPICmd_DMA_Enable(ENABLE);
}
#endif

SPI_CodecTypeDef CodecCmd;
void SPICmd_SendCMD(uint8_t CmdType,uint8_t AddrL,uint8_t ValueL,uint16_t Len)
{
	
	CodecCmd.Type = CmdType;
	CodecCmd.Request = 0;
	CodecCmd.AddrH = 0;
	CodecCmd.AddrL = AddrL;
	CodecCmd.ValueH = 0;
	CodecCmd.ValueL = ValueL;
	CodecCmd.LenH = (uint8_t)(Len>>8);
	CodecCmd.LenL = (uint8_t)Len;
	
#if	USE_SPI_CMD_DMA
	SPICmdWrite((uint8_t *)(&CodecCmd), sizeof(CodecCmd));
#else
	SPICmd_brustWrite((uint8_t *)(&CodecCmd), sizeof(CodecCmd));
#endif
}




#if	USE_SPI_CMD_DMA
void SPICmd_DMA_Init(u32 m_addr)
{
	/* Setup DMA for SPI RX */
    DMA_InitTypeDef dma_init_structure;

	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit( SPICMD_Tx_DMA_STREAM );
	dma_init_structure.DMA_Channel = SPICMD_Tx_DMA_Channel;
	dma_init_structure.DMA_PeripheralBaseAddr = (uint32_t) &SPICMD_PORT->DR;
	dma_init_structure.DMA_Memory0BaseAddr = m_addr;
	dma_init_structure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma_init_structure.DMA_BufferSize = 0;
	dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_init_structure.DMA_Mode = DMA_Mode_Normal;
	dma_init_structure.DMA_Priority = DMA_Priority_VeryHigh;
	dma_init_structure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dma_init_structure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	dma_init_structure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma_init_structure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init( SPICMD_Tx_DMA_STREAM, &dma_init_structure );

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//DMA_ITConfig(SPICMD_Tx_DMA_STREAM, SPICMD_Tx_DMA_IT, ENABLE);
}

extern SPI_CodecTypeDef CodecCmd;

void SPICmdChn_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOI, ENABLE);

	/* Use Alternate Functions for SPI pins */
	//GPIO_PinAFConfig( GPIOI,  GPIO_PinSource0,  GPIO_AF_SPI2	);
	GPIO_PinAFConfig( GPIOI,  GPIO_PinSource1,  GPIO_AF_SPI2	);
	GPIO_PinAFConfig( GPIOI,  GPIO_PinSource3,  GPIO_AF_SPI2	);

	/* Setup pin types */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	//GPIO_InitStructure.GPIO_Pin   = SPICMD_CS_PIN;
	//GPIO_Init( SPICMD_GPIO_PORT, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin   = SPICMD_SCK_PIN;
	GPIO_Init( SPICMD_GPIO_PORT, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin   = SPICMD_MOSI_PIN;
	GPIO_Init( SPICMD_GPIO_PORT, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = SPICMD_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(SPICMD_GPIO_PORT, &GPIO_InitStructure);

	SPICMD_CS_HIGH;

	SPI_I2S_DeInit(SPICMD_PORT);
	SPI_Cmd(SPICMD_PORT, DISABLE);

	SPI_StructInit(&SPI_InitStructure);

	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* 数据位长度 ： 8位 */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;			/* 时钟下降沿采样数据 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		/* 时钟的第1个边沿采样数据 */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//SPI_NSS_Hard; //SPI_NSS_Soft;			/* 片选控制方式：软件控制 */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/* 数据位传输次序：高位先传 */
	SPI_InitStructure.SPI_CRCPolynomial = 7;			/* CRC多项式寄存器，复位后为7。本例程不用 */
	SPI_Init(SPICMD_PORT, &SPI_InitStructure);

	/* Enable the SPI peripheral */
	SPI_I2S_DMACmd(SPICMD_PORT,SPI_I2S_DMAReq_Tx,ENABLE);
	SPI_Cmd(SPICMD_PORT, ENABLE);		/* enable SPI1 module  */


//	SPI1_DMA_DeInit(SPI1_SLAVE_Rx_DMA_Channel);
	//SPIData_DMA_Init((u32)CodecRxBuffer);
	SPICmd_DMA_Init((u32)&CodecCmd);
}

void SPICmd_DMA_Enable(FunctionalState isEnable)
{
	DMA_Cmd(SPICMD_Tx_DMA_STREAM, isEnable);
}

#endif
