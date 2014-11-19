#include "stdio.h"

#include "main.h"
#include "mxchipWNET.h"
#include "mico_rtos.h"
#include "platform.h"

extern int wifi_up;
int http_need_connect = 1;

#define WEB_SERVER	  "192.168.2.254"

const char http_Request[]="GET / HTTP/1.1\r\n\
Host: www.baidu.com\r\n\
  User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_8_4) AppleWebKit/536.30.1 (KHTML, like Gecko) Version/6.0.5 Safari/536.30.1\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n\
Accept-Language: zh-cn\r\n\
Accept-Encoding: \r\n\
Connection: keep-alive\r\n\
\r\n\
";

void tcp_client_thread(void *arg)
{
  int fd_client = -1;
  char *buf, ipstr[32];
  int con = -1;
  fd_set readfds;
  fd_set sendfds;
  struct timeval_t t;
  struct sockaddr_t addr;
  int opt=0; 
  int opt_len = sizeof(opt);
  
  buf = (char*)malloc(3*1024);
  
  t.tv_sec = 1;
  t.tv_usec = 0;
  
  while(1) {    
    /*If wifi is established, connect to www.baidu.com, and send a http request*/
    if(wifi_up&&http_need_connect){
      http_need_connect = 0;
      debug_out("TCP client test: Connecting to web server %s\r\n", WEB_SERVER);
      if(gethostbyname(WEB_SERVER, (u8 *)ipstr, 32) != 0){
        debug_out("TCP client test: %s failed! Redo DNS in 5 sec... \r\n", WEB_SERVER);	
        http_need_connect = 1;
        sleep(5);
        continue;
      }

      debug_out("TCP client test: %s address is %s \r\n", WEB_SERVER, ipstr);			
      fd_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      addr.s_ip = inet_addr(ipstr); 
      addr.s_port = 8081;
      if (connect(fd_client, &addr, sizeof(addr))!=0) {
        close(fd_client);
        fd_client = -1;
        http_need_connect = 1;
        debug_out("TCP client test: Connect to web server failed! Reconnect in 5 sec...\r\n");
        sleep(5);
        continue;
      }
      else{
        debug_out("TCP client test: Connect to web server success! Reading web pages...\r\n");
        send(fd_client, http_Request, sizeof(http_Request), 0);
      }
    }	
    
    /*Reset tcp client connection if wifi is down.*/
    if(wifi_up == 0 && fd_client!=-1){
      debug_out("TCP client test: Wi-Fi down, clean TCP sockets resource.\r\n");
      close(fd_client);
      fd_client = -1;
      http_need_connect = 1;
      sleep(1);
      continue;
    }
    
    /*Check status on erery sockets */
    FD_ZERO(&readfds);	
    FD_ZERO(&sendfds);	
    
    if(fd_client!=-1){
      FD_SET(fd_client, &readfds);
      FD_SET(fd_client, &sendfds);
    }
    
    select(1, &readfds, &sendfds, NULL, &t);
    
    /*Read html data from www.baidu.com */
    if(fd_client != -1){
      if(FD_ISSET(fd_client, &sendfds))
      {
        if(send(fd_client, buf, 2*1024, 0)<=0){
          getsockopt(fd_client, SOL_SOCKET, SO_ERROR, &opt, &opt_len);
          debug_out("TCP client test: FD %d Send error, error No. %d.\r\n", fd_client, opt);
          close(fd_client);
          fd_client = -1;
          http_need_connect = 1;
          debug_out("TCP client test: Web connection closed.\r\n");
        }
      }
      if(FD_ISSET(fd_client, &readfds))
      {
        con = recv(fd_client, buf, 2*1024, 0);
        if(con > 0)
          debug_out("TCP client test: Get %s data successful! data length: %d bytes\r\n", WEB_SERVER, con);
        else{
          close(fd_client);
          fd_client = -1;
          http_need_connect = 1;
          debug_out("TCP client test: Web connection closed.\r\n");
        }
      }
    }
  }
}