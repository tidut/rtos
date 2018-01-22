/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-4to20.c
 * Description 	:	Current sensor
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

unsigned int Current_Value_u32 = 0;	
extern unsigned char Current_Fs_RXD_u8;

_FLAGS FLAGS;

void Calculate_Current(void);
void Call_Current_Sensor(void);
/************************************* currrent (4to20 mA) GET handler ********************************
 * Note:: Pot1 and 4to20 sensor both using same ADC channel this need to be a seperate binary file.
***********************************************************************************************************
***********************************************************************************************************/

unsigned char Result_outofrange = 1;	

void Call_Current_Sensor(void)
{
	printf("\r\n Current Sensor is working ...............");	
	Calculate_Current();
	printf("\r\n Current Sensor output is %d mA  " , Current_Value_u32);
}

void Calculate_Current(void)
{	
	float temp_cur = 0;
	unsigned char *buffer;
	unsigned int Adc_Value_u32;
	
	Adc_Value_u32 = als_sensor.value(ADC_SENSOR_1);
	temp_cur = (Adc_Value_u32 * 3.25) / 32768;
	temp_cur = temp_cur/145;
	temp_cur = temp_cur*1000;
	Current_Value_u32 = temp_cur;	
	
	// check filter is enabled or not
	if(Current_Fs_RXD_u8)
	{
		Result_outofrange =  DoFiltering(3); 
		snprintf((char *)buffer, 30, "Current = %d mA", Current_Value_u32);
		printf("\r\n My Calculation %s",(char *)buffer);
	}
}

static void res_current_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{	
	unsigned char Error_Buffer_u8[20];
	
	Calculate_Current();
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);

	// check filter is enabled or not
	if(Current_Fs_RXD_u8)
	{
		Result_outofrange =  DoFiltering(3); 	
		if(!Result_outofrange)
		{
			// if current value is out of range then reset the flag
			FLAGS.PS_Result_outofrange = 0;
			memset(Error_Buffer_u8,0, sizeof(Error_Buffer_u8));
			strcat(Error_Buffer_u8,"ERROR CODE = 04");
			REST.set_response_payload(response, Error_Buffer_u8, strlen(Error_Buffer_u8));
		}
		else
		{
			FLAGS.PS_Result_outofrange = 1;
			REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Current = %d mA", Current_Value_u32));
		}
	}	
	else
	{
		FLAGS.PS_Result_outofrange = 1;
		REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Current = %d mA", Current_Value_u32));
	}
}
EVENT_RESOURCE(res_current,"title=\"Current\";obs",res_current_get_handler,NULL,NULL,NULL,NULL);

/***************************************************END******************************************/

