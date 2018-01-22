/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-UART.c
 * Description 	:		
 * Written by 	: 
 * *******************************************************
 * *******************************************************/

#include "contiki.h"
#include <string.h>
#include "uart.h"
#include "contiki.h"
#include "er-coap.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "dev/als-sensor.h"
#include "er-coap-engine.h"


#define DEBUG 1

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((unsigned char *)addr)[0], ((unsigned char *)addr)[1], 		  ((unsigned char *)addr)[2], ((unsigned char *)addr)[3], ((unsigned char *)addr)[4], ((unsigned char *)addr)[5], ((unsigned char *)addr)[6], ((unsigned char *)addr)[7], ((unsigned char *)addr)[8], ((unsigned char *)addr)[9], ((unsigned char *)addr)[10], ((unsigned char *)addr)[11], ((unsigned char *)addr)[12], ((unsigned char *)addr)[13], ((unsigned char *)addr)[14], ((unsigned char *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

unsigned char UART_Tx_Buffer_u8[200];

/**************************************** UART POST and GET handlers *************************************/

static void res_UART_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{	
	unsigned char Index_u8;
	unsigned int Uart_BufferLen_u32 = 0;

	printf("\r\nData in Txbuffer BEFORE MODIFICATION is:: %s###############################",UART_Tx_Buffer_u8);	
	Uart_BufferLen_u32 = strlen((const char *)UART_Tx_Buffer_u8);
	
	if(UART_Tx_Buffer_u8[Index_u8] >= 0x41 || UART_Tx_Buffer_u8[Index_u8] <= 0x5A)
	{
		for (Index_u8 = 0; Index_u8 < Uart_BufferLen_u32 ;Index_u8++)
		{
			UART_Tx_Buffer_u8[Index_u8] = UART_Tx_Buffer_u8[Index_u8] + 0x20;
		}
	}
	else
	{
		for (Index_u8 = 0; Index_u8 < Uart_BufferLen_u32 ;Index_u8++)
		{
			UART_Tx_Buffer_u8[Index_u8] = UART_Tx_Buffer_u8[Index_u8] - 0x20;
		}	
	}
	
	UART_Tx_Buffer_u8[Uart_BufferLen_u32+1]  = 0;	
	printf("\r\nData in Txbuffer AFTER MODIFICATION is:: %s*****************************************",UART_Tx_Buffer_u8);
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, UART_Tx_Buffer_u8, strlen((const char *)UART_Tx_Buffer_u8));
}


static void res_UART_post_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	unsigned char Success_u8 = 1;
	unsigned char Index_u8;
	unsigned char *uart= NULL;
	unsigned char *incoming = NULL;	
	unsigned int Uart_BufferLen_u32 = 0;
	
	printf("\r\nExecuting UART POST handler.........................................");

	if(Success_u8 && (Uart_BufferLen_u32 = REST.get_post_variable(request, "uart", &incoming))) 
	{
    	PRINTF("incoming data:: %s %d\n", uart, Uart_BufferLen_u32);
	}	
	else
	{
		Success_u8 = 0;
		printf("\r\nFailed to fetch data.........................................");
	}
	
	memset(UART_Tx_Buffer_u8, 0, sizeof(UART_Tx_Buffer_u8));
	strcat((char *)UART_Tx_Buffer_u8,(const char *)incoming);

	for (Index_u8 = 0; Index_u8 < strlen((const char *)UART_Tx_Buffer_u8) ;Index_u8++)
	{
		PRINTF("\r\nServer received::%c", UART_Tx_Buffer_u8[Index_u8]);
	}
	
	printf("\r\nCopy finished........................................");		
	if (!Success_u8) 
	{
		REST.set_response_status(response, REST.status.BAD_REQUEST);
	}  	
}
RESOURCE(res_UART, "title=\"Test: POST uart=tx|rx\";rt=\"Control\"", res_UART_get_handler,res_UART_post_handler,NULL, NULL);





