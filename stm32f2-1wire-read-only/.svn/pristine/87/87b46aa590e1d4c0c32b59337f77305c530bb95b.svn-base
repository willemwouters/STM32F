/**
  @page SSL server demonstration Readme file
 
  @verbatim
  ******************** (C) COPYRIGHT 2011 STMicroelectronics *******************
  * @file    SSL server/readme.txt
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    07-October-2011
  * @brief   Description of the STM32F217 SSL server demonstration.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
   @endverbatim

@par Description

This directory contains a set of sources files that implement a SSL server
demonstration for STM32F217 devices.

Please note that for SSL server demonstration, PolarSSL v0.12.0, FreeRTOS v6.1.0
and LwIP v1.3.2 are used:
    - PolarSSL v0.12.0 is used as the SSL/TLS library.
    - FreeRTOS v6.1.0 is used as the Real Time Kernel.
    - LwIP v1.3.2 is used as the TCP/IP stack. 

@par Project Directory contents 

 - "inc": contains the demonstration firmware header files 
    - inc/main.h               main config file 
    - inc/config.h             PolarSSL library configuration options
    - inc/FreeRTOSConfig.h     FreeRTOS configuration options
    - inc/netconf.h            header for netconf.c
    - inc/lwipopts.h           LwIP stack configuration options
    - inc/stm32F2x7_eth_bsp.h  header for stm32F2x7_eth_bsp.c
    - inc/stm32f2xx_it.h       Header for stm32f2xx_it.c
    - inc/stm32f2xx_conf.h     Library Configuration file
    - inc/stm32f2x7_eth_conf.h STM32 Ethernet driver Configuration file
    - inc/ssl_server.h         header for ssl_server.c
    - inc/tasks_misc.h         header for tasks_misc.c

 - "src": contains the demonstration firmware source files
    - src/main.c               main program file
    - src/ssl_server.c         SSL server main task
    - src/stm32F2x7_eth_bsp.c  STM32F2x7 Ethernet hardware configuration
    - src/tasks_misc.c         Tasks misc functions body
    - src/netconf.c            LwIP stack initializations
    - src/stm32f2xx_it.c       STM32 Interrupt handlers 
    - src/system_stm32f2xx.c   STM32 system clock configuration file

 - "EWARM": contains preconfigured project for EWARM toolchain
 - "MDK-ARM": contains preconfigured project for MDK-ARM toolchain
 - "RIDE": contains preconfigured project for RIDE toolchain
 - "TASKING": contains preconfigured project for TASKING toolchain
 - "TrueSTUDIO": contains preconfigured project for TrueSTUDIO toolchain

@par Hardware and Software environment

  - This example has been tested with the following environments:
     - STM322xG-EVAL board
     - Http clients: Firefox Mozilla v3.6 or Microsoft Internet Explorer v8
     - DHCP server: PC utility TFTPD32 (http://tftpd32.jounin.net/) is used as a DHCP server

  - STM322xG-EVAL Set-up 
    - Connect STM322xG-EVAL board to remote PC (through a crossover ethernet cable)
      or to your local network (through a straight ethernet cable)
    - RS232 link (used with HyperTerminal like application to display debug messages):
      connect a null-modem female/female RS232 cable between the DB9 connector CN16
      (USART3) and PC serial port.
    - STM322xG-EVAL jumpers setting
        +==========================================================================================+
        +  Jumper |       MII mode configuration            |     RMII mode configuration(*)       +
        +==========================================================================================+
        +  JP5    | 2-3 provide 25MHz clock by MCO(PA8)     |  Not fitted                          +
        +         | 1-2 provide 25MHz clock by ext. Crystal |                                      +
        + -----------------------------------------------------------------------------------------+
        +  JP6    |          2-3                            |  1-2                                 +
        + -----------------------------------------------------------------------------------------+
        +  JP8    |          Open                           |  Close                               +
        + -----------------------------------------------------------------------------------------+
        +  JP22   | 1-2: RS232 is enabled                                                          +
        +==========================================================================================+
    (*) User have to provide the 50 MHz clock by soldering a 50 MHz oscillator (ref SM7745HEV-50.0M or
        equivalent) on the U3 footprint located under CN3 and also removing jumper on JP5. This oscillator
        is not provided with the board. 
        For more details, please refer to STM3220G-EVAL evaluation board User manual (UM1057).
    @Note: the default setting is MII mode, to change it to RMII mode refer to main.h file.


@par How to use it ?

In order to make the program work, you must do the following:
  1. Load the demonstration code in the STM32F2x7 Flash memory (see below)
  2. Refer to "AN3365 Secure socket layer (SSL) for STM32F217xx microcontrollers"
     to know how to use the demonstration

 @Note: If you using a firewall, you must be sure that the web browser accepts
        connection requests. Otherwise, the firewall refuses the connection.

In order to load the demonstration code, you have do the following:
   - EWARM
      - Open the Project.eww workspace
      - Rebuild all files: Project->Rebuild all
      - Load project image: Project->Debug
      - Run program: Debug->Go(F5)

   - RIDE
      - Open the Project.rprj project
      - Rebuild all files: Project->build project
      - Load project image: Debug->start(ctrl+D)
      - Run program: Debug->Run(ctrl+F9)

   - MDK-ARM
      - Open the Project.uvproj project
      - Rebuild all files: Project->Rebuild all target files
      - Load project image: Debug->Start/Stop Debug Session
      - Run program: Debug->Run (F5)

   - TASKING
      - Open TASKING toolchain.
      - Click on File->Import, select General->'Existing Projects into Workspace'
        and then click "Next". 
      - Browse to TASKING workspace directory and select the project:
          - STM322xG_EVAL: to configure the project for STM32F2xx devices.
      - Rebuild all project files: Select the project in the "Project explorer" 
        window then click on Project->build project menu.
      - Run program: Select the project in the "Project explorer" window then click 
        Run->Debug (F11)

   - TrueSTUDO
      - Open the TrueSTUDIO toolchain.
      - Click on File->Switch Workspace->Other and browse to TrueSTUDIO workspace directory.
      - Click on File->Import, select General->'Existing Projects into Workspace' and then click "Next". 
      - Browse to the TrueSTUDIO workspace directory, select the project:
          - STM322xG_EVAL: to configure the project for STM32F2xx devices
      - Rebuild all project files: Select the project in the "Project explorer" 
        window then click on Project->build project menu.
      - Run program: Run->Debug (F11)

 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 */
