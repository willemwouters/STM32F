#include "stdio.h"
#include "ctype.h"
#include "platform.h"
#include "mxchipWNet.h"
#include "mxchipWNet_HA.h"
#include "user_misc.h"
#include "flash_if.h"

#define MAX_CLIENT 8
#define MAX_SITES 32
#define ONE_SECOND 1000
#define ONE_MINUTE 60*ONE_SECOND
#define MAX_UPGRADE_TIME 5*ONE_MINUTE// upgrade time should less than 5 minutes.

#define UART_CMD_MAX_LEN 1400
#define UART_CMD_LEN 12
#define IP_CMD_MAX_LEN 1460

#define UART_FRAM_START 0xAA
#define UART_FRAM_END 0x55
#define CONTROL_FLAG 0xBB
#define FRAM_FLAG 0x00BB

#define UAP_START_TIME_SHORT 5*ONE_SECOND
#define STATION_RETRY_SHORT 20*ONE_SECOND
#define SEND_TIMEOUT 200


enum {
  CMD_OK = 0,
  CMD_FAIL,
};

enum {
  STA_CONNECT = 1<<0,
  //UAP_SERVER_START = 1<<1,
  UAP_START = 1<<2,
  REMOTE_CONNECT = 1<<3,
};

enum {
  INIT_STATE = 0, // first 20 seconds until wifi & tcp state changed
  DUAL_MODE_STATE, // After 20 seconds, wifi didn't connected
  NORMAL_WORK_STATE, // tcp connected.
  NORMAL_WORK_RETRY_STATE // TCP/STA disconnected, retry.
};

typedef  struct  _scan_tbl  
{  
    char ssid[32];  
    int rssi;  // sorted by rssi
}scan_tbl_t; 

#pragma pack(1)
typedef struct _upgrade_t {
  u8 md5[16];
  u32 len;
  u8 data[1];
}ota_upgrate_t;

#pragma pack(1)
typedef struct _udp_search {
  u8 ip[16];
  u8 mac[18];
  u8 ver[8]; 
}udp_search_st;


extern vu32 MS_TIMER;
extern vu32 SEC_TIMER;
extern vu32 TICK_TIMER;

static int wifi_disalbed = 0;
static int need_reload = 0;
static int startScan = 0;
static u32 network_state = 0;
static u32 running_state = 0;
static int cloud_ip_addr = 0;
static u32 dns_pending = 0;
static u32 cloud_retry = 0;
static u32 need_report = 0;
static u32 cloud_enable = CLOUD_ENABLE;
static u32 uap_start_time = UAP_START_TIME;
//static u32 sta_retry_interval = STA_RETRY_SHORT;
static int ap_num = 0;


static u8 hugebuf[1600]; // cmd, fwd data are saved in this buffer
mxchipWNet_HA_st  *device_info;


//static void tcp_client_tick(void);
void delay_reload(void);
static void report_state(void);
static void tcp_recv_tick(void);
static void connect_to_cloud(void);
static void close_cloud(int retry_interval);
static int ip_cmd_process(unsigned char *buf, int cmd_len, int socket_fd);
static int search_cmd_process(unsigned char *buf, int cmd_len, const struct sockaddr_t *dest_addr, socklen_t addrlen);
static int ota_process(unsigned char *buf, int cmd_len, int *socket_fd);
void fast_send(int sockfd, const void *buf, size_t len);


int is_wifi_disalbed(void)
{
  return wifi_disalbed;
}

int is_network_state(int state)
{
  if ((network_state & state) == 0)
    return 0;
  else
    return 1;
}

static void report_state()
{
//  mxchip_state_t *cmd = (mxchip_state_t*)hugebuf;
  get_status((mxchip_state_t*)hugebuf);
  status_changed_cb((mxchip_state_t*)hugebuf);
}

void set_network_state(int state, int on)
{
  if (on)
    network_state |= state;
  else {
    network_state &= ~state;
    if (state == STA_CONNECT)
      network_state &= ~REMOTE_CONNECT;
  }
    
  switch(running_state) {
  case INIT_STATE:
  case NORMAL_WORK_RETRY_STATE:
    if (on) {
      if (state == REMOTE_CONNECT)  //connected to remote server
        running_state = NORMAL_WORK_STATE;

      if (state == UAP_START) 
        running_state = DUAL_MODE_STATE;
    }
		else{
      if (state == STA_CONNECT) {
        uap_start_time = MS_TIMER + UAP_START_TIME_SHORT;
      }
		}
    break;
  case DUAL_MODE_STATE:
    if ((state == STA_CONNECT) && on) {
      uap_stop();
      running_state = INIT_STATE;
      //delay_reload();
      //return; // do not report tcp connect, cause we will reboot system.
    }
    break;

  case NORMAL_WORK_STATE:
    if (!on && ((state == STA_CONNECT) || (state == REMOTE_CONNECT))) {
      if (state == STA_CONNECT) {
        uap_start_time = MS_TIMER + UAP_START_TIME_SHORT;
      }
      running_state = NORMAL_WORK_RETRY_STATE;
    }
    break;
    
  default:
    break;
  }
  
  if ((state == STA_CONNECT) || (state == REMOTE_CONNECT))  
    need_report = 1;
}

void delay_reload()
{
  need_reload = 1;
}


void formatMACAddr(void *destAddr, void *srcAddr)
{
  sprintf((char *)destAddr, "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c",\
					toupper(*(char *)srcAddr),toupper(*((char *)(srcAddr)+1)),\
					toupper(*((char *)(srcAddr)+2)),toupper(*((char *)(srcAddr)+3)),\
					toupper(*((char *)(srcAddr)+4)),toupper(*((char *)(srcAddr)+5)),\
					toupper(*((char *)(srcAddr)+6)),toupper(*((char *)(srcAddr)+7)),\
					toupper(*((char *)(srcAddr)+8)),toupper(*((char *)(srcAddr)+9)),\
					toupper(*((char *)(srcAddr)+10)),toupper(*((char *)(srcAddr)+11)));
}

void socket_connected(int fd)
{
  if(fd==device_info->cloud_fd){
    set_network_state(REMOTE_CONNECT, 1);
  }
}

void RptConfigmodeRslt(network_InitTypeDef_st *nwkpara)
{
  if(nwkpara == NULL){
    system_reload();
    }
  else{
    memcpy(device_info->conf.sta_ssid, nwkpara->wifi_ssid, sizeof(device_info->conf.sta_ssid));
    memcpy(device_info->conf.sta_key, nwkpara->wifi_key, sizeof(device_info->conf.sta_key));
    /*Clear fastlink record*/
    memset(&(device_info->conf.fastLinkConf), 0x0, sizeof(fast_link_st));
    updateConfiguration(device_info);
    system_reload();
  }
}

void connected_ap_info(apinfo_adv_t *ap_info, char *key, int key_len)  //callback, return connected AP info
{
  /*Update fastlink record*/
  int result, result1;
  result = memcmp(&(device_info->conf.fastLinkConf.ap_info), ap_info, sizeof(ApList_adv_t));
  result1 = memcmp(&(device_info->conf.fastLinkConf.key), key, key_len);
  if(device_info->conf.fastLinkConf.availableRecord == 0||result||result1){
    device_info->conf.fastLinkConf.availableRecord = 1;
    memcpy(&(device_info->conf.fastLinkConf.ap_info), ap_info, sizeof(ApList_adv_t));
    memcpy(device_info->conf.fastLinkConf.key, key, key_len);
    device_info->conf.fastLinkConf.key_len = key_len;
    updateConfiguration(device_info);
  }
}

void userWatchDog(void)
{
}

void WifiStatusHandler(int event)
{
  switch (event) {
    case MXCHIP_WIFI_UP:
			set_network_state(STA_CONNECT,1);
      break;
    case MXCHIP_WIFI_DOWN:
			set_network_state(STA_CONNECT,0);
      break;
    case MXCHIP_UAP_UP:
      set_network_state(UAP_START,1);
      break;
    case MXCHIP_UAP_DOWN:
      set_network_state(UAP_START,0);
      break;  
    default:
      break;
  }
  return;
}

void ApListCallback(ScanResult *pApList)
{
  ap_num = pApList->ApNum;
  memcpy(&hugebuf[sizeof(mxchip_cmd_head_t)-1], pApList->ApList, ap_num*sizeof(ApList_str));
  startScan = 0;
}

void NetCallback(net_para_st *pnet)
{
  strcpy((char *)device_info->status.ip, pnet->ip);
  strcpy((char *)device_info->status.mask, pnet->mask);
  strcpy((char *)device_info->status.gw, pnet->gate);
  strcpy((char *)device_info->status.dns, pnet->dns);
}

///////////////////////////////////////////////////////////////////

/* Sysytem initilize */
void mxchipWNet_HA_init(void)
{
  network_InitTypeDef_st wNetConfig;
  network_InitTypeDef_adv_st wNetConfigAdv;
  int err = MXCHIP_FAILED;

  net_para_st para;
	device_info = (mxchipWNet_HA_st *)malloc(sizeof(mxchipWNet_HA_st));
  memset(device_info, 0, sizeof(mxchipWNet_HA_st)); 

  SystemCoreClockUpdate();
	mxchipInit();
  hal_uart_init();
  getNetPara(&para, Station);
  formatMACAddr((void *)device_info->status.mac, &para.mac);
  strcpy((char *)device_info->status.ip, (char *)&para.ip);
  strcpy((char *)device_info->status.mask, (char *)&para.mask);
  strcpy((char *)device_info->status.gw, (char *)&para.gate);
  strcpy((char *)device_info->status.dns, (char *)&para.dns);

  readConfiguration(device_info);

  device_info->tcpServer_fd = -1;
  device_info->tcpClient_fd = (int *)malloc(4 * MAX_CLIENT);
  device_info->cloud_fd = -1;
  device_info->udpSearch_fd = -1;
  memset(device_info->tcpClient_fd, -1, sizeof(4 * MAX_CLIENT));

  if(device_info->conf.fastLinkConf.availableRecord){ //Try fast link
    memcpy(&wNetConfigAdv.ap_info, &device_info->conf.fastLinkConf.ap_info, sizeof(ApList_adv_t));
    memcpy(&wNetConfigAdv.key, &device_info->conf.fastLinkConf.key, device_info->conf.fastLinkConf.key_len);
    wNetConfigAdv.key_len = device_info->conf.fastLinkConf.key_len;
    wNetConfigAdv.dhcpMode = DHCP_Client;
    strcpy(wNetConfigAdv.local_ip_addr, (char*)device_info->conf.ip);
    strcpy(wNetConfigAdv.net_mask, (char*)device_info->conf.mask);
    strcpy(wNetConfigAdv.gateway_ip_addr, (char*)device_info->conf.gw);
    strcpy(wNetConfigAdv.dnsServer_ip_addr, (char*)device_info->conf.dns);
    wNetConfigAdv.wifi_retry_interval = 100;
    err = StartAdvNetwork(&wNetConfigAdv);
  }

  if(err == MXCHIP_FAILED){
    wNetConfig.wifi_mode = Station;
    strcpy(wNetConfig.wifi_ssid, device_info->conf.sta_ssid);
    strcpy(wNetConfig.wifi_key, device_info->conf.sta_key);
    wNetConfig.dhcpMode = DHCP_Client;
    strcpy(wNetConfig.local_ip_addr, (char*)device_info->conf.ip);
    strcpy(wNetConfig.net_mask, (char*)device_info->conf.mask);
    strcpy(wNetConfig.gateway_ip_addr, (char*)device_info->conf.gw);
    strcpy(wNetConfig.dnsServer_ip_addr, (char*)device_info->conf.dns);
	  wNetConfig.wifi_retry_interval = 500;
    StartNetwork(&wNetConfig);
  }
  ps_enable();
}

static void uap_start(void)
{
  network_InitTypeDef_st wNetConfig;

  if(device_info->cloud_fd > 0)   //Donot retry to cloud, as the station is disconnected, reboot if station is recovered
  {
    close(device_info->cloud_fd);
    cloud_enable = 0;
  }

  wNetConfig.wifi_mode = Soft_AP;
  sprintf(wNetConfig.wifi_ssid, "%s_%c%c%c%c", device_info->conf.uap_ssid,\
                  device_info->status.mac[12], device_info->status.mac[13],
                  device_info->status.mac[15], device_info->status.mac[16]);
  strcpy(wNetConfig.wifi_key, device_info->conf.uap_key);
  wNetConfig.dhcpMode = DHCP_Server;
  strcpy(wNetConfig.local_ip_addr, UAP_IP_ADDRESS);
  strcpy(wNetConfig.net_mask, UAP_MASK_ADDRESS);
  strcpy(wNetConfig.gateway_ip_addr, UAP_GW_ADDRESS);
  strcpy(wNetConfig.dnsServer_ip_addr, UAP_DNS_ADDRESS);
  strcpy(wNetConfig.address_pool_start, UAP_DHCP_SERVER_START);
  strcpy(wNetConfig.address_pool_end, UAP_DHCP_SERVER_END);
  StartNetwork(&wNetConfig);
	uap_start_time = MS_TIMER + UAP_START_TIME_SHORT; 
}


/* main tick */
void mxchipWNet_HA_tick(void)
{
  mxchipTick();
  if (!is_wifi_disalbed()) {
    if (is_network_state(STA_CONNECT) == 0 && is_network_state(UAP_START) == 0) {
      if (MS_TIMER > uap_start_time) {
        uap_start();
      }
    }

    if(is_network_state(STA_CONNECT) && cloud_enable && MS_TIMER > cloud_retry)
      connect_to_cloud();

    tcp_recv_tick();
  }
  
  if(need_report){
    report_state();
    need_report = 0;
  }

  if (need_reload == 1) {
    msleep(500);
    NVIC_SystemReset();
  }
}

void dns_ip_set(u8 *hostname, u32 ip)
{
	if((int)ip == -1){
		cloud_ip_addr = 0;
		cloud_retry = MS_TIMER + CLOUD_RETRY;
	}
	else
		cloud_ip_addr = ip;
	
  dns_pending = 0;
}

static void connect_to_cloud(void)
{
  int opt = 0;
  struct sockaddr_t addr;

  if(cloud_ip_addr == 0 && dns_pending == 0){  //DNS function
    cloud_ip_addr = dns_request((char *)device_info->conf.server_domain);
    if(cloud_ip_addr == -1){
      cloud_ip_addr = 0; //dns failed, retry again
      goto cloud_error;  
    }
    else if (cloud_ip_addr == 0){  //DNS pending, waiting for callback
      dns_pending = 1;
      return;
    }
	}

  if( device_info->cloud_fd == -1 && (u32)cloud_ip_addr>0){
    device_info->cloud_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(device_info->cloud_fd,0,SO_BLOCKMODE,&opt,4);
    addr.s_ip = cloud_ip_addr; 
    addr.s_port = device_info->conf.server_port;
    if (connect(device_info->cloud_fd, &addr, sizeof(addr))!=0) 
      goto cloud_error; 
  }
	return;
cloud_error:
  close_cloud(CLOUD_RETRY);
  return;
}

static void close_cloud(int retry_interval)
{
  close(device_info->cloud_fd);
  device_info->cloud_fd = -1;
  cloud_ip_addr = 0;
  if(is_network_state(REMOTE_CONNECT))
    set_network_state(REMOTE_CONNECT, 0);
  cloud_retry = MS_TIMER + retry_interval;
}


static void tcp_recv_tick(void)
{
  int i, j, len;
  fd_set readfds;
  struct timeval_t t, sendTimeout;
  struct sockaddr_t addr;
  socklen_t addrLen;
  
  FD_ZERO(&readfds);
  t.tv_sec = 0;
  t.tv_usec = 1000;
  
  if (device_info->tcpServer_fd==-1) {
    device_info->tcpServer_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    addr.s_port = TCP_SERVER_PORT;
    bind(device_info->tcpServer_fd, &addr, sizeof(addr));
    listen(device_info->tcpServer_fd, 0);
    
    memset(device_info->tcpClient_fd, -1, 4 * MAX_CLIENT);
  } else {
    FD_SET(device_info->tcpServer_fd, &readfds);
    for(i=0;i<MAX_CLIENT;i++) {
      if (device_info->tcpClient_fd[i] != -1)
        FD_SET(device_info->tcpClient_fd[i], &readfds);
    }
  }

  if(device_info->udpSearch_fd==-1){
    device_info->udpSearch_fd = socket(AF_INET, SOCK_DGRM, IPPROTO_UDP);
    addr.s_port = UDP_BROADCAST_PORT;
    bind(device_info->udpSearch_fd, &addr, sizeof(addr));
  }
  else{
    FD_SET(device_info->udpSearch_fd, &readfds);
  }

  if(device_info->cloud_fd != -1)
    FD_SET(device_info->cloud_fd, &readfds);

  select(1, &readfds, NULL, NULL, &t);

  for(i=0;i<MAX_CLIENT;i++) {
    if (device_info->tcpClient_fd[i] != -1) {
      if (FD_ISSET(device_info->tcpClient_fd[i], &readfds)) {
        len = recv(device_info->tcpClient_fd[i], hugebuf, IP_CMD_MAX_LEN, 0);
        if(len==-1){
          close(device_info->tcpClient_fd[i]);
          device_info->tcpClient_fd[i] = -1;
        }
        else
          ip_cmd_process(hugebuf, len, device_info->tcpClient_fd[i]);
      }
    }
  }
  
  if (FD_ISSET(device_info->tcpServer_fd, &readfds)) {
    j = accept(device_info->tcpServer_fd, &addr, &len);
    if (j > 0) {
      for(i=0;i<MAX_CLIENT;i++) {
        if ( device_info->tcpClient_fd[i] == -1) {
           device_info->tcpClient_fd[i] = j;
           sendTimeout.tv_sec = 2;
           sendTimeout.tv_usec = 0;
          setsockopt(j, 0, SO_SNDTIMEO, &sendTimeout, sizeof(struct timeval_t));
          break;
        }
      }
    }
  }

  if(device_info->cloud_fd != -1){
    if (FD_ISSET(device_info->cloud_fd, &readfds)) {
      len = recv(device_info->cloud_fd, hugebuf, IP_CMD_MAX_LEN, 0);
      if(len==-1)
        close_cloud(0);
      else
        ip_cmd_process(hugebuf, len, device_info->cloud_fd);
    }
  }

   if (FD_ISSET(device_info->udpSearch_fd, &readfds)) {
     len = recvfrom(device_info->udpSearch_fd, hugebuf, IP_CMD_MAX_LEN, 0, &addr, &addrLen);
     search_cmd_process(hugebuf, len, &addr, sizeof(struct sockaddr_t));
  } 
}

static int search_cmd_process(unsigned char *buf, int cmd_len, const struct sockaddr_t *dest_addr, socklen_t addrlen)
{
  u32 len=0;
  mxchip_cmd_head_t *p_reply;
  udp_search_st *data;
  u16 cksum, cmd;
    
  if(device_info->udpSearch_fd == 0)
    return 0;

  if ((buf[0] != CONTROL_FLAG) || (buf[1] != 0))
    return 0;

  if (check_sum(hugebuf, cmd_len) != 1)
    return 0;
    
  p_reply = (mxchip_cmd_head_t *)buf; //Use the same data block to save the RAM
  data = (udp_search_st *)(p_reply->data);
  p_reply->cmd_status = CMD_OK;
  cmd = p_reply->cmd;
  p_reply->cmd |= 0x8000;
  if(cmd == CMD_SEARCH){
    p_reply->datalen = sizeof(udp_search_st);
    memcpy(data->ip, device_info->status.ip, 16);
    memcpy(data->mac, device_info->status.mac, 18);
    sprintf((char *)data->mac, "%08x", FW_VERSION);
    len = sizeof(mxchip_cmd_head_t) + 1 + p_reply->datalen;
    cksum = calc_sum(hugebuf, len-2);
    hugebuf[len-2] = cksum & 0x0ff;
    hugebuf[len-1] = (cksum & 0xff00) >> 8;
    sendto(device_info->udpSearch_fd, (u8*)p_reply, sizeof(mxchip_cmd_head_t) + 1 + p_reply->datalen, \
        0, dest_addr, addrlen);
  }
  return 0;
}

static int ip_cmd_process(unsigned char *buf, int cmd_len, int socket_fd)
{
  u32 len=0;
  mxchip_cmd_head_t *p_reply;
  u16 cksum, cmd;
    
  if ((buf[0] != CONTROL_FLAG) || (buf[1] != 0))
    return 0;

  if (check_sum(hugebuf, cmd_len) != 1)
    return 0;
    
  p_reply = (mxchip_cmd_head_t *)buf;
  p_reply->cmd_status = CMD_OK;
  cmd = p_reply->cmd;
  p_reply->cmd |= 0x8000;
  switch (cmd) {
    case CMD_READ_VERSION:
      p_reply->datalen = 8;
      sprintf((char *)p_reply->data, "%08x", FW_VERSION);
      break;
      
    case CMD_READ_CONFIG:
      p_reply->datalen = CONFIG_DATA_SIZE + 18;// 18 is mac address
      memcpy(p_reply->data, device_info->conf.uap_ssid, CONFIG_DATA_SIZE);
      memcpy((char *)&p_reply->data[CONFIG_DATA_SIZE], device_info->status.mac,18);
      break;

    case CMD_WRITE_CONFIG:
      if (p_reply->datalen < CONFIG_DATA_SIZE)
        p_reply->cmd_status = CMD_FAIL;
      else {
        memcpy(device_info->conf.uap_ssid, p_reply->data, CONFIG_DATA_SIZE);
        /*Clear fastlink record*/
        memset(&(device_info->conf.fastLinkConf), 0x0, sizeof(fast_link_st));
        updateConfiguration(device_info);
        delay_reload();
      }
      p_reply->datalen = 0;
      break;

    case CMD_SCAN:
      mxchipStartScan();
      startScan = 1;

      while(startScan == 1){
        mxchipTick();
      }
      p_reply->datalen = ap_num*sizeof(ApList_str);
      break;

    case CMD_OTA:
      ota_process(buf, cmd_len, &socket_fd);
      p_reply->datalen = 0;
      return 0; //send ack in ota_process

    case CMD_NET2COM:
      wifi_input(buf, cmd_len);
      return 0; 

    default:
      return 0;

  }
  
  len = sizeof(mxchip_cmd_head_t) + 1 + p_reply->datalen;
  cksum = calc_sum(hugebuf, len-2);
  hugebuf[len-2] = cksum & 0x0ff;
  hugebuf[len-1] = (cksum & 0xff00) >> 8;
  fast_send(socket_fd, (u8*)p_reply, sizeof(mxchip_cmd_head_t) + 1 + p_reply->datalen);

  return 0;
}

static int ota_process(unsigned char *buf, int cmd_len, int *socket_fd)
{
  mxchip_cmd_head_t *p_control_cmd;
  ota_upgrate_t *p_upgrade;
  u8 * p_bin;
  int bin_len, total_len, head_len;
  u32 end_time = MS_TIMER + MAX_UPGRADE_TIME;
  u32 flash_addr = UPDATE_START_ADDRESS;
  u8 md5_ret[16];
  mxchip_cmd_head_t cmd_ack;
  fd_set readfds;
  struct timeval_t t;

  memset(&cmd_ack, 0, sizeof(cmd_ack));
  cmd_ack.cmd_status = CMD_FAIL;
  p_control_cmd = (mxchip_cmd_head_t *)buf;
  cmd_ack.flag = p_control_cmd->flag;
  cmd_ack.cmd = p_control_cmd->cmd | 0x8000;
  head_len = sizeof(mxchip_cmd_head_t) + sizeof(ota_upgrate_t) - 2;
  if (cmd_len < head_len){
    goto CMD_REPLY;
  }
  FLASH_If_Init();
  p_upgrade = (ota_upgrate_t*)(p_control_cmd->data);

  p_bin = p_upgrade->data;
  total_len = p_upgrade->len;
  bin_len = cmd_len - head_len;
  total_len -= bin_len;

  if (bin_len>0)
    FLASH_If_Write(&flash_addr, (uint32_t *)p_bin, bin_len);
  delay_reload(); //Reset module if OTA is finished

  while (total_len>0) {
    mxchipTick();
  
    FD_ZERO(&readfds);
    t.tv_sec = 0;
    t.tv_usec = 1000;
    FD_SET(*socket_fd, &readfds);
    select(1, &readfds, NULL, NULL, &t);

    if (FD_ISSET(*socket_fd, &readfds)) {
      bin_len = recv(*socket_fd, (char*)p_bin, 1024, 0);
      if(bin_len == -1){       
        if(*socket_fd == device_info->cloud_fd)
          close_cloud(0);
        else{
          close(*socket_fd);
          *socket_fd = -1;
        }
        
        return 0; //Net error, donot need to send ack
      }
      else{
        FLASH_If_Write(&flash_addr, (uint32_t *)p_bin, bin_len);
				total_len-=bin_len;
			}
    }
  }

  md5_hex((u8 *)UPDATE_START_ADDRESS, flash_addr - UPDATE_START_ADDRESS, md5_ret);
  if(memcmp(md5_ret, p_upgrade->md5, 16) != 0) {
    FLASH_Lock();
    goto CMD_REPLY;
  }

  memset(&device_info->conf.bootTable, 0, sizeof(boot_table_t));
  device_info->conf.bootTable.length = flash_addr - UPDATE_START_ADDRESS;
  device_info->conf.bootTable.start_address = UPDATE_START_ADDRESS;
  device_info->conf.bootTable.type = 'A';
  device_info->conf.bootTable.upgrade_type = 'U';
  updateConfiguration(device_info);
  cmd_ack.cmd_status = CMD_OK;
  
CMD_REPLY:
  fast_send(*socket_fd, (u8 *)&cmd_ack, sizeof(cmd_ack) + 1 + cmd_ack.datalen);
  return 0;
}

/* Get system status */
void get_status(mxchip_state_t *cmd)
{
  u16 cksum;
  sta_ap_state_t ap_state;

  cmd->flag = 0x00BB;
  cmd->cmd = 0x8008;
  cmd->cmd_status = 0;
  cmd->datalen = sizeof(mxchip_state_t) - 10;

  device_info->status.uap_state = is_network_state(UAP_START);
  device_info->status.sta_state = is_network_state(STA_CONNECT);
  if (is_network_state(STA_CONNECT) == 1)
    device_info->status.tcp_client = is_network_state(REMOTE_CONNECT) + 1;
  else
    device_info->status.tcp_client = 0;

  CheckNetLink(&ap_state);
  device_info->status.signal = ap_state.wifi_strength;
  memcpy((void *)&cmd->status, &device_info->status, sizeof(current_state_t));

  cksum = calc_sum(cmd, sizeof(mxchip_state_t) - 2);
  cmd->cksum = cksum;
}

/* Back to default configuration and reload */
void system_default(void)
{
}

/* Wifi Power down*/
void wifi_disable(void)
{
	int i;
  for(i=0;i<MAX_CLIENT;i++) {
    if (device_info->tcpClient_fd[i] != -1)
			close(device_info->tcpClient_fd[i]);
      device_info->tcpClient_fd[i] = -1;
  }

  if(device_info->cloud_fd != -1){
		close(device_info->cloud_fd);
		cloud_enable = 0;
    device_info->cloud_fd = -1;
	}
	wifi_power_down();
}
/* Output data by wifi */
void wifi_output(u8 *buf, int len)
{
  int i;
  for(i=0;i<MAX_CLIENT;i++) {
    if (device_info->tcpClient_fd[i] != -1)
      fast_send(device_info->tcpClient_fd[i], buf, len);
  }

  if(device_info->cloud_fd != -1)
    fast_send(device_info->cloud_fd, buf, len);
}

void wifi_input(u8 *buf, int len)
{
    hal_uart_send_data(buf, len);
}

void fast_send(int sockfd, const void *buf, size_t len)
{
  int start_tick = MS_TIMER;
  if(sockfd == device_info->cloud_fd && !is_network_state(REMOTE_CONNECT))
    return;
  while(tx_buf_size(sockfd)<(int)len){
    if(start_tick + SEND_TIMEOUT < (int)MS_TIMER)
      return;
    msleep(20);
  }
  send(sockfd, buf, len, 0);
}












