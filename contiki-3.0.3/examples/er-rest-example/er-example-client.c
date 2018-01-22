/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Erbium (Er) CoAP client example.
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uart.h"
#include "common.h"
#include "contiki.h"
#include "contiki-net.h"
#include "er-coap-engine.h"
#include "dev/als-sensor.h"
#include "dev/GenericTypeDefs.h"

#if CONTIKI_TARGET_NATIVE
#include "slip-config.h"
#include "sys/stat.h"
#include "native-rdc.h"
#endif

#if CETIC_CSMA_STATS
#include "csma.h"
#endif

#if CETIC_NODE_CONFIG
#include "node-config.h"
#endif

#include "net/ip/uip.h"
#include "contiki-lib.h"
#include "net/rpl/rpl.h"
#include "net/netstack.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6.h"
#include "net/rpl/rpl-private.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip-ds6-route.h"

/****************************************/


#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((unsigned char *)addr)[0], ((unsigned char *)addr)[1], ((unsigned char *)addr)[2], ((unsigned char *)addr)[3], ((unsigned char *)addr)[4], ((unsigned char *)addr)[5], ((unsigned char *)addr)[6], ((unsigned char *)addr)[7], ((unsigned char *)addr)[8], ((unsigned char *)addr)[9], ((unsigned char *)addr)[10], ((unsigned char *)addr)[11], ((unsigned char *)addr)[12], ((unsigned char *)addr)[13], ((unsigned char *)addr)[14], ((unsigned char *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

// filter service post for POT for one node ( POT _ NODE)
#define FS_POST_POT

// filter service post for Temprature for one node ( POT _ NODE)
#define FS_POST_TEMP

// Data function modifier service post for Temprature & for one node ( POT _ NODE)
#define DFMS_POST_TEMP


// filter service post for Current for one node ( Current _ NODE)
#define FS_POST_CUR

// filter service post for Current
/*#define FS_POST_POWER*/

// Data function modifier service post for power
//#define DFMS_POST_POWER

// Filter service get operation for testing
//#define FS_GET
//#define POT_GET
//#define POWER_GET

#define GET_INTERVAL   				15
#define Client_0_POST_INTERVAL  	5
#define Client_1_POST_INTERVAL  	8

#define LOCAL_PORT      		UIP_HTONS(COAP_DEFAULT_PORT + 1)
#define REMOTE_PORT     		UIP_HTONS(COAP_DEFAULT_PORT)

#define NUMBER_OF_URLS 			12

// querry for each every post
const char msg[20][10] = {"?color=g","?color=b","?uart=t","pot=","DFMS=","PowerFS=","PotFS=","TempFS=","CurFS=","ERR="};

// URLs 
char *service_urls[NUMBER_OF_URLS] =
{ ".well-known/core", "/actuators/toggle_led1", "/actuators/leds", "/sensors/button","battery/","/Test/UART" ,"sensor/potentiometer","Test/DFMS","Test/Filter","Power"};

uint16_t Ipaddress1[10] = 	{0xaaaa, 0, 0, 0, 0x0212, 0x4b00, 0x0779, 0xb347};  // pot 
uint16_t Ipaddress2[10] = 	{0xaaaa, 0, 0, 0, 0x0212, 0x4b00, 0x042b, 0x3d06};  // curr
uint16_t Ipaddress3[10] = 	{0xaaaa, 0, 0, 0, 0x0212, 0x4b00, 0x042b, 0x3cc9};  // energy

#define SERVER_NODE1(ipaddr)   	uip_ip6addr(ipaddr, Ipaddress1[0], Ipaddress1[1], Ipaddress1[2], Ipaddress1[3], Ipaddress1[4], Ipaddress1[5], Ipaddress1[6], Ipaddress1[7]) 
#define SERVER_NODE2(ipaddr)   	uip_ip6addr(ipaddr, Ipaddress2[0], Ipaddress2[1], Ipaddress2[2], Ipaddress2[3], Ipaddress2[4], Ipaddress2[5], Ipaddress2[6], Ipaddress2[7])    
#define SERVER_NODE3(ipaddr)   	uip_ip6addr(ipaddr, Ipaddress3[0], Ipaddress3[1], Ipaddress3[2], Ipaddress3[3], Ipaddress3[4], Ipaddress3[5], Ipaddress3[6], Ipaddress3[7]) 

//PROCESS(POT_process, "Potentiometer process");
//PROCESS(UART0_receive_process, "UART Rx process");
PROCESS(er_example_client0, "Erbium Example Client 0");
PROCESS(er_example_client1, "Erbium Example Client 1");
AUTOSTART_PROCESSES(&er_example_client0,&er_example_client1);

uip_ipaddr_t server_ipaddr1;
uip_ipaddr_t server_ipaddr2;

unsigned char Uart_Rxbuf[100];
unsigned char Pot_Postbuf[32];
unsigned char Pot_buffer[32];
unsigned char Dm_Postbuffer[64];
unsigned char Fs_Postbuffer[64];
unsigned char PowerRespbuffer[64];

uint16_t Sec_count = 0;

extern volatile unsigned char FS_buffer[100];

char DFMServicePower[] 	= "(V*I)::V,I"; 
char DFMService[] 		= "(((T*9)/5)+32)::T"; //for C to F
char DFMService1[] 		= "(((T-32)*5)/9)::T"; //for F to C
char FService[] 		= "(>=0.5&<=3.3)::P";
char Temp_FService[] 	= "(>=10&<=50)::T";
char Cur_FService[] 	= "(>=5&<=19)::C";
char Power_FService[] 	= "(>=0&<=1)::P";
const char ledmsg[6][14] = {"Toggle!", "mode=on", "mode=off", "mode=on", "mode=off", "Toggle66!"};

/**
 * ************************************************
 * Handler Name :	client_chunk_handler
 * Function		: 	Default kind of handler
 * ************************************************
 * */
void client_chunk_handler(void *response)
{
	const unsigned char *chunk;
	int len = coap_get_payload(response, &chunk);

	printf("\r\n4to20 RESPONSE handler.............................");
	printf("\r\n|%.*s", len, (char *)chunk);
	printf("\r\nLength of Response is %d",len);
	printf("\r\nRespose string is ::::::: %s",chunk);
	//enablepotthread = 1;
}

/**
 * ************************************************
 * Handler Name :	pot_chunk_handler
 * Function		: 	potentionmeter handler
 * ************************************************
 * */
void pot_chunk_handler(void *response)
{
	const unsigned char *chunk;
	int len = coap_get_payload(response, &chunk);

	printf("\r\nPOT RESPONSE handler.............................");
	printf("\r\n|%.*s", len, (char *)chunk);
	printf("\r\nLength of Response is %d",len);
	printf("\r\nResponse is::::::: %s",chunk);
}

/**
 * ************************************************
 * Handler Name :	TempFS_chunk_handler
 * Function		: 	temprature handler
 * ************************************************
 * */
void TempFS_chunk_handler(void *response)
{
	const unsigned char *chunk;
	int len = coap_get_payload(response, &chunk);

	printf("\r\nTemperature FS RESPONSE handler.............................");
	printf("\r\n|%.*s", len, (char *)chunk);
	printf("\r\nLength of Response is %d",len);
	printf("\r\nRespose string is ::::::: %s",chunk);
	//enablepotthread = 1;
}

/**
 * ************************************************
 * Handler Name :	TempDFMS_chunk_handler
 * Function		: 	Data function modifier handler
 * ************************************************
 * */
void TempDFMS_chunk_handler(void *response)
{
	const unsigned char *chunk;

	int len = coap_get_payload(response, &chunk);

	printf("\r\nTemp DFMS  RESPONSE handler.............................");
	printf("\r\n|%.*s", len, (char *)chunk);
	printf("\r\nLength of Response is %d",len);
	printf("\r\nResponse is::::::: %s",chunk);
}

/**
 * ************************************************
 * Handler Name :	Filter_chunk_handler
 * Function		: 	filter handler
 * ************************************************
 * */
void Filter_chunk_handler(void *response)
{
	const unsigned char *chunk;
	int  i = 0;
	int len = coap_get_payload(response, &chunk);

	printf("\r\n|%.*s", len, (char *)chunk);
	printf("\r\nLength of Response is %d",len);
	printf("\r\nFS Buffer data is::::::: %s",chunk);
	
	memset((char *)FS_buffer,0, sizeof(FS_buffer));
	
	strcat((char *)FS_buffer,(const char *)chunk);

	for (i = 0; i<strlen((const char*)FS_buffer) ;i++)
	{
		printf("\r\nServer received::%c", FS_buffer[i]);
	}

	printf("\r\nCopy to buffer is  finished........................................");
	
	printf("\r\nGLobal buffer is %s...................",FS_buffer);
	//enablepotthread = 1;
}

/**
 * ************************************************
 * Handler Name :	Power_chunk_handler
 * Function		: 	power handler
 * ************************************************
 * */
void Power_chunk_handler(void *response)
{
	const unsigned char *chunk;
	int  i = 0;
	int len = coap_get_payload(response, &chunk);

	printf("\r\n|%.*s", len, (char *)chunk);
	printf("\r\nLength of Response is %d",len);
	printf("\r\nPower Buffer data is::::::: %s",chunk);	
	memset((char *)PowerRespbuffer,0, sizeof(PowerRespbuffer));	
	strcat((char *)PowerRespbuffer,(const char *)chunk);

	for (i = 0; i<strlen((const char*)PowerRespbuffer) ;i++)
	{
		printf("\r\nreponse rxd from server is::%c", PowerRespbuffer[i]);
	}
	printf("\r\nCopy to buffer is  finished........................................");	
	printf("\r\nGLobal buffer is %s...................",PowerRespbuffer);
}

/**
 * ******************************************************************************
 * Thread 			: 	er_example_client0
 * Description 		:	It is to post the filter service & Data function 
 * 						modifier service strings to the server	 
 * 
 * **/
PROCESS_THREAD(er_example_client0, ev, data)
{
	PROCESS_BEGIN();
	
	static coap_packet_t request[1];      /* This way the packet can be treated as pointer as usual. */
	static struct etimer et;
	
	
	// Intergrating the server address to client
	SERVER_NODE1(&server_ipaddr1);
	PRINTF("\r\nStarting er rest  Server.............\n");

	// receives all CoAP messages
	coap_init_engine();

	// Timer initialization
	etimer_set(&et, Client_0_POST_INTERVAL * CLOCK_SECOND);	

	while(1) 
	{
		PROCESS_WAIT_EVENT();

		if(ev == PROCESS_EVENT_TIMER) 
		{			
			printf("\r\n--POST  %d Sec Client timer event --\r\n",Client_0_POST_INTERVAL);
			// for debugging
			PRINT6ADDR(&server_ipaddr1);
			PRINTF("\r\nRemote port : %u\r\n", UIP_HTONS(REMOTE_PORT));
			
			#ifdef LED_POST
				/**************************************************************************/                            
				coap_init_message(request, COAP_TYPE_CON, COAP_PUT, 0);
				
				// use the URL based on peripheral
				coap_set_header_uri_path(request, service_urls[2]);
				
				if(num_g == 0)
				{
					num_g = 1;
					// post the data with query
					coap_set_header_uri_query(request, msg[1]);
					coap_set_payload(request, (unsigned char *)ledmsg[1], strlen(ledmsg[1]));
				} 			
				else if(num_g == 1)
				{
					num_g = 0;
					coap_set_header_uri_query(request, msg[2]);
					coap_set_payload(request, (unsigned char *)ledmsg[2], strlen(ledmsg[2]));
				}
				
				/** if any data comes back from any post it will go to client_chunk_handler */
				COAP_BLOCKING_REQUEST(&server_ipaddr1, REMOTE_PORT, request,client_chunk_handler);	
				printf("\r\nClient Posted %s on server [actuators/leds] resource......................",put_msg[2);
				/******************************************************************************/   
			#endif			
			
			#ifdef UART_POST
				/** added fro testing */
				/*****-----------------UART Byte--------------------------------------******/             
				coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
				coap_set_header_uri_path(request, service_urls[5]);

				coap_set_header_uri_query(request,  msg[2]);
				coap_set_payload(request, (unsigned char *)Uart_Rxbuf, strlen(Uart_Rxbuf));

				/** if any data comes back from any post it will go to client_chunk_handler */
				COAP_BLOCKING_REQUEST(&server_ipaddr1, REMOTE_PORT, request,client_chunk_handler);
				printf("\r\nClient Posted %s on server [Test/UART] resource......................",Uart_Rxbuf);				
				//memset(Uart_Rxbuf,0, sizeof(Uart_Rxbuf));
				/******************************************************************************/   
			#endif 
			
			#ifdef FS_POST_POT
				/** added here for Potentiometer */
				/*****---------------------------------------------------------------------******/      
				printf("\r\nFilter service is enabled..................");       
				coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
				coap_set_header_uri_path(request, service_urls[8]);

				strcat((char*)Fs_Postbuffer,(const char*)msg[6]);
				strcat((char*)Fs_Postbuffer,(const char*)FService);
				
				coap_set_header_uri_query(request,  msg[6]);
				coap_set_payload(request, (char*)Fs_Postbuffer, strlen(Fs_Postbuffer));

					/** if any data comes back from any post it will go to pot_chunk_handler */
				COAP_BLOCKING_REQUEST(&server_ipaddr1, REMOTE_PORT, request,pot_chunk_handler);
				printf("\r\nClient Posted %s on server [Test/FS] resource......................",FService);
				printf("\r\nClient Performed POST operation on [Test/Filter] resource of...................................... %x",Ipaddress1[7]);
				memset(Fs_Postbuffer,0, sizeof(Fs_Postbuffer));			
				/******************************************************************************/   
			#endif 
						
			#ifdef FS_POST_TEMP
				/** added here for temprature */
				/*****---------------------------------------------------------------------******/           
				coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
				coap_set_header_uri_path(request, service_urls[8]);

				strcat((char*)Fs_Postbuffer,(const char*)msg[7]);
				strcat((char*)Fs_Postbuffer,(const char*)Temp_FService);
				
				coap_set_header_uri_query(request,  msg[7]);
				coap_set_payload(request, (char*)Fs_Postbuffer, strlen(Fs_Postbuffer));

				/** if any data comes back from any post it will go to pot_chunk_handler */
				COAP_BLOCKING_REQUEST(&server_ipaddr1, REMOTE_PORT, request,TempFS_chunk_handler);
				printf("\r\nClient Posted %s on server [Testr/Filter] resource......................",Temp_FService);
				printf("\r\nClient Performed POST operation on [Test/Filter] resource of...................................... %x",Ipaddress1[7]);
				memset(Fs_Postbuffer,0, sizeof(Fs_Postbuffer));			
				/******************************************************************************/   
			#endif 
			
			#ifdef DFMS_POST_TEMP
				/** DFMS added here for temprature */
				/*****---------------------------------------------------------------------******/             
				coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
				coap_set_header_uri_path(request, service_urls[7]);

				strcat((char*)Dm_Postbuffer,(const char*)msg[4]);
				strcat((char*)Dm_Postbuffer,(const char*)DFMService);
				
				coap_set_header_uri_query(request,  msg[4]);
				coap_set_payload(request, (char*)Dm_Postbuffer, strlen(Dm_Postbuffer));

				COAP_BLOCKING_REQUEST(&server_ipaddr1, REMOTE_PORT, request,TempDFMS_chunk_handler);
				printf("\r\nClient Posted %s on server [Test/DFMS] resource......................",DFMService);
				printf("\r\nClient Performed POST operation on [Test/DFMS] resource of...................................... %x",Ipaddress1[7]);
				memset(Dm_Postbuffer,0, sizeof(Dm_Postbuffer));			
				/******************************************************************************/   
			#endif 			
		
			#ifdef DFMS_POST_POWER
				/*****---------------------------------------------------------------------******/             
				coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
				coap_set_header_uri_path(request, service_urls[7]);

				strcat((char*)Dm_Postbuffer,(const char*)msg[4]);
				strcat((char*)Dm_Postbuffer,(const char*)DFMServicePower);
				
				coap_set_header_uri_query(request,  msg[4]);
				coap_set_payload(request, (char*)Dm_Postbuffer, strlen(Dm_Postbuffer));

				COAP_BLOCKING_REQUEST(&server_ipaddr1, REMOTE_PORT, request,client_chunk_handler);
				printf("\r\nClient Posted %s on server [Test/DFMS] resource......................",Dm_Postbuffer);
				memset(Dm_Postbuffer,0, sizeof(Dm_Postbuffer));
				printf("\r\nClient Performed POST operation on [Test/DFMS] resource of...................................... %x",Ipaddress1[7]);
				/******************************************************************************/   
			#endif 
		
			#ifdef FS_POST_POWER
				/*****---------------------------------------------------------------------******/             
				coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
				coap_set_header_uri_path(request, service_urls[8]);

				strcat((char*)Fs_Postbuffer,(const char*)msg[5]);
				strcat((char*)Fs_Postbuffer,(const char*)Power_FService);
				
				coap_set_header_uri_query(request,  msg[5]);
				coap_set_payload(request, (char*)Fs_Postbuffer, strlen(Fs_Postbuffer));

				COAP_BLOCKING_REQUEST(&server_ipaddr1, REMOTE_PORT, request,client_chunk_handler);
				printf("\r\nClient Posted %s on server [Test/FS] resource......................",Fs_Postbuffer);
				memset(Fs_Postbuffer,0, sizeof(Fs_Postbuffer));
				printf("\r\nClient Performed POST operation on [Test/FS] resource of...................................... %x",Ipaddress1[7]);		
				/******************************************************************************/   
			#endif 
			
			printf("\r\n--Done--\r\n");
			Sec_count ++;
			etimer_reset(&et);
		}
		
		if(Sec_count >= 1)
		{		
			Sec_count = 0;
			/** all are used for GET operations */
			#ifdef BUTTON_GET
				printf("\r\n--GET  %d Sec timer exipred --\r\n",GET_INTERVAL);			
				coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
				coap_set_header_uri_path(request, service_urls[3]);
				COAP_BLOCKING_REQUEST(&server_ipaddr1, REMOTE_PORT, request,client_chunk_handler);
				printf("\r\nClient Performed GET operation on [/sensors/button] resource" );
			#endif
			
			#ifdef POT_GET
				coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
				coap_set_header_uri_path(request, service_urls[7]);
				COAP_BLOCKING_REQUEST(&server_ipaddr1, REMOTE_PORT, request,client_chunk_handler);
				printf("\r\nClient Performed GET operation on [/sensors/potentiometer] resource of %x",Ipaddress1[7]);
			#endif
			
			#ifdef FS_GET
				coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
				coap_set_header_uri_path(request, service_urls[8]);
				COAP_BLOCKING_REQUEST(&server_ipaddr1, REMOTE_PORT, request,Filter_chunk_handler);
				printf("\r\nClient Performed GET operation on [/Test/Filter] resource of %x",Ipaddress1[7]);
			#endif
			
			#ifdef POWER_GET
				coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
				coap_set_header_uri_path(request, service_urls[9]);
				COAP_BLOCKING_REQUEST(&server_ipaddr1, REMOTE_PORT, request,Power_chunk_handler);
				printf("\r\nClient Performed GET operation on [Power] resource of %x",Ipaddress1[7]);
			#endif			
			etimer_restart(&et);								
		} 
	}
	PROCESS_END();
}

/**
 * ******************************************************************************
 * Thread 			: 	er_example_client1
 * Description 		:	It is to post the filter service & Data function 
 * 						modifier service strings to the server	 
 * 
 * **/
PROCESS_THREAD(er_example_client1, ev, data)
{
	PROCESS_BEGIN();

	static coap_packet_t request[1];      /* This way the packet can be treated as pointer as usual. */
	static struct etimer etc1;
		
	SERVER_NODE2(&server_ipaddr2);
	PRINTF("\r\nStarting er rest  client1.............\n");

	/* receives all CoAP messages */
	coap_init_engine();
	etimer_set(&etc1, Client_1_POST_INTERVAL * CLOCK_SECOND);		
	
	while(1) 
	{
		PROCESS_WAIT_EVENT();

		if(ev == PROCESS_EVENT_TIMER) 
		{			
			printf("\r\n--POST  %d Sec Client1 event timer --\r\n",Client_1_POST_INTERVAL);

			PRINT6ADDR(&server_ipaddr2);
			PRINTF("\r\nRemote port : %u\r\n", UIP_HTONS(REMOTE_PORT));
			memset(Fs_Postbuffer,0, sizeof(Fs_Postbuffer));

			#ifdef FS_POST_CUR
				/*****---------------------------------------------------------------------******/         
				coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
				coap_set_header_uri_path(request, service_urls[8]);

				strcat((char*)Fs_Postbuffer,(const char*)msg[8]);
				strcat((char*)Fs_Postbuffer,(const char*)Cur_FService);
				
				coap_set_header_uri_query(request,  msg[8]);
				coap_set_payload(request, (char*)Fs_Postbuffer, strlen(Fs_Postbuffer));

				COAP_BLOCKING_REQUEST(&server_ipaddr2, REMOTE_PORT, request,client_chunk_handler);
				printf("\r\nClient Posted %s on server [Testr/Filter] resource......................",Cur_FService);
				printf("\r\nClient Performed POST operation on [Test/Filter] resource of...................................... %x",Ipaddress2[7]);
				memset(Fs_Postbuffer,0, sizeof(Fs_Postbuffer));			
				/******************************************************************************/   
			#endif 					
			etimer_reset(&etc1);
		}
	}
	PROCESS_END();
}























