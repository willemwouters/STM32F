#ifndef __MAIN_H
#define __MAIN_H

#define DEFAULT_AP_NAME           "MXCHIP_RD"
#define DEFAULT_AP_PASSWORD       "stm32f215"


void tcp_server_thread(void *arg);
void udp_echo_thread(void *arg);
void tcp_client_thread(void *arg);
void udp_multicast_thread(void *arg);
void web_server_thread(void *arg);





#endif /* __MAIN_H */
