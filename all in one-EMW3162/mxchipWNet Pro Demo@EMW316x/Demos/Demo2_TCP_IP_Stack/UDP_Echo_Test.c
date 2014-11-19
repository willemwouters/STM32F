#include "stdio.h"

#include "main.h"
#include "mxchipWNET.h"
#include "mico_rtos.h"
#include "platform.h"

void udp_echo_thread(void *arg)
{
  int fd_udp = -1, con;
  char *buf;
  struct timeval_t t;
  fd_set readfds;
  struct sockaddr_t addr;
  socklen_t addrLen;
  
  buf = (char*)malloc(3*1024);
  t.tv_sec = 10;
  t.tv_usec = 0;
  
  
  /*Establish a UDP port to receive any data sent to this port*/
  if (fd_udp==-1) {
    fd_udp = socket(AF_INET, SOCK_DGRM, IPPROTO_UDP);
    addr.s_ip = INADDR_ANY;
    addr.s_port = 8090;
    bind(fd_udp, &addr, sizeof(addr));
    debug_out("UDP Echo test: Open UDP port %d\r\n", addr.s_port);
  }
  
  while(1){
    /*Check status on erery sockets */
    FD_ZERO(&readfds);	
    FD_SET(fd_udp, &readfds);	
    
    select(1, &readfds, NULL, NULL, &t);
    
    /*Read data from udp and send data back */ 
    if (FD_ISSET(fd_udp, &readfds)) {
      con = recvfrom(fd_udp, buf, 3*1024, 0, &addr, &addrLen);
      sendto(fd_udp, buf, con, 0, &addr, sizeof(struct sockaddr_t));
    }
  }
}