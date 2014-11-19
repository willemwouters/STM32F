/**
  ******************************************************************************
  * @file    ssl_server.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    07-October-2011
  * @brief   This file contains all the functions prototypes for the ssl_server.c 
  *          file.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SSL_SERVER_H
#define __SSL_SERVER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "polarssl/ssl.h"

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint32_t State;
}rng_state;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void ssl_server(void *pvParameters);
void ssl_DynPage(ssl_context *ssl);
void ssl_sendframes(ssl_context *ssl, char *data, int datalen);
int RandVal(void* arg);

#ifdef __cplusplus
}
#endif

#endif /* __SSL_SERVER_H */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
