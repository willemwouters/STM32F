/* Includes ------------------------------------------------------------------*/

#include "stdio.h"
#include "string.h"
#include <ctype.h>                    /* character functions                 */

#include "platform.h"
#include "mxchipWNet.h"
#include "menu.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_CMD_LEN  200
#define CNTLQ      0x11
#define CNTLS      0x13
#define DEL        0x7F
#define BACKSPACE  0x08
#define CR         0x0D
#define LF         0x0A
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern const char menu[];
extern int menu_enable;
char ERROR_STR [] = "\n*** ERROR: %s\n";    /* ERROR message string in code   */
int  inputCount = 0;
char cmdbuf [MAX_CMD_LEN] = {0};

//extern const char menu[];

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Analyse a command parameter
  * @param  commandBody: command string address
  * @param  para: The para we are looking for
  * @param  paraBody: A pointer to the buffer to receive the para body.
  * @param  paraBodyLength: The length, in bytes, of the buffer pointed to by the paraBody parameter.
  * @retval the actual length of the paraBody received, -1 means failed to find this paras 
  */
int findCommandPara(char *commandBody, char para, char *paraBody, int paraBodyLength)
{
  int i = 0;
  int k, j;
  int retval = -1;
  para = toupper(para);
  while(commandBody[i] != 0) {
    if(commandBody[i] == '-' && commandBody[i+1] == para){   /* para found!             */
      retval = 0;
      for (k = i+2; commandBody[k] == ' '; k++);      /* skip blanks                 */
      for(j = 0; commandBody[k] != ' ' && commandBody[k] != 0 && commandBody[k] != '-'; j++, k++){   /* para body found!             */
          paraBody[j] = commandBody[k];
          retval ++;
          if( retval == paraBodyLength)
            return retval;
        }
    }
    i++;
  }
  return retval;
}


static int getline (void)  {
  char c;
	
	if(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET)
		return 0;
	else{ 
		c = USART_ReceiveData(USARTx);
		if (c  == CR)  c = LF;     /* read character                 */
		if (c == BACKSPACE  ||  c == DEL)  {    /* process backspace              */
			if (inputCount != 0)  {
				inputCount--;                              /* decrement count                */
				putchar (BACKSPACE);                /* echo backspace                 */
				putchar (' ');
				putchar (BACKSPACE);
			}
			return 0;
		}
		else if (c != CNTLQ && c != CNTLS)  {   /* ignore Control S/Q             */ 
			putchar(c);                  /* echo and store character       */
			cmdbuf[inputCount] = c;
			inputCount++; 
			if(inputCount < MAX_CMD_LEN - 1  &&  c != LF)
				return 0;
			else{
				cmdbuf[--inputCount] = 0; 
				return 1;
			}
		}
	}
	return 0;
}


/**
  * @brief  Display the Main Menu on HyperTerminal
  * @param  None
  * @retval None
  */
void Main_Menu(void)
{
  char cmdname[15] = {0};                            /* command input buffer        */
  int i, j;                                       /* index for command buffer    */

	if(getline()==0)                              /* input command line          */
		return;

	for (i = 0; cmdbuf[i] == ' '; i++);        /* skip blanks on head         */
	for (; cmdbuf[i] != 0; i++)  {             /* convert to upper characters */
		cmdbuf[i] = toupper(cmdbuf[i]);
	}

	for (i = 0; cmdbuf[i] == ' '; i++);        /* skip blanks on head         */
	for(j=0; cmdbuf[i] != ' '&&cmdbuf[i] != 0; i++,j++)  {         /* find command name       */
		cmdname[j] = cmdbuf[i];
	}
	cmdname[j] = '\0';

	/***************** Command: WPS configuration  *************************/
	if(strcmp(cmdname, "WPS") == 0 || strcmp(cmdname, "1") == 0) {
		printf ("\nWPS started......, press WPS button on your AP\n"); 
		menu_enable	= 0;	
		wlan_disconnect();
    OpenConfigmodeWPS(60);
	}
	/***************** Command: Easylink configuration  *************************/
	else if(strcmp(cmdname, "EASYLINK") == 0 || strcmp(cmdname, "2") == 0)	{
		printf ("\nEasyLink started......, start your easylink function on iOS/Android APP\n");
		menu_enable	= 0;	
		wlan_disconnect();
    OpenEasylink(60);									   	
	}
	/***************** Command: Easylink configuration  v2*************************/
	else if(strcmp(cmdname, "EasyLink_V2") == 0 || strcmp(cmdname, "3") == 0)	{
		printf ("\nEasyLink v2  started......, start your easylink function on iOS/Android APP\n");
		menu_enable	= 0;	
		wlan_disconnect();
    OpenEasylink2_withdata(60);									   	
	}
 /***************** Command: Reboot *************************/
	else if(strcmp(cmdname, "REBOOT") == 0 || strcmp(cmdname, "4") == 0)  {
		NVIC_SystemReset();                            
	}

	else if(strcmp(cmdname, "HELP") == 0 || strcmp(cmdname, "?") == 0)	{
		printf ("%s", menu);                         /* display command menu        */
	}

	else if(strcmp(cmdname, "") == 0 )	{                         
	}
	
	else{
		printf (ERROR_STR, "UNKNOWN COMMAND");
	}
	
	memset(cmdbuf, 0x0, MAX_CMD_LEN);
	inputCount = 0;

	if(menu_enable)
	  printf ("\nMXCHIP> ");
}

/**
  * @}
  */

/*******************(C)COPYRIGHT 2011 STMicroelectronics *****END OF FILE******/
