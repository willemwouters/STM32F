
extern uint8_t CodecRxBuffer[BLOCK_CNT*BLOCK_LEN];
extern uint8_t CodecSendBuffer[BLOCK_CNT*(BLOCK_LEN + 2)];

void InitCamera(void);
void ResetCamera(void);

