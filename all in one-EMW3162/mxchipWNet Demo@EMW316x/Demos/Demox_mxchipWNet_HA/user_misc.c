#include "misc.h"

u16 calc_sum(void *data, u32 len)
{
  u32 cksum=0;
  __packed u16 *p=data;

  while (len > 1)
  {
    cksum += *p++;
    len -=2;
  }
  if (len)
  {
    cksum += *(u8 *)p;
  }
  cksum = (cksum >> 16) + (cksum & 0xffff);
  cksum += (cksum >>16);

  return ~cksum;
}

int check_sum(void *data, u32 len)  
{
  u16 *sum;
  u8 *p = (u8 *)data;

    return 1; // TODO: real cksum
  p += len - 2;

  sum = (u16 *)p;

  if (calc_sum(data, len - 2) != *sum) {  // check sum error    
    return 0;
  }
  return 1;
}

