/************************************************************
*															*
*			AVEO Technology Corp.							*
*		Copyright (c) 2012.All rights Reserved				*
*															*
*	Project		:   stm32 BootCode					        *
*	Device		:   stm32 All Types						    *
*	File		:   stm32_config_Codec.h				    *
*	Contents	:   Main FrameWork							*
*	Written by	:   Paul zhang								*
*	Date		:   2012-10-30								*
*	Release Ver	:   Version 001.000							*
*															*
*************************************************************/

#ifndef _stm32_Config_Codec_H_
#define _stm32_Config_Codec_H_

#define Codec_Standby_Mode				0x10
#define Codec_Soft_Reset				0x11
#define Codec_Set_VCL_Window			0x12
#define Codec_Set_Image_Format			0x13

#define Codec_Write_Sensor_Register		0x20
#define Codec_Read_Sensor_Register		0x21
#define Codec_Write_Chip_Register		0x22
#define Codec_Read_Chip_Register		0x23
#define Codec_Write_EEPROM				0x24
#define Codec_Read_EEPROM				0x25

#define Codec_Send_Video_Data			0x40
#define Codec_Set_Video_Length			0x41
#define Codec_Video_Capture_Start		0x42
#define Codec_Video_Capture_Stop		0x43
#define Codec_Video_Preview_Start		0x44
#define Codec_Video_Preview_Stop		0x45

#define Codec_Send_Audio_Data			0x30
#define Codec_Set_Audio_Length			0x31
#define Codec_Audio_Capture_Start		0x32
#define Codec_Audio_Capture_Stop		0x33

#define Codec_Read_Helper_fw				0x50
#define Codec_Read_Real_fw					0x51

typedef enum
{
	VGA   = 1,
	QQVGA = 2,
	QVGA  = 3,
	QCIF  = 4,
	CIF   = 5
}VIDEOSIZE;

typedef enum
{
	YUV420 = 0,
	YUV422 = 1
} JPGFMT;

//#define BLOCK_CNT	32
//#define BLOCK_CNT	5
#define BLOCK_CNT	10
//#define BLOCK_LEN	256
#define BLOCK_LEN	258

void Set_VideoFraneStart(u8 u8Frame,u16 u16DMALen);
void Set_Read_VideoLen(u16 u16Len);
//int Set_Read_CodecData(u8 blockcnt);
int Set_Read_CodecData(u32 blockcnt);
void Set_Read_CodecDataC(void);
int CheckReadFinish(void);

//#define TEST_SPI_CONTINUE			1
#define TEST_SPI_CONTINUE			0
#define TEST_SPI_NOMEMCPY			0

#endif

