#include "stdio.h"
#include "ctype.h"

#include "stm32f2xx.h"
#include "platform.h"
#include "mdns.h"
#include "mxchipWNET_TypeDef.h"
#include "mxchipWNET.h"


#define AP_NAME           "MXCHIP_RD"
#define AP_PASSWORD       "stm32f215"

#define IPPORT_MDNS 5353
#define MDNS_ADDR 0xE00000FB //"224.0.0.251"

int udp_multicast_thread(void)
{
  int mDNS_fd = -1;
  char *buf;
  int con = -1;
  fd_set readfds, exceptfds;
  struct timeval_t t;
  struct sockaddr_t addr;
  socklen_t addrLen;
  u32 opt;
  
  buf = (char*)malloc(3*1024);
  
  mdns_init();
  
  t.tv_sec = 10;
  t.tv_usec = 100;
  
  mDNS_fd = socket(AF_INET, SOCK_DGRM, IPPROTO_UDP);
  opt = MDNS_ADDR;
  setsockopt(mDNS_fd, SOL_SOCKET, IP_ADD_MEMBERSHIP, &opt, 4);
  addr.s_port = IPPORT_MDNS;
  addr.s_ip = INADDR_ANY;
  bind(mDNS_fd, &addr, sizeof(addr));
  
  while(1) {
    /*Check status on erery sockets */
    FD_ZERO(&readfds);
    FD_SET(mDNS_fd, &readfds);	
    select(1, &readfds, NULL, &exceptfds, &t);
    
    /*Read data from udp and send data back */ 
    if (FD_ISSET(mDNS_fd, &readfds)) {
      con = recvfrom(mDNS_fd, buf, 3*1024, 0, &addr, &addrLen);  
      addr.s_ip = MDNS_ADDR;
      addr.s_port = IPPORT_MDNS;
      mdns_handler(mDNS_fd, (u8 *)buf, con);
    }
  }
}

