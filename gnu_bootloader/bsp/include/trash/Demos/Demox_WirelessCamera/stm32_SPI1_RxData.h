/************************************************************
*															*
*			AVEO Technology Corp.							*
*		Copyright (c) 2012.All rights Reserved				*
*															*
*	Project		:   stm32 BootCode					        *
*	Device		:   stm32 All Types						    *
*	File		:   stm32_SPI1_RxData.h					    *
*	Contents	:   Main FrameWork							*
*	Written by	:   Paul zhang								*
*	Date		:   2012-10-30								*
*	Release Ver	:   Version 001.000							*
*															*
*************************************************************/

// SPI Data I/O  define

#define SPIDATA_PORT						SPI1
#define SPIDATA_CLK							RCC_APB2Periph_SPI1
#define SPIDATA_GPIO_PORT 					GPIOA
#define SPIDATA_GPIO_CLK 					RCC_AHB1Periph_GPIOA

#define	SPIDATA_CS_PIN		 			    GPIO_Pin_4
#define SPIDATA_CS_GPIO_PORT				GPIOA
#define SPIDATA_CS_GPIO_CLK				RCC_AHB1Periph_GPIOA

#define	SPIDATA_SCK_PIN		 			GPIO_Pin_5
#define SPIDATA_SCK_GPIO_PORT				GPIOA
#define SPIDATA_SCK_GPIO_CLK				RCC_AHB1Periph_GPIOA

#define	SPIDATA_MOSI_PIN		 			GPIO_Pin_7
#define SPIDATA_MOSI_GPIO_PORT				GPIOA
#define SPIDATA_MOSI_GPIO_CLK				RCC_AHB1Periph_GPIOA

#define	SPIDATA_MISO_PIN		 			GPIO_Pin_6
#define SPIDATA_MISO_GPIO_PORT				GPIOA
#define SPIDATA_MISO_GPIO_CLK				RCC_AHB1Periph_GPIOA

#define SPIDATA_SLAVE_Rx_DMA_FLAG			DMA_FLAG_TCIF0
#define SPIDATA_SLAVE_Rx_DMA_IT			DMA_IT_TC

#define SPIDATA_SLAVE_Rx_DMA_STREAM		DMA2_Stream0
#define SPIDATA_SLAVE_Rx_DMA_Channel		DMA_Channel_3

#define SPI_SLAVE_DR_Base             	0x4001300C

void SPIData_Init(void);
void SPIData_Writebyte(u8 _ucValue);
u8 SPIData_ReadByte(void);
u8 SPIData_BrustRead(u8 *pBuff, u32 len);

void SPIData_DMA_Init(u32 m_addr);
void SPIData_DMA_Config(u32 p_addr,u32 m_addr, u16 count);
void SPIData_DMA_Enable(FunctionalState isEnable);
void Set_SPIData_DMA_Read(u32 p_addr,u32 m_addr, u16 u16Count);
