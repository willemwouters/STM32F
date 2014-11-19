#include "ds2482.h"

int ResetMaster(I2C_TypeDef* I2Cx)
{
  uint8_t temp = 0;
  
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
      
  I2C_GenerateSTART(I2Cx, ENABLE);    
  
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(I2Cx, 0x3E, I2C_Direction_Transmitter);

  while(!I2C_CheckEvent(I2Cx,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  I2C_SendData(I2Cx,0xF0);
  
  while(!I2C_CheckEvent(I2Cx,  I2C_EVENT_MASTER_BYTE_TRANSMITTING));
  
  I2C_GenerateSTART(I2Cx, ENABLE);
  
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(I2Cx, 0x3F, I2C_Direction_Transmitter);///

  while(!I2C_CheckEvent(I2Cx,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  temp = I2C_ReceiveData(I2Cx);

  I2C_GenerateSTOP(I2Cx, ENABLE);
  
  return 1;
}