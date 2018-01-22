/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-Air.c
 * Description 	:	Air sensor
 * Written by 	: 
 * *******************************************************
 * *******************************************************/
 
#include <board.h>
#include "reg.h"
#include "contiki.h"
#include <string.h>
#include "uart.h"
#include "common.h"
#include "contiki.h"
#include "er-coap.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "diag_server.h"
#include "dev/als-sensor.h"
#include "er-coap-engine.h"
#include "dev/GenericTypeDefs.h"
#include "er-coap-transactions.h"
#include "er-coap-separate.h"
#include "er-plugtest.h"
#include "er-example-server.h"

unsigned int Air_Value_u32 = 0;	

_FLAGS FLAGS;

void Call_Air_Sensor(void);
void Calculate_Air_Quality(void);

/**************************************************************************
 * Function 	: 	Call_Air_Sensor
 * Description	:	To read the value from Air Quality sensor
***************************************************************************/

void Call_Air_Sensor(void)
{
	printf("\r\nAir Sensor is working ...............");
	printf("\r\n");
	Calculate_Air_Quality();
}

void Calculate_Air_Quality(void)
{
	float sane = 0;
		
	sane = als_sensor.value(ADC_SENSOR_1);
	Air_Value_u32 = sane/1000;
			
	printf("\r\n Air sensor Value is %d ",Air_Value_u32);
	
	if(Air_Value_u32>=24)
	{
		printf("\n\rHigh pollution! Force signal active.");
	}
	else if( (Air_Value_u32>=4) && (Air_Value_u32<24))
	{
		printf("\r\n Low pollution!");
	}
	else
	{
		printf("\r\n Air fresh!");
	}	
	printf("\r\n");
}

/************************************* Air Sensor GET handler ********************************
 **********************************************************************************************************
***********************************************************************************************************/

static void res_Air_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	Calculate_Air_Quality();	
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);	
	//REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Air_Quality = %d ppm", Air_Value_u32));
	
	if(Air_Value_u32>=24)
	{
		printf("\n\rHigh pollution! Force signal active.");
		REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size,"High pollution! Force signal active."));
	}
	else if( (Air_Value_u32>=4) && (Air_Value_u32<24))
	{
		printf("\r\n Low pollution!");
		REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size,"Low pollution!"));
	}
	else
	{
		printf("\r\n Air fresh!");
		REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size,"Air fresh!"));
	}
	
}

EVENT_RESOURCE(res_Air_Quality,"title=\"Event air_quality\";obs",res_Air_get_handler,NULL,NULL,NULL,NULL);

/***************************************************END******************************************/
