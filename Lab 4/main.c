/*
 * main.c - Example project for UT.6.02x Embedded Systems - Shape the World
 * Jonathan Valvano and Ramesh Yerraballi
 * July 14, 2015
 * Hardware requirements 
     TM4C123 LaunchPad, optional Nokia5110
     CC3100 wifi booster and 
     an internet access point with OPEN, WPA, or WEP security
 
 * derived from TI's getweather example
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * Application Name     -   Get weather
 * Application Overview -   This is a sample application demonstrating how to
                            connect to openweathermap.org server and request for
              weather details of a city.
 * Application Details  -   http://processors.wiki.ti.com/index.php/CC31xx_SLS_Get_Weather_Application
 *                          doc\examples\sls_get_weather.pdf
 */
 /* CC3100 booster pack connections (unused pins can be used by user application)
Pin  Signal        Direction      Pin   Signal     Direction
P1.1  3.3 VCC         IN          P2.1  Gnd   GND      IN
P1.2  PB5 UNUSED      NA          P2.2  PB2   IRQ      OUT
P1.3  PB0 UART1_TX    OUT         P2.3  PE0   SSI2_CS  IN
P1.4  PB1 UART1_RX    IN          P2.4  PF0   UNUSED   NA
P1.5  PE4 nHIB        IN          P2.5  Reset nRESET   IN
P1.6  PE5 UNUSED      NA          P2.6  PB7  SSI2_MOSI IN
P1.7  PB4 SSI2_CLK    IN          P2.7  PB6  SSI2_MISO OUT
P1.8  PA5 UNUSED      NA          P2.8  PA4   UNUSED   NA
P1.9  PA6 UNUSED      NA          P2.9  PA3   UNUSED   NA
P1.10 PA7 UNUSED      NA          P2.10 PA2   UNUSED   NA

Pin  Signal        Direction      Pin   Signal      Direction
P3.1  +5  +5 V       IN           P4.1  PF2 UNUSED      OUT
P3.2  Gnd GND        IN           P4.2  PF3 UNUSED      OUT
P3.3  PD0 UNUSED     NA           P4.3  PB3 UNUSED      NA
P3.4  PD1 UNUSED     NA           P4.4  PC4 UART1_CTS   IN
P3.5  PD2 UNUSED     NA           P4.5  PC5 UART1_RTS   OUT
P3.6  PD3 UNUSED     NA           P4.6  PC6 UNUSED      NA
P3.7  PE1 UNUSED     NA           P4.7  PC7 NWP_LOG_TX  OUT
P3.8  PE2 UNUSED     NA           P4.8  PD6 WLAN_LOG_TX OUT
P3.9  PE3 UNUSED     NA           P4.9  PD7 UNUSED      IN (see R74)
P3.10 PF1 UNUSED     NA           P4.10 PF4 UNUSED      OUT(see R75)

UART0 (PA1, PA0) sends data to the PC via the USB debug cable, 115200 baud rate
Port A, SSI0 (PA2, PA3, PA5, PA6, PA7) sends data to Nokia5110 LCD

*/
#include "..\cc3100\simplelink\include\simplelink.h"
#include "../inc/tm4c123gh6pm.h"
#include "ADCSWTrigger.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "application_commands.h"
#include "LED.h"
#include "Nokia5110.h"
#include "ST7735.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SSID_NAME  "SGN4"        /* Access point name to connect to. */
#define SEC_TYPE   SL_SEC_TYPE_WPA
#define PASSKEY    "123454321"        /* Password in case of secure AP */
#define BAUD_RATE   115200
void UART_Init(void){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioConfig(0,BAUD_RATE,50000000);
}

#define MAX_RECV_BUFF_SIZE  1024
#define MAX_SEND_BUFF_SIZE  512
#define MAX_HOSTNAME_SIZE   40
#define MAX_PASSKEY_SIZE    32
#define MAX_SSID_SIZE       32


#define SUCCESS             0

#define CONNECTION_STATUS_BIT   0
#define IP_AQUIRED_STATUS_BIT   1

/* Application specific status/error codes */
typedef enum{
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,/* Choosing this number to avoid overlap w/ host-driver's error codes */

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;


/* Status bits - These are used to set/reset the corresponding bits in 'g_Status' */
typedef enum{
    STATUS_BIT_CONNECTION =  0, /* If this bit is:
                                 *      1 in 'g_Status', the device is connected to the AP
                                 *      0 in 'g_Status', the device is not connected to the AP
                                 */

    STATUS_BIT_IP_AQUIRED,       /* If this bit is:
                                 *      1 in 'g_Status', the device has acquired an IP
                                 *      0 in 'g_Status', the device has not acquired an IP
                                 */

}e_StatusBits;


#define SET_STATUS_BIT(status_variable, bit)    status_variable |= (1<<(bit))
#define CLR_STATUS_BIT(status_variable, bit)    status_variable &= ~(1<<(bit))
#define GET_STATUS_BIT(status_variable, bit)    (0 != (status_variable & (1<<(bit))))
#define IS_CONNECTED(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_CONNECTION)
#define IS_IP_AQUIRED(status_variable)          GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_IP_AQUIRED)

typedef struct{
    UINT8 SSID[MAX_SSID_SIZE];
    INT32 encryption;
    UINT8 password[MAX_PASSKEY_SIZE];
}UserInfo;

/*
 * GLOBAL VARIABLES -- Start
 */

char Recvbuff[MAX_RECV_BUFF_SIZE];
char SendBuff[MAX_SEND_BUFF_SIZE];
char HostName[MAX_HOSTNAME_SIZE];
unsigned long DestinationIP;
int SockID;


typedef enum{
    CONNECTED = 0x01,
    IP_AQUIRED = 0x02,
    IP_LEASED = 0x04,
    PING_DONE = 0x08

}e_Status;
UINT32  g_Status = 0;
/*
 * GLOBAL VARIABLES -- End
 */


 /*
 * STATIC FUNCTION DEFINITIONS  -- Start
 */

static int32_t configureSimpleLinkToDefaultState(char *);


/*
 * STATIC FUNCTION DEFINITIONS -- End
 */


void Crash(uint32_t time){
  while(1){
    for(int i=time;i;i--){};
    LED_RedToggle();
  }
}
/*
 * Application's entry point
 */
// 1) change Austin Texas to your city
// 2) you can change metric to imperial if you want temperature in F
/*void buttonInit(void);
void buttonInit(void){
	SYSCTL_RCGCGPIO_R |= 0x10;									//activate port e
	volatile int32_t delay = SYSCTL_RCGCGPIO_R; //delay
	//GPIO_PORTE_PCTL_R &= ~0x00F0FFFF; 					//GPIO PE0-3 and PE5
	GPIO_PORTE_DIR_R &= ~0x01;									//PE0 input
	GPIO_PORTE_AFSEL_R &= ~0x01;								//disable alt functions
	GPIO_PORTE_DEN_R |= 0x01;									//enable digital IO
	GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFF0F0000)+0x00000000;
	GPIO_PORTE_AMSEL_R &= ~0x01;								//disable analog functionality
}*/
void getWeather(char recvBuff[]);

char finalTemp[13];
void getWeather(char recvBuff[])
{
	
	int i=0;
	char temp[7] = "\"temp\":";
	//findTemp = recvBuff;
//	findTemp = strstr(recvBuff, temp);
	int flag = 0, k=0;
	char* findTemp;
	findTemp = strstr(recvBuff, temp);
	strncpy(finalTemp, findTemp, 12);
	
}

char AppSpot[35];
/*void sendToServer();
void sendToServer()
{
	int32_t retVal;  SlSecParams_t secParams;
  char *pConfig = NULL; INT32 ASize = 0; SlSockAddrIn_t  Addr;
	
}*/

//#define REQUEST "GET /data/2.5/weather?q=Austin%20Texas&units=metric HTTP/1.1\r\nUser-Agent: Keil\r\nHost:api.openweathermap.org\r\nAccept: */*\r\n\r\n"
#define REQUEST "GET /data/2.5/weather?q=Austin%20Texas&APPID=b80489a17aecd9fb3607c56947abbdbd HTTP/1.1\r\nHost:api.openweathermap.org\r\n\r\n"
//#define QUERY "GET /query?city=Austin&id=Akshans%20and%20Clint&greet=ADCData&edxcode=8086 HTTP/1.1\r\nUser-Agent: Keil\r\nHost: embsysmooc.appspot.com\r\n\r\n"
char QUERY[] = "GET /query?city=Austin&id=Akshans%20and%20Clint&greet=Voltage:%200000&edxcode=8086 HTTP/1.1\r\nUser-Agent: Keil\r\nHost: embsysmooc.appspot.com\r\n\r\n";
char QUERY2[] = "&edxcode=8086 HTTP/1.1\r\nUser-Agent: Keil\r\nHost: embsysmooc.appspot.com\r\n\r\n";
char QUERY1[] = "GET /query?city=Austin&id=Akshans%20and%20Clint&greet=";
//#define ADCData "ADCData"
uint32_t data = 0;
char ADCValue[] = "1000";
uint32_t times[11];
//void Timer0A_Init100HzInt(void);
void Timer1_Init(void);
void Timer1_Init(void)
{ 
  volatile uint32_t delay; 
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1 
  delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating 
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup 
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode 
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, down-count  
  TIMER1_TAILR_R = 0xFFFFFFFF;  // 4) reload value 
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution 
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A 
}
/*void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
  PF2 ^= 0x04;                   // profile
	

	if(counter != 1000){
		tData[counter] = ADCvalue;
		times[counter] = TIMER1_TAR_R;
		
		counter++;
	}
}*/
uint32_t time1 = 0, time2 = 0;
int main(void){int32_t retVal;  SlSecParams_t secParams;
  char *pConfig = NULL; INT32 ASize = 0; SlSockAddrIn_t  Addr;
	ST7735_InitR(INITR_REDTAB);				//LCD init
	ST7735_FillScreen(0);  // set screen to black
  ST7735_SetCursor(0,0);
  initClk();        // PLL 50 MHz
	//SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
//	ADC0_SAC_R |= 0x02;							// hardware avging
	Timer1_Init();
  ADC0_InitSWTriggerSeq3_Ch9();
  UART_Init();      // Send data to PC, 115200 bps
  LED_Init();       // initialize LaunchPad I/O 
  UARTprintf("Weather App\n");
  retVal = configureSimpleLinkToDefaultState(pConfig); // set policies
  if(retVal < 0)Crash(4000000);
  retVal = sl_Start(0, pConfig, 0);
  if((retVal < 0) || (ROLE_STA != retVal) ) Crash(8000000);
  secParams.Key = PASSKEY;
  secParams.KeyLen = strlen(PASSKEY);
  secParams.Type = SEC_TYPE; // OPEN, WPA, or WEP
  sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
  while((0 == (g_Status&CONNECTED)) || (0 == (g_Status&IP_AQUIRED))){
    _SlNonOsMainLoopTask();
  }
  UARTprintf("Connected\n");
	data = ADC0_InSeq3();	//Print data. 
	sprintf(ADCValue,"%zu",(int)data);
	char* findADCData;
	findADCData = strstr(QUERY, "Voltage:%20") + 11;
	strncpy(findADCData, ADCValue, 4);

	
	int i = 0;
	
  while(1){
		data = ADC0_InSeq3();	//Print data. 
	sprintf(ADCValue,"%zu",(int)data);
	char* findADCData;
	findADCData = strstr(QUERY, "Voltage:%20") + 11;
	strncpy(findADCData, ADCValue, 4);
	
    strcpy(HostName,"openweathermap.org");
    retVal = sl_NetAppDnsGetHostByName(HostName,
             strlen(HostName),&DestinationIP, SL_AF_INET);
    if(retVal == 0){
      Addr.sin_family = SL_AF_INET;
      Addr.sin_port = sl_Htons(80);
      Addr.sin_addr.s_addr = sl_Htonl(DestinationIP);// IP to big endian 
      ASize = sizeof(SlSockAddrIn_t);
      SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
      if( SockID >= 0 ){
        retVal = sl_Connect(SockID, ( SlSockAddr_t *)&Addr, ASize);
      }
      if((SockID >= 0)&&(retVal >= 0)){
        strcpy(SendBuff,REQUEST); 
			//	time1 = TIMER1_TAR_R;
        sl_Send(SockID, SendBuff, strlen(SendBuff), 0);// Send the HTTP GET 
        sl_Recv(SockID, Recvbuff, MAX_RECV_BUFF_SIZE, 0);// Receive response
			
				
        sl_Close(SockID);
				
				
        LED_GreenOn();
        UARTprintf("\r\n\r\n");
        UARTprintf(Recvbuff);  UARTprintf("\r\n");
      }
    }
		getWeather(Recvbuff);
		UARTprintf("\n\n");
		UARTprintf(finalTemp);
		UARTprintf("\n\n");
		LED_GreenOff();
		strcpy(HostName,"embsysmooc.appspot.com");
    retVal = sl_NetAppDnsGetHostByName(HostName,
             strlen(HostName),&DestinationIP, SL_AF_INET);
    if(retVal == 0){
      Addr.sin_family = SL_AF_INET;
      Addr.sin_port = sl_Htons(80);
      Addr.sin_addr.s_addr = sl_Htonl(DestinationIP);// IP to big endian 
      ASize = sizeof(SlSockAddrIn_t);
      SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
      if( SockID >= 0 ){
        retVal = sl_Connect(SockID, ( SlSockAddr_t *)&Addr, ASize);
      }
      if((SockID >= 0)&&(retVal >= 0)){
        strcpy(SendBuff,QUERY); 
				time1 = TIMER1_TAR_R;
        sl_Send(SockID, SendBuff, strlen(SendBuff), 0);// Send the HTTP GET 
        sl_Recv(SockID, Recvbuff, MAX_RECV_BUFF_SIZE, 0);// Receive response 
				time2 = TIMER1_TAR_R;
        sl_Close(SockID);
				times[i] = time1 - time2;
				i++;
				if(i==10)
				{
					break;
				}
        LED_GreenOn();
        UARTprintf("\r\n\r\n");
        UARTprintf(Recvbuff);  UARTprintf("\r\n");
      }
    }
		
		char* weather;
		
    //while(Board_Input()==0){}; // wait for touch
    
			//print weather
		
  }
}

/*!
    \brief This function puts the device in its default state. It:
           - Set the mode to STATION
           - Configures connection policy to Auto and AutoSmartConfig
           - Deletes all the stored profiles
           - Enables DHCP
           - Disables Scan policy
           - Sets Tx power to maximum
           - Sets power policy to normal
           - Unregister mDNS services

    \param[in]      none

    \return         On success, zero is returned. On error, negative is returned
*/
static int32_t configureSimpleLinkToDefaultState(char *pConfig){
  SlVersionFull   ver = {0};
  UINT8           val = 1;
  UINT8           configOpt = 0;
  UINT8           configLen = 0;
  UINT8           power = 0;

  INT32           retVal = -1;
  INT32           mode = -1;

  mode = sl_Start(0, pConfig, 0);


    /* If the device is not in station-mode, try putting it in station-mode */
  if (ROLE_STA != mode){
    if (ROLE_AP == mode){
            /* If the device is in AP mode, we need to wait for this event before doing anything */
      while(!IS_IP_AQUIRED(g_Status));
    }

        /* Switch to STA role and restart */
    retVal = sl_WlanSetMode(ROLE_STA);

    retVal = sl_Stop(0xFF);

    retVal = sl_Start(0, pConfig, 0);

        /* Check if the device is in station again */
    if (ROLE_STA != retVal){
            /* We don't want to proceed if the device is not coming up in station-mode */
      return DEVICE_NOT_IN_STATION_MODE;
    }
  }
    /* Get the device's version-information */
  configOpt = SL_DEVICE_GENERAL_VERSION;
  configLen = sizeof(ver);
  retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (unsigned char *)(&ver));

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
  retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);

    /* Remove all profiles */
  retVal = sl_WlanProfileDel(0xFF);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
  retVal = sl_WlanDisconnect();
  if(0 == retVal){
        /* Wait */
     while(IS_CONNECTED(g_Status));
  }

    /* Enable DHCP client*/
  retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&val);

    /* Disable scan */
  configOpt = SL_SCAN_POLICY(0);
  retVal = sl_WlanPolicySet(SL_POLICY_SCAN , configOpt, NULL, 0);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
  power = 0;
  retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&power);

    /* Set PM policy to normal */
  retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);

    /* TBD - Unregister mDNS services */
  retVal = sl_NetAppMDNSUnRegisterService(0, 0);


  retVal = sl_Stop(0xFF);


  g_Status = 0;
  memset(&Recvbuff,0,MAX_RECV_BUFF_SIZE);
  memset(&SendBuff,0,MAX_SEND_BUFF_SIZE);
  memset(&HostName,0,MAX_HOSTNAME_SIZE);
  DestinationIP = 0;;
  SockID = 0;


  return retVal; /* Success */
}




/*
 * * ASYNCHRONOUS EVENT HANDLERS -- Start
 */

/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent){
  switch(pWlanEvent->Event){
    case SL_WLAN_CONNECT_EVENT:
    {
      SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            /*
             * Information about the connected AP (like name, MAC etc) will be
             * available in 'sl_protocol_wlanConnectAsyncResponse_t' - Applications
             * can use it if required
             *
             * sl_protocol_wlanConnectAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
             *
             */
    }
    break;

    case SL_WLAN_DISCONNECT_EVENT:
    {
      sl_protocol_wlanConnectAsyncResponse_t*  pEventData = NULL;

      CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
      CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);

      pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            /* If the user has initiated 'Disconnect' request, 'reason_code' is SL_USER_INITIATED_DISCONNECTION */
      if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code){
        UARTprintf(" Device disconnected from the AP on application's request \r\n");
      }
      else{
        UARTprintf(" Device disconnected from the AP on an ERROR..!! \r\n");
      }
    }
    break;

    default:
    {
      UARTprintf(" [WLAN EVENT] Unexpected event \r\n");
    }
    break;
  }
}

/*!
    \brief This function handles events for IP address acquisition via DHCP
           indication

    \param[in]      pNetAppEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent){
  switch(pNetAppEvent->Event)
  {
    case SL_NETAPP_IPV4_ACQUIRED:
    {

      SET_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);
        /*
             * Information about the connected AP's ip, gateway, DNS etc
             * will be available in 'SlIpV4AcquiredAsync_t' - Applications
             * can use it if required
             *
             * SlIpV4AcquiredAsync_t *pEventData = NULL;
             * pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
             * <gateway_ip> = pEventData->gateway;
             *
             */

    }
    break;

    default:
    {
            UARTprintf(" [NETAPP EVENT] Unexpected event \r\n");
    }
    break;
  }
}

/*!
    \brief This function handles callback for the HTTP server events

    \param[in]      pServerEvent - Contains the relevant event information
    \param[in]      pServerResponse - Should be filled by the user with the
                    relevant response information

    \return         None

    \note

    \warning
*/
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse){
    /*
     * This application doesn't work with HTTP server - Hence these
     * events are not handled here
     */
  UARTprintf(" [HTTP EVENT] Unexpected event \r\n");
}

/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent){
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */
  UARTprintf(" [GENERAL EVENT] \r\n");
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
*/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock){
  switch( pSock->Event )
  {
    case SL_NETAPP_SOCKET_TX_FAILED:
    {
            /*
            * TX Failed
            *
            * Information about the socket descriptor and status will be
            * available in 'SlSockEventData_t' - Applications can use it if
            * required
            *
            * SlSockEventData_t *pEventData = NULL;
            * pEventData = & pSock->EventData;
            */
      switch( pSock->EventData.status )
      {
        case SL_ECLOSE:
          UARTprintf(" [SOCK EVENT] Close socket operation failed to transmit all queued packets\r\n");
          break;


        default:
          UARTprintf(" [SOCK EVENT] Unexpected event \r\n");
          break;
      }
    }
    break;

    default:
      UARTprintf(" [SOCK EVENT] Unexpected event \r\n");
    break;
  }
}
/*
 * * ASYNCHRONOUS EVENT HANDLERS -- End
 */


