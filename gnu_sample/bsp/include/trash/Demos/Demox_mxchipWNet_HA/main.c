#include "stdio.h"
#include "ctype.h"

#include "stm32f2xx.h"
#include "platform.h"
#include "mxchipWNet_HA.h"
#include "user_misc.h"

#define DynamicMemAlloc          

static u8 hugebuf[1500]; // cmd, fwd data are saved in this buffer

static void uart_tick(void);

void status_changed_cb(mxchip_state_t *cmd)
{
    hal_uart_send_data((u8 *)cmd, sizeof(mxchip_state_t));
}

int uart_cmd_process(u8 *buf, int len)
{
  int control;
  mxchip_cmd_head_t *cmd_header;
  u16 cksum;
    
  cmd_header = (mxchip_cmd_head_t *)buf;

  switch(cmd_header->cmd) {
  case CMD_COM2NET:
    cmd_header->cmd |= 0x8000;
      //wifi_output(&buf[8], len-10);//exclude head & cksum
      wifi_output(buf, len);//send all UART command
      break;
    case CMD_GET_STATUS:
      get_status((mxchip_state_t*)buf);
      hal_uart_send_data(buf, sizeof(mxchip_state_t));
      break;
    case CMD_CONTROL:
      if (cmd_header->datalen != 1)
        break;
      control = buf[8];
      switch(control) {
        case 1: 
					delay_reload();
          break;
        case 2:
          restoreConfiguration();
          delay_reload();
          break;
        case 3:
          wifi_disable();
          break;
        case 4: 
          OpenConfigmodeWPS(120);
          break;
        case 5: // TODO: Easylink
          OpenEasylink(120);
          break;
        default:
          break;
        }
        cmd_header->cmd |= 0x8000;
        cmd_header->cmd_status = 1;
        cmd_header->datalen = 0;
        cksum = calc_sum(buf, 8);
        buf[8] = cksum & 0x00ff;
        buf[9] = (cksum & 0x0ff00) >> 8;
        hal_uart_send_data(buf, 10);
        break;
    default:
      break;
  }
  return 0;
}

static void uart_tick(void)
{
  int recvlen; 

  recvlen = hal_uart_get_one_packet(hugebuf);
  if (recvlen == 0)
    return; 
  uart_cmd_process(hugebuf, recvlen);
}

int main(void)
{
  mxchipWNet_HA_init();
  
  while(1) {
    mxchipWNet_HA_tick();
    uart_tick();
    hal_uart_tx_tick();
  }
}

