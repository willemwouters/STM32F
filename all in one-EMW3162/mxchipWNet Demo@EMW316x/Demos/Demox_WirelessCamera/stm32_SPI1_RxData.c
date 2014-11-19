/************************************************************
*															*
*			AVEO Technology Corp.							*
*		Copyright (c) 2012.All rights Reserved				*
*															*
*	Project		:   stm32 BootCode					        *
*	Device		:   stm32 All Types						    *
*	File		:   stm32_SPI1_RxData.c					    *
*	Contents	:   Main FrameWork							*
*	Written by	:   Paul zhang								*
*	Date		:   2012-10-30								*
*	Release Ver	:   Version 001.000							*
*															*
*************************************************************/
#include "stm32f2xx.h"
#include "stm32_SPI1_RxData.h"
#include "stm32_Config_Codec.h"
#include <stdio.h>

extern uint8_t CodecRxBuffer[BLOCK_CNT*BLOCK_LEN];

void SPIData_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE );

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);

	/* Use Alternate Functions for SPI pins */
	GPIO_PinAFConfig( GPIOA,  GPIO_PinSource5,  GPIO_AF_SPI1	);
	//L.GPIO_PinAFConfig( GPIOA,  GPIO_PinSource6,  GPIO_AF_SPI1	);
	GPIO_PinAFConfig( GPIOA,  GPIO_PinSource7,  GPIO_AF_SPI1	);
	GPIO_PinAFConfig( GPIOA,  GPIO_PinSource4,  GPIO_AF_SPI1	);

	/* Setup pin types */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	GPIO_InitStructure.GPIO_Pin   = SPIDATA_CS_PIN;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin   = SPIDATA_SCK_PIN;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin   = SPIDATA_MOSI_PIN;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	SPI_I2S_DeInit(SPIDATA_PORT);
	SPI_Cmd(SPIDATA_PORT, DISABLE);

	SPI_StructInit(&SPI_InitStructure);

	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_RxOnly;	/* 数据方向：2线只接收 */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;		/* STM32的SPI工作模式 ：从机模式 */
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	/* 数据位长度 ： 8位 */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;			/* 时钟下降沿采样数据 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		/* 时钟的第1个边沿采样数据 */
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//SPI_NSS_Hard; //SPI_NSS_Soft;			/* 片选控制方式：软件控制 */
	//L.SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;	/* 波特率预分频系数：4分频 */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	/* 数据位传输次序：高位先传 */
	SPI_InitStructure.SPI_CRCPolynomial = 7;			/* CRC多项式寄存器，复位后为7。本例程不用 */
	SPI_Init(SPIDATA_PORT, &SPI_InitStructure);

	/* Enable the SPI peripheral */
	SPI_I2S_DMACmd(SPIDATA_PORT,SPI_I2S_DMAReq_Rx,ENABLE);
	SPI_Cmd(SPIDATA_PORT, ENABLE);		/* enable SPI1 module  */

	SPIData_DMA_Init((u32)CodecRxBuffer);
}




void SPIData_DMA_Init(u32 m_addr)
{
	/* Setup DMA for SPI RX */
    DMA_InitTypeDef dma_init_structure;

	//NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_DeInit( DMA2_Stream0 );
	dma_init_structure.DMA_Channel = SPIDATA_SLAVE_Rx_DMA_Channel;
	dma_init_structure.DMA_PeripheralBaseAddr = (uint32_t) &SPIDATA_PORT->DR;
	dma_init_structure.DMA_Memory0BaseAddr = m_addr;
	dma_init_structure.DMA_DIR = DMA_DIR_PeripheralToMemory;
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
	DMA_Init( SPIDATA_SLAVE_Rx_DMA_STREAM, &dma_init_structure );

//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_Init(&NVIC_InitStructure);

	//DMA_ITConfig(SPIDATA_SLAVE_Rx_DMA_STREAM, SPIDATA_SLAVE_Rx_DMA_IT, ENABLE);
}

void SPIData_DMA_Enable(FunctionalState isEnable)
{
	DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
	DMA_Cmd(DMA2_Stream0,isEnable);
}



