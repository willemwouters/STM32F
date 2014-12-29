#ifndef __FILE_H__
#define __FILE_H__
#include "stdlib.h"
#include "string.h"
#include "mxchipWNet_TypeDef.h"
#include "stm32f2xx.h"

#define AF_INET 2
#define SOCK_STREAM 1
#define SOCK_DGRM 2 
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
#define SOL_SOCKET   1
#define INADDR_ANY   0
#define INADDR_BROADCAST 0xFFFFFFFF


#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int


#ifndef ssize_t
#define ssize_t unsigned int
#endif

struct sockaddr_t {
   u16        s_type;
   u16        s_port;
   u32    	  s_ip;
   u16        s_spares[6];
} ;

typedef struct in_addr {
   u32 s_addr __attribute__ ((packed));	
} in_addr_t;


struct timeval_t {
	unsigned long		tv_sec;		/* seconds */
	unsigned long		tv_usec;	/* microseconds */
};

typedef long time_t;  

typedef  struct  _timeval_st{ 
    long tv_sec;
    long tv_hmsec;
}TIMEVAL_st; 

#define NULL 0

typedef int socklen_t;

typedef enum
{
  IRQ_TRIGGER_RISING_EDGE  = 0x1, /* Interrupt triggered at input signal's rising edge  */
  IRQ_TRIGGER_FALLING_EDGE = 0x2, /* Interrupt triggered at input signal's falling edge */
  IRQ_TRIGGER_BOTH_EDGES   = IRQ_TRIGGER_RISING_EDGE | IRQ_TRIGGER_FALLING_EDGE,
} gpio_irq_trigger_t;

typedef void (*gpio_irq_handler_t)( void* arg);

typedef enum {
  SO_REUSEADDR = 2,         /* Socket always support this option */
	SO_BROADCAST = 6,		/* Socket always support this option */
    
  IP_ADD_MEMBERSHIP = 3, /* Join Multicast group */
  IP_DROP_MEMBERSHIP = 4, /* Leave Multicast group */
    
	SO_BLOCKMODE = 0x1000,  /* set socket as block/non-block mode, default is block mode */
 	SO_SNDTIMEO = 0x1005,	/* send timeout */
	SO_RCVTIMEO =0x1006	,	/* receive timeout */
	SO_CONTIMEO =0x1007	,	/* connect timeout */
    
  SO_RDBUFLEN = 0x1008,
  SO_WRBUFLEN = 0x1009,    
} SOCK_OPT_VAL;

typedef struct _net_para {
  char dhcp;
	char ip[16]; // such as string  "192.168.1.1"
	char gate[16];
  char mask[16];
	char dns[16];
  char mac[16]; // such as string "7E0000001111"
  char broadcastip[16];
} net_para_st;


 
typedef  struct  _ApList_str  
{  
  char ssid[32];  
  char ApPower;  // min:0, max:100
}ApList_str; 
 
typedef  struct  _ApList_adv 
{ 
  char ssid[32]; 
  char ApPower;  // min:0, max:100
  char bssid[6];
  char channel;
  SECURITY_TYPE_E security;
}ApList_adv_t;

typedef  struct  _ScanResult_adv 
{ 
  char ApNum;       //AP number
  ApList_adv_t * ApList;
} ScanResult_adv; 

typedef  struct  _ScanResult 
{  
  char ApNum;       //AP number
  ApList_str * ApList; 
} ScanResult;  

typedef struct _sta_ap_state{
  int is_connected;
  int wifi_strength;
  u8  ssid[32];
  u8  bssid[6];
  int channel;
}sta_ap_state_t;


typedef  struct  _adv_ap_info  
{  
  char ssid[32];  
  char bssid[6];
  char channel;
  SECURITY_TYPE_E security;
}apinfo_adv_t; 


typedef struct _network_InitTypeDef_st 
{ 
  char wifi_mode;    // SoftAp(0)£¬sta(1)  
  char wifi_ssid[32]; 
  char wifi_key[64]; 
  char local_ip_addr[16]; 
  char net_mask[16]; 
  char gateway_ip_addr[16]; 
  char dnsServer_ip_addr[16]; 
  char dhcpMode;       // disable(0), client mode(1), server mode(2) 
  char address_pool_start[16]; 
  char address_pool_end[16]; 
  int wifi_retry_interval;//sta reconnect interval, ms
} network_InitTypeDef_st; 

typedef struct _network_InitTypeDef_adv_st 
{ 
  apinfo_adv_t ap_info;
  char key[64];
  int  key_len;
  char local_ip_addr[16]; 
  char net_mask[16]; 
  char gateway_ip_addr[16]; 
  char dnsServer_ip_addr[16]; 
  char dhcpMode;       // disable(0), client mode(1), server mode(2) 
  char address_pool_start[16]; 
  char address_pool_end[16]; 
  int wifi_retry_interval;//not used in fastlink
} network_InitTypeDef_adv_st; 


struct wifi_InitTypeDef
{
	u8 wifi_mode;		// adhoc mode(1), AP client mode(0), AP mode(2)
	u8 wifi_ssid[32];
	u8 wifi_key[32];
};

typedef enum {
  MXCHIP_SUCCESS = 0,
  MXCHIP_FAILED = -1,
  MXCHIP_8782_INIT_FAILED = -2,
  MXCHIP_SYS_ILLEGAL = -3,
  MXCHIP_WIFI_JOIN_FAILED = -4,

  MXCHIP_WIFI_UP = 1,
  MXCHIP_WIFI_DOWN,

  MXCHIP_UAP_UP,
  MXCHIP_UAP_DOWN,
} MxchipStatus;

/* Upgrade iamge should save this table to flash */
typedef struct  _boot_table_t {
	u32 start_address; // the address of the bin saved on flash.
	u32 length; // file real length
	u8 version[8];
	u8 type; // B:bootloader, P:boot_table, A:application, D: 8782 driver
	u8 upgrade_type; //u:upgrade, 
	u8 reserved[6];
}boot_table_t;

typedef struct _lib_config_t {
  int tcp_buf_dynamic; // TCP socket buffer: 1= dynamic mode, 0=static mode.
  int tcp_max_connection_num; // TCP max connection number in static mode, should less than 12(default: 12).
  int tcp_rx_size; // TCP read buffer size in static mode (default: 2048). tcp_rx_size*tcp_max_connection_num should<=214576.
  int tcp_tx_size; // TCP send buffer size in static mode (default: 2048). tcp_tx_size*tcp_max_connection_num should<=214576.
  int hw_watchdog; // Watchdag output: EMW3162->PC2, EMW3161->PB14. 1:enable 0:diaable
  WIFI_CHANNEL wifi_channel; // 0:USA(1~11), 1:China(1~13), 2:JP(1~14)
  int nfc_enable; // 1=enable nfc, 0=disable. default is disable.
}lib_config_t;

#define FD_UART         1
#define FD_USER_BEGIN   2
#define FD_SETSIZE      1024 // MAX 1024 fd
typedef unsigned long   fd_mask;

#define NBBY    8               /* number of bits in a byte */
#define NFDBITS (sizeof(fd_mask) * NBBY)        /* bits per mask */

#define howmany(x, y)   (((x) + ((y) - 1)) / (y))

typedef struct fd_set {
        fd_mask   fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define _fdset_mask(n)   ((fd_mask)1 << ((n) % NFDBITS))
#define FD_SET(n, p)     ((p)->fds_bits[(n)/NFDBITS] |= _fdset_mask(n))
#define FD_CLR(n, p)     ((p)->fds_bits[(n)/NFDBITS] &= ~_fdset_mask(n))
#define FD_ISSET(n, p)  ((p)->fds_bits[(n)/NFDBITS] & _fdset_mask(n))
#define FD_ZERO(p)      memset(p, 0, sizeof(*(p)))

#define MSG_DONTWAIT    0x40    /* Nonblocking io  */

/**********************************mxchipWNet library framework**************************************************/
MxchipStatus mxchipInit(void);
void mxchipTick(void);
/****************************************************************************************************************/

/**********************************mxchipWNet library Configuration**********************************************/
char* system_lib_version(void);
void system_version(char *appVersion, int len);        //Callback, user should provide the APP version tp library 
void lib_config(lib_config_t* conf);
void lib_led_enable(LED_TYPE inLed, u8 enable);  //Enable/disable led controller in library
/****************************************************************************************************************/

/**********************************Wi-Fi Configuration***********************************************************/
int OpenConfigmodeWPS(int timeout);
int CloseConfigmodeWPS(void);

int OpenEasylink(int timeout);
int OpenEasylink2(int timeout);
int OpenEasylink2_withdata(int timeout);
int CloseEasylink(void);
int CloseEasylink2(void);
void easylink_user_data_result(int datalen, char* data);  //callback, Return user info from "OpenEasylink2_withdata"
void RptConfigmodeRslt(network_InitTypeDef_st *nwkpara);  //callback, Return SSID and Password
/****************************************************************************************************************/

/**********************************Wi-Fi Scanning****************************************************************/
void mxchipStartScan(void);
void ApListCallback(ScanResult *pApList);
void StartAdvScan(void);
void ApListadv_Callback(ScanResult_adv *pApList);
/****************************************************************************************************************/

/**********************************Wi-Fi Connection**************************************************************/
int wifi_power_down(void);
int wifi_power_up(void);
int StartNetwork(network_InitTypeDef_st* pNetworkInitPara);
int StartAdvNetwork(network_InitTypeDef_adv_st* pNetworkInitParaAdv);
int CheckNetLink(sta_ap_state_t *ap_state);
void WifiStatusHandler(int status);       //callback, return current status once Wi-Fi's status is changed
void connected_ap_info(apinfo_adv_t *ap_info, char *key, int key_len);  //callback, return connected AP info
int wlan_disconnect(void);
int sta_disconnect(void);
int uap_stop(void);
void ps_enable(void);
void ps_disable(void); 
/****************************************************************************************************************/

/**********************************Socket SSL/TLS APIs***********************************************************/
int setSslMaxlen(int len);
int getSslMaxlen(void);
void set_cert(const char *cert_pem, const char*private_key_pem);
int setSSLmode(int enable, int sockfd);
/****************************************************************************************************************/

/**********************************BSD Socket APIs***************************************************************/
int socket(int domain, int type, int protocol);
int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);
int bind(int sockfd, const struct sockaddr_t *addr, socklen_t addrlen);
int connect(int sockfd, const struct sockaddr_t *addr, socklen_t addrlen);
void socket_connected(int sockfd);            //Callback, notify a successful socket connection in unblock mode
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr_t *addr, socklen_t *addrlen);
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval_t *timeout);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t  sendto(int  sockfd,  const  void  *buf,  size_t  len,  int  flags,const  struct  sockaddr_t  *dest_addr, socklen_t addrlen);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
ssize_t recvfrom(int  sockfd,  void  *buf,  size_t  len,  int  flags,struct  sockaddr_t  *src_addr,  socklen_t *addrlen);
int read(int sockfd, void *buf, size_t len); 
int write(int sockfd, void *buf, size_t len); 
int close(int fd);
int tx_buf_size(int sockfd);
/****************************************************************************************************************/

/**********************************TCP/IP Tools******************************************************************/
int getNetPara(net_para_st * pnetpara, WiFi_Interface iface);
void NetCallback(net_para_st *pnet);       //Callback, return the DHCP result once DHCP is success
int gethostbyname(const char * name, u8 * ip_addr, u8 ipLength);    //Start DNS discover in block mode
u32 dns_request(char *hostname);  //Start DNS discover in unblock mode
void dns_ip_set(u8 *hostname, u32 ip);    //Callback, return the NDS result in unblock DNS mode
void set_tcp_keepalive(int num, int seconds);
void get_tcp_keepalive(int *num, int *seconds);
/****************************************************************************************************************/

/**********************************Timer*************************************************************************/
time_t user_time(TIMEVAL_st *t);
int sleep(int seconds);
int msleep(int mseconds);
int SetTimer(unsigned long ms, void (*psysTimerHandler)(void));
/****************************************************************************************************************/

/**********************************System tools******************************************************************/
void WatchDog(void);        //Callback, library call this  function every 20ms
void system_reload(void);   //Excute a software reset
int gpio_irq_enable ( GPIO_TypeDef* gpio_port, uint8_t gpio_pin_number, gpio_irq_trigger_t trigger, gpio_irq_handler_t handler, void* arg );
int gpio_irq_disable( GPIO_TypeDef* gpio_port, uint8_t gpio_pin_number );
/****************************************************************************************************************/


/**********************************System Debugger***************************************************************/
int get_tcp_clients(void);
void memory_status(int *total_free, int *max_len);
void get_malloc_list(void (*debug_mem)(u32 p, int len));
void malloc_list_mem(u8 *memory_tbl, int size);
/****************************************************************************************************************/

/**********************************Programmer's tools************************************************************/
u16 ntohs(u16 n);
u16 htons(u16 n);
u32 ntohl(u32 n);
u32 htonl(u32 n);

//Convert an ip address string in dotted decimal format to  its binary representation.
u32 inet_addr(char *s); 

//Convert a binary ip address to its dotted decimal format. 
//PARAMETER1 's':  location to place the dotted decimal string.  This must be an array of at least 16 bytes.
//PARAMETER2 'x':  ip address to convert.
//
//RETURN VALUE:  returns 's' .
char *inet_ntoa( char *s, u32 x );

//This is function is used to caclute the md5 value for the input buffer
//* input: the source buffer;
//* len: the length of the source buffer;
//* output: point the output buffer, should be a 16 bytes buffer
void md5_hex(u8 *input, u32 len, u8 *output);

//transform src string to hex mode 
//* example: "aabbccddee" => 0xaabbccddee
//* each char in the string must 0~9 a~f A~F, otherwise return 0
//* return the real obuf length
unsigned int str2hex(unsigned char *ibuf, unsigned char *obuf, unsigned int olen);
/****************************************************************************************************************/

/*******************Interrupt routine need by library, they should be excuted in stm32f2xx_it.c*****************/
void NoOS_systick_irq(void);
void gpio_irq(void);
void sdio_irq(void);
void dma_irq(void);
/****************************************************************************************************************/

#endif

