#include "stdio.h"

#include "main.h"
#include "mxchipWNET.h"
#include "mico_rtos.h"
#include "platform.h"

void tcp_server_thread(void *arg)
{
  int i, j,con, len, fd_listen = -1;
  struct sockaddr_t addr;
  fd_set readfds;
  struct timeval_t t;
  char *buf, ip_address[16];
  int clientfd[8];
  
  for(i=0;i<8;i++) 
    clientfd[i] = -1;
  
  buf = (char*)malloc(3*1024);
  
  t.tv_sec = 1;
  t.tv_usec = 0;
  
  /*Establish a TCP server that accept the tcp clients connections*/
  if (fd_listen==-1) {
    fd_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    addr.s_ip = INADDR_ANY;
    addr.s_port = 8080;
    bind(fd_listen, &addr, sizeof(addr));
    listen(fd_listen, 0);
    debug_out("TCP server test: TCP server established at port: %d\r\n", addr.s_port);
  }

  
  while(1){
    /*Check status on erery sockets */
    FD_ZERO(&readfds);
    FD_SET(fd_listen, &readfds);	
    for(i=0;i<8;i++) {
      if (clientfd[i] != -1)
        FD_SET(clientfd[i], &readfds);
    }
    
    select(1, &readfds, NULL, NULL, &t);
    
    /*Check tcp connection requests */
    if(FD_ISSET(fd_listen, &readfds)){
      j = accept(fd_listen, &addr, &len);
      if (j > 0) {
        inet_ntoa(ip_address, addr.s_ip );
        debug_out("TCP server test: Client %s:%d connected, fd: %d\r\n", ip_address, addr.s_port, j);
        for(i=0;i<8;i++) {
          if (clientfd[i] == -1) {
            clientfd[i] = j;
            break;
          }
        }
      }
    }
    
    /*Read data from tcp clients and send data back */ 
    for(i=0;i<8;i++) {
      if (clientfd[i] != -1) {
        if (FD_ISSET(clientfd[i], &readfds)) {
          con = recv(clientfd[i], buf, 1*1024, 0);
          if (con > 0) {
            send(clientfd[i], buf, con, 0);
          }
          else {
            close(clientfd[i]);
            debug_out("TCP server test: Client closed, fd: %d\r\n", clientfd[i]);
            clientfd[i] = -1;
          }
        }
      }
    }
  }
}