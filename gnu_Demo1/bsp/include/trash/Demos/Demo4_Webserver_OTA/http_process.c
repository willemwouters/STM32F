#include <stdio.h>
#include <string.h>
#include "stm32f2xx.h"
#include "flash_if.h"
#include "http_process.h"
#include "mxchipWNet.h"
#include "main.h"
#include "base64.h"
#include "flash_configurations.h"
#include "web_data.c"

#define HTTP_DATA_MAX_LEN 2048
#define UDP_SEARCH_MAX_LEN 100

#define BOOT_TABLE_ADDR 0x08004000

#define MAX_CLIENT 8
int listen_fd = -1;
int clientfd[MAX_CLIENT];

typedef struct
{
	char *pToken1;	//HTTP request 
	char *pToken2;	//URL
	char *pToken3;  //URL function
//	char *pToken4;	//Next
} httpToken_struct;

typedef enum{
	HTTP_DISPLAY_STANDARD,
	HTTP_DISPLAY_IOS,
	HTTP_DISPLAY_ANDROID
} httpDisplayType;

static char userName[]="admin";
static char usrPassword[]="admin";
static char firmwareVersion[]="2.0";
static char *auth_str = NULL;

char *httpRequest;
static void HandleHttpClient(int index);
static u8 HTTPParse(char* pStr, httpToken_struct *httpToken);
static void send_http_data(int index, char *data, int len);
int auth_init(char *name, char *passwd);
static void send_system_page(int index);
static void get_update_post(int index, char *postdata, int len);
static void get_settings_post(int index, char *postdata, int len);
char *memmem(char *src, int src_len, const char *dst, int dst_len);
void save_reset_Response(u8 result, int index);

extern int MS_TIMER;
extern config_t configParas;


void delay_reload(void)
{
	NVIC_SystemReset();
}

void http_init(void)
{
	int i;
  httpRequest = malloc(HTTP_DATA_MAX_LEN);
	listen_fd = -1;
  for(i=0;i<MAX_CLIENT;i++) {
	  clientfd[i] = -1;
	}
	auth_init(userName, usrPassword);
}

void stop_http_server(void)
{
	int i;
  for(i=0;i<MAX_CLIENT;i++) {
    if (clientfd[i] != -1){
      close(clientfd[i]);
			clientfd[i] = -1;
		}
	}
	close(listen_fd);
	listen_fd = -1;
}

void http_tick(void)
{
	int i, j, len;
	fd_set readfds;
	struct timeval_t t, timeout;
	struct sockaddr_t addr;
  
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;
	
	FD_ZERO(&readfds);
	t.tv_sec = 0;
	t.tv_usec = 1000;
	
	if (listen_fd==-1) {
		listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		addr.s_port = 80;
		bind(listen_fd, &addr, sizeof(addr));
		listen(listen_fd, 0);
	} else {
		FD_SET(listen_fd, &readfds);
		for(i=0;i<MAX_CLIENT;i++) {
			if (clientfd[i] != -1)
				FD_SET(clientfd[i], &readfds);
		}
	}
	
	select(1, &readfds, NULL, NULL, &t);

	for(i=0;i<MAX_CLIENT;i++) {
		if (clientfd[i] != -1) {
			if (FD_ISSET(clientfd[i], &readfds)) {
				HandleHttpClient(clientfd[i]);
				msleep(500);
        close(clientfd[i]);
	      clientfd[i] = -1;
			}
		}
	}
	
	if (FD_ISSET(listen_fd, &readfds)) {
		j = accept(listen_fd, &addr, &len);
		if (j >= 0) {
			for(i=0;i<MAX_CLIENT;i++) {
				if ( clientfd[i] == -1) {
					 clientfd[i] = j;
					setsockopt(j, 0, SO_SNDTIMEO, &timeout, sizeof(struct timeval_t));
					break;
				}
			}
		}
	}
}


/*******************************************************************************
* Function Name  	:  HandleHttpClient.  
* Description    	:  handle http request.
* Input          		:  ClientSocket:Http Client socket.
* Return         	:  None.
*******************************************************************************/
static void HandleHttpClient(int index)
{
	int NumOfBytes;
	httpToken_struct httpToken ={0,0,0};
	char *p_auth;

    msleep(200); // sleep 200 ms, just incase http request fregment.
	NumOfBytes = recv(index, httpRequest, HTTP_DATA_MAX_LEN, 0);
	if (NumOfBytes < 0) {
		return;
	} else if (NumOfBytes == 0) {
		return;
	}

	httpRequest[NumOfBytes] = '\0';
	if(!HTTPParse(httpRequest,&httpToken)) 
		goto EXIT;;//http request header error

    p_auth = strstr(httpToken.pToken2, auth_str);
	if (p_auth == NULL) { // un-authrized
        send_http_data(index, (char*)authrized, strlen(authrized));
        goto EXIT;
	} else {
        p_auth += strlen(auth_str);
        if (*p_auth != 0x0d) {
            send_http_data(index, (char*)authrized, strlen(authrized));
            goto EXIT;
        }
    }
		
	if(!strcmp(httpToken.pToken1, "GET"))  
	{
    if(!strncmp(httpToken.pToken2, "/system.htm", strlen("/system.htm")))  {
			send_system_page(index);
		} 
	  else if(!strncmp(httpToken.pToken2, "/ ", 2))  {
			send_system_page(index);
		} else {
			send_http_data(index, (char *)not_found, strlen(not_found));
		}
	}
	else if(!strcmp(httpToken.pToken1, "POST"))  
	{
		if(!strncmp(httpToken.pToken2, "/update.htm", strlen("/update.htm")))  {
			get_update_post(index, httpToken.pToken2, NumOfBytes - (httpToken.pToken2-httpRequest));
		} 
		if(!strncmp(httpToken.pToken2, "/settings.htm", strlen("/settings.htm")))  {
			get_settings_post(index, httpToken.pToken2, NumOfBytes - (httpToken.pToken2-httpRequest));
		} 

	}

EXIT:
    
	return;
}

static u8 HTTPParse(char* pStr, httpToken_struct *httpToken)
{
	char* pch = strchr(pStr, ' ');
	char* pch2 = strchr(pStr, '/');
	char* pch3;
	httpToken->pToken1 = pStr;
	
	if(pch) 
	{
		*pch='\0';		// 到第一个" " 为pToken1
		
		pch++;
		pch3 = strchr(pch, ' ');				////////////////////////////
		if(pch2&&pch2<pch3)
			{//从第一个'/' 到其后面的第一个' ' 赋给 pToken2 ('/' 不在赋值字符串中)
				httpToken->pToken2=pch2;
				pch2++;			
				pch2 = strchr(pch2, '/');
				if(pch2&&pch2<pch3)
					{// pToken2 中第一个'/' 到结尾赋给pToken3
						 pch2++;
						httpToken->pToken3=pch2;
					}
				else
					httpToken->pToken3=NULL;
			}
		else
			httpToken->pToken2=NULL;
		return TRUE;
	}
	return FALSE;
}

static void send_http_data(int index, char *data, int len)
{
	int SendCount = 0, NumAtOnce;
	u32 end_time = MS_TIMER+5000; // max wait for 5 seconds.
	
	while(SendCount < len)
	{
		NumAtOnce = send(index, data+SendCount, len-SendCount, 0); 
		if(0 >= NumAtOnce)
			{
				return;
			}
		if (end_time<MS_TIMER) // timeout
			return;
		SendCount+=NumAtOnce;
	}	  
}


int auth_init(char *name, char *passwd)
{
	int len, outlen;
	char *src_str;
	len = strlen(name) + strlen(passwd) + 2;

	if (auth_str)
		free(auth_str);
	
	src_str = malloc(len);
	if (src_str == 0)
		return -1;

  sprintf(src_str, "%s:%s", name, passwd);
	auth_str = (char *)base64_encode((const unsigned char *)src_str, strlen(src_str), &outlen);
	len = strlen(auth_str);
	auth_str[len-1] = 0;
	free(src_str);
	return 0;
}

void html_decode(char *p, int len)
{
	int i, j, val;
	char assic[4];

	for (i=0; i<len; i++) {
		if (p[i] == '+')
			p[i] = ' ';
		if (p[i] == '%') {
			if ((i+2) >= len)
				return;
			assic[2] = 0;
			assic[0] = p[i+1];
			assic[1] = p[i+2];
			val = strtol(assic,NULL,16);
			p[i] = val;
			for (j = i+1; j< len;j++) 
				p[j] = p[j+2];
		}
	}
}

/*******************************************************************************
* Function Name  :  PostParse.  
* Description    :  Post context Paser.
* Input          :  
* Return         :  None.
*******************************************************************************/
static u8 PostParse(char** ppStr, const char* pFlag, char** ppValue)
{
	char* pch=strstr(*ppStr, pFlag);
	char* pch2=NULL;
	if(pch) 
	{
		pch2=strchr(pch, '=');
		if(!pch2) return FALSE;
		pch2++;
		*ppValue=pch2;
		if(!*ppValue) return FALSE;
		pch=strchr(pch2, '&');
		if(pch) 
		{
			*pch='\0';
			html_decode(pch2, strlen(pch2));
			*ppStr=pch+1;
			return TRUE;
		}
	}
	return FALSE;
}


static void send_system_page(int index)
{
	char *body;
	u32 NumOfBytes;	
	
#define FORMAT_POST_STR sprintf(body, systemPage, APP_INFO,\
						configParas.wifi_ssid, configParas.wifi_key);

	memset(httpRequest,0,HTTP_DATA_MAX_LEN);
	body = httpRequest;
	FORMAT_POST_STR;
	sprintf(httpRequest, headerPage, strlen(body)); 	// recalute the body length.
	body = httpRequest+strlen(httpRequest);
	FORMAT_POST_STR;
	NumOfBytes = strlen(httpRequest);

	send_http_data(index, httpRequest, NumOfBytes);
}

static void get_settings_post(int index, char *postdata, int len)
{
	char* pToken1, *pValue;
  pToken1  = postdata;
		
	if(!(PostParse(&pToken1,"SSID",&pValue))) {
		save_reset_Response(0, index);
	  return;
	}
	
	strcpy(configParas.wifi_ssid, pValue);

	if(!PostParse(&pToken1,"pass",&pValue)) {
		save_reset_Response(0, index);
	  return;
	}
	
	strcpy(configParas.wifi_key, pValue);
	
	if(strstr(pToken1, "save")) {
		save_reset_Response(1, index);
		return;
	}
	
	if(strstr(pToken1, "reset")) {
		save_reset_Response(2, index);
		return;
	}
	
	if(strstr(pToken1, "upload")==0) {
		return;
	}
}
static void get_update_post(int index, char *postdata, int len)
{
	static const char endline[] = {'\r', '\n', '\r', '\n'};
	static const char lengthstr[] = "Content-Length: ";
	static const char boundarystr[] = "boundary=";
	char *boundcp, *boundary, *p;
	char *read_buffer = postdata, *end_pos = NULL, *lengthpos;
	int read_buffer_size = len;
  struct timeval_t time;
	int bytes_received, read_len, content_len = 0, total_read;
	const char *resp;
	u32 addr = UPDATE_START_ADDRESS;
  time.tv_sec = 5;
  time.tv_usec = 0;
	
	/* Firmware update: OTA from webserver*/
    setsockopt(index,0, SO_RCVTIMEO, &time, sizeof(struct timeval_t));

    /* Get the content length & boundary & begin of content data */
    do {
        end_pos = (char*) memmem(read_buffer, read_buffer_size, endline, sizeof(endline));

        if ( ( lengthpos = (char*) memmem( read_buffer,  read_buffer_size, lengthstr, strlen( lengthstr )) ) != NULL )
        {
            content_len = atoi(lengthpos + sizeof( lengthstr)-1);
        }
        if (( boundary = (char*) memmem( read_buffer,  read_buffer_size, boundarystr, strlen(boundarystr) )) != NULL )
        {
            boundary += strlen(boundarystr);
            p = boundary;
            while(*p != 0x0d)
                p++;
            *p++ = 0; 
            // now, we have found out the boundary, copy out.
            boundcp = (char*)malloc(strlen(boundary)+1);
            if (boundcp != NULL) {
                strcpy(boundcp, boundary);
            }
        }
        
        if (end_pos == NULL)
        {
            read_buffer = httpRequest;
            bytes_received = recv(index, httpRequest, HTTP_DATA_MAX_LEN, 0 );
            if ( bytes_received <= 0 )
            {
                break;
            }
            else
            {
                total_read += bytes_received;
                read_buffer_size = bytes_received;
            }
        }

    } while ( end_pos == NULL );
    if (boundcp == NULL || content_len == 0) {
        resp = systemResponseError;
        goto EXIT;
    }

    end_pos += sizeof(endline);
    read_buffer_size = read_buffer_size - (end_pos-read_buffer);
    content_len -= read_buffer_size;
    read_buffer = end_pos;
    /* Get the begin of file data and write to flash */
    do {
        end_pos = (char*)memmem(read_buffer, read_buffer_size, endline, sizeof(endline));
        
        if (end_pos == NULL)
        {
            read_buffer = httpRequest;
            bytes_received = recv(index, httpRequest, HTTP_DATA_MAX_LEN, 0 );
            if ( bytes_received <= 0 )
            {
                break;
            }
            else
            {
                content_len -= bytes_received;
                read_buffer_size = bytes_received;
                if (content_len <= 0)
                    break;
            }
        }

    } while ( end_pos == NULL );
    if (end_pos == NULL) {
        resp = systemResponseError;
        goto EXIT;
    }
    
    SetTimer(6000, delay_reload); // whether success or not, need reload system to use bootload erase NEW Image flash.
    FLASH_If_Init();
    end_pos += sizeof(endline);
    read_buffer_size = read_buffer_size - (end_pos-read_buffer);
    if (read_buffer_size > 0) {
			  FLASH_If_Write(&addr, (void *)end_pos, read_buffer_size);
    }

    content_len -= strlen(boundcp) - 4; // last string is '--'+boudnary+'--'
    /* Recv file and write to flash, if it's last package, find the end of file to write */
    while(content_len > 0) {
        if (content_len > HTTP_DATA_MAX_LEN)
            read_len = HTTP_DATA_MAX_LEN;
        else
            read_len = content_len;
        
        bytes_received = recv(index, httpRequest, read_len, 0 );
        if ( bytes_received <= 0 )
        {
            break;
        }
        
				FLASH_If_Write(&addr, (void *)httpRequest, bytes_received);    
        content_len -= bytes_received;
    }
    
    if (content_len == 0) {        
      memset(&configParas, 0, sizeof(boot_table_t));
    	configParas.bootTable.length = addr - UPDATE_START_ADDRESS;
    	configParas.bootTable.start_address = UPDATE_START_ADDRESS;
    	configParas.bootTable.type = 'A';
    	configParas.bootTable.upgrade_type = 'U';
			updateConfig(&configParas);
      resp = systemResponseSucc;
    } else
      resp = systemResponseError;
EXIT:
    FLASH_Lock();
    sprintf(httpRequest, HTTPSaveResponse, 
			strlen(resp), resp);
    send_http_data(index, httpRequest, strlen(httpRequest));
}


void save_reset_Response(u8 result, int index)
{
	if(result == TRUE)
	{
		sprintf(httpRequest, HTTPSaveResponse, 
		strlen(SaveResponseSucc), SaveResponseSucc);
		updateConfig(&configParas);
	}
	else if(result == FALSE)
	{
		sprintf(httpRequest, HTTPSaveResponse, 
		strlen(SaveResponseError), SaveResponseError);
		
	} else if(result == 2)
	{
		sprintf(httpRequest, HTTPSaveResponse, 
		strlen(ResponseReset), ResponseReset);
		SetTimer(3000, delay_reload);
	}

	send_http_data(index, httpRequest, strlen(httpRequest));
}

/* Find dst string from src string. return the first place */
char *memmem(char *src, int src_len, const char *dst, int dst_len)
{
    int i, j;

    for (i=0; i<=src_len-dst_len; i++) {
        if (src[i] == dst[0]) {
            for (j=1; j<dst_len; j++) {
                if (src[i+j] != dst[j])
                    break;
            }
            if (j == dst_len)
                return &src[i];
        }
    }

    return NULL;
}


