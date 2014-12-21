/************************************************************
*															*
*			AVEO Technology Corp.							*
*		Copyright (c) 2012.All rights Reserved				*
*															* 
*	Project		:   stm32 BootCode					        *
*	Device		:   stm32 All Types						    *
*	File		:   stm32_SPI3_CMD_Port.h				    *
*	Contents	:   Main FrameWork							*
*	Written by	:   Paul zhang								*
*	Date		:   2012-10-30								*
*	Release Ver	:   Version 001.000							*
*															*
*************************************************************/

#ifndef _stm32_SPI3_CMD_Port_H
#define _stm32_SPI3_CMD_Port_H


typedef struct spicodectype
{
	uint8_t Type;
	uint8_t Request;
	uint8_t AddrH;
	uint8_t AddrL;
	uint8_t ValueH;
	uint8_t ValueL;
	uint8_t LenH;
	uint8_t LenL;
} SPI_CodecTypeDef;


// SPI Cmd I/O  define
#define SPICMD_GPIO_PORT 				GPIOI
#define SPICMD_GPIO_CLK 				RCC_AHB1Periph_GPIOI

#define	SPICMD_CS_PIN		 			GPIO_Pin_0
#define SPICMD_CS_GPIO_PORT			GPIOI
#define SPICMD_CS_GPIO_CLK				RCC_AHB1Periph_GPIOI

#define	SPICMD_SCK_PIN		 			GPIO_Pin_1
#define SPICMD_SCK_GPIO_PORT			GPIOI
#define SPICMD_SCK_GPIO_CLK			RCC_AHB1Periph_GPIOI

#define	SPICMD_MOSI_PIN		 		GPIO_Pin_3
#define SPICMD_MOSI_GPIO_PORT			GPIOI
#define SPICMD_MOSI_GPIO_CLK			RCC_AHB1Periph_GPIOI


#define SPICMD_PORT						SPI2

#define SPICMD_Tx_DMA_FLAG				DMA_FLAG_TCIF4
#define SPICMD_Tx_DMA_IT				DMA_IT_TC

#define SPICMD_Tx_DMA_STREAM			DMA1_Stream4
#define SPICMD_Tx_DMA_Channel			DMA_Channel_0

#define SPICMD_DR_Base             	0x4000380C


#define SPICMD_CS_HIGH					GPIO_SetBits(SPICMD_CS_GPIO_PORT, SPICMD_CS_PIN)
#define SPICMD_CS_LOW					GPIO_ResetBits(SPICMD_CS_GPIO_PORT, SPICMD_CS_PIN)

#define SPICMD_SCK_HIGH				GPIO_SetBits(SPICMD_SCK_GPIO_PORT, SPICMD_SCK_PIN)
#define SPICMD_SCK_LOW					GPIO_ResetBits(SPICMD_SCK_GPIO_PORT, SPICMD_SCK_PIN)

#define SPICMD_MOSI_HIGH				GPIO_SetBits(SPICMD_MOSI_GPIO_PORT, SPICMD_MOSI_PIN)
#define SPICMD_MOSI_LOW				GPIO_ResetBits(SPICMD_MOSI_GPIO_PORT, SPICMD_MOSI_PIN)


#define	AV2862_POWER_PIN		 		GPIO_Pin_2
#define AV2862_POWER_GPIO_PORT			GPIOA
#define AV2862_POWER_GPIO_CLK			RCC_AHB1Periph_GPIOA

#define AV2862_POWER_OFF()				GPIO_SetBits(AV2862_POWER_GPIO_PORT, AV2862_POWER_PIN)
#define AV2862_POWER_ON()				GPIO_ResetBits(AV2862_POWER_GPIO_PORT, AV2862_POWER_PIN)

#define	AV2862_RESET_PIN		 		GPIO_Pin_15
#define AV2862_RESET_GPIO_PORT			GPIOH
#define AV2862_RESET_GPIO_CLK			RCC_AHB1Periph_GPIOH

#define AV2862_RESET_OFF()				GPIO_SetBits(AV2862_RESET_GPIO_PORT, AV2862_RESET_PIN)
#define AV2862_RESET_ON()				GPIO_ResetBits(AV2862_RESET_GPIO_PORT, AV2862_RESET_PIN)

void SPICmd_Delay(u16 u16Cnt);
void SPICmd_Init(void);
void SPICmd_brustWrite(u8 *pBuff, u8 len);
void SPICmd_SendCMD(uint8_t CmdType,uint8_t AddrL,uint8_t ValueL,uint16_t Len);
void SPICmd_DMA_Enable(FunctionalState isEnable);
void SPICmdWrite(u8 *pBuff, u8 len);
void SPICmdChn_Init(void);

#define USE_SPI_CMD_DMA				0



#endif

