/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-adc.c
 * Description 	:		
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

_FLAGS FLAGS;
unsigned int Temp_Lm61_u32 = 0;
	
extern unsigned char DFMS_Rxd_u8;
extern unsigned char Pot_Fs_RXD_u8;
extern unsigned char Temp_Fs_RXD_u8;
extern volatile float Temprature_Final_Res;

/************************************* Potentiometer1 added GET handler ********************************
***********************************************************************************************************
***********************************************************************************************************/

volatile float POT_1_Voltage = 0;
float POT_1_frac = 0;		
unsigned int POT_1_dec = 0;	

volatile float POT_2_Voltage = 0;
float POT_2_frac = 0;		
unsigned int POT_2_dec = 0;	

float Temprature_frac_Val = 0;
unsigned int Temprature_dec_Val = 0;

unsigned char Pot1_Result_outofrange_u8 = 1;
unsigned char Pot2_Result_outofrange_u8 = 1;
unsigned char lm61_Result_outofrange_u8 = 1;

void Calculate_POT_1_Value(void);
void Calculate_POT_2_Value(void);
void Call_Pot_Temp_Sensor(void);
void Calculate_Temprature_Value(void);

void Call_Pot_Temp_Sensor(void)
{
	printf("\r\n Temprature + POT Sensor is working  ...........");	
		
	Calculate_POT_1_Value();
	Calculate_POT_2_Value();
	Calculate_Temprature_Value();

	// Send alert message if battery volatge is lower than threshold
	if(Battery_Low_Alert_Flag)
	{
		Read_SOC_Reg_and_CompareThreshold();					
		dpars.Batinfo.Batterylowindication = 1;
	}
	else
	{
		dpars.Batinfo.Batterylowindication = 0;
	}
	
	// Send Battery volatge to application			
	Read_Vcell();
	dpars.Batinfo.Batterycapacity = Battery_Vout_inMv;	
	
	// Read charger connection
	CheckChargerConnectionStatus();

	if(Charger_Connection_Status)
	{
		dpars.Batinfo.chargeConnectionstatus = 1;
	}
	else
	{
		dpars.Batinfo.chargeConnectionstatus = 0;
	}
}

void Calculate_POT_1_Value(void)
{		
	POT_1_Voltage = (als_sensor.value(ADC_SENSOR_1) * 2.5 )/65536;		
	POT_1_dec = POT_1_Voltage;
	POT_1_frac = POT_1_Voltage - POT_1_dec;
	printf("\r\n Pot 1 Value is %d.%02lu  ",POT_1_dec,(unsigned int)(POT_1_frac*100));
	if(Pot_Fs_RXD_u8)
	{
		Pot1_Result_outofrange_u8 =  DoFiltering(0);
	}
}

static void res_pot1_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	unsigned char pot1_ERR_buffer_u8[20] = "ERROR CODE = 04";
	
	Calculate_POT_1_Value();
	
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	
	// Look for potentiometer filter service variable
	if(Pot_Fs_RXD_u8)
	{
		if(!Pot1_Result_outofrange_u8)
		{
			REST.set_response_payload(response, pot1_ERR_buffer_u8, strlen(pot1_ERR_buffer_u8));
		}
		else
		{
			REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Output=%d.%02lu volts", POT_1_dec, (unsigned int)(POT_1_frac*100)));
		}
	}	
	else
	{
		REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Output=%d.%02lu volts", POT_1_dec, (unsigned int)(POT_1_frac*100)));
	}
}
EVENT_RESOURCE(res_pot1,"title=\"Event POT_1\";obs",res_pot1_get_handler,NULL,NULL,NULL,NULL);


/************************************* Potentiometer2 added GET handler ********************************
***********************************************************************************************************
***********************************************************************************************************/

void Calculate_POT_2_Value(void)
{
	POT_2_Voltage = (als_sensor.value(ADC_SENSOR_2) * 2.5 )/65536;		
	POT_2_dec = POT_2_Voltage;
	POT_2_frac = POT_2_Voltage - POT_2_dec;
	printf("\r\n Pot 2 Value is %d.%02lu  ",POT_2_dec,(unsigned int)(POT_2_frac*100));
	if(Pot_Fs_RXD_u8)
	{
		Pot2_Result_outofrange_u8 =  DoFiltering(1);
	}
}

static void res_pot2_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	unsigned char pot2_ERR_buffer_u8[20] = "ERROR CODE = 04";		
	
	Calculate_POT_2_Value();
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	
	// Look for potentiometer filter service variable	
	if(Pot_Fs_RXD_u8)
	{	
		if(!Pot2_Result_outofrange_u8)
		{
			REST.set_response_payload(response, pot2_ERR_buffer_u8, strlen(pot2_ERR_buffer_u8));
		}
		else
		{
			REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Output=%d.%02lu volts", POT_2_dec, (unsigned int)(POT_2_frac*100)));
		}
	}
	else
	{
		REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Output=%d.%02lu volts", POT_2_dec, (unsigned int)(POT_2_frac*100)));
	}
}
EVENT_RESOURCE(res_pot2,"title=\"Event POT_2\";obs",res_pot2_get_handler,NULL,NULL,NULL,NULL);


/************************************* LM61    added GET handler ********************************
***********************************************************************************************************
***********************************************************************************************************/	


void Calculate_Temprature_Value(void)
{
	float sane = 0;
		
	printf("\r\n\r\nRaw Temprature sensor Value ===== %d \r\n", (uint16_t)als_sensor.value(ADC_SENSOR_0));
	sane = (als_sensor.value(ADC_SENSOR_0) * 2.33)/65536;
	sane = (sane - 0.6)/10;
	Temprature_dec_Val = sane;
	Temprature_frac_Val = sane - Temprature_dec_Val;
	Temp_Lm61_u32 = ((sane*1000));
				
	printf("\r\n Temprature sensor Value is %d.%d ",Temprature_dec_Val,Temp_Lm61_u32);
	if(Temp_Fs_RXD_u8)
	{	
		lm61_Result_outofrange_u8 =  DoFiltering(2);
	}
}

static void res_lm61_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{

	unsigned char lmERR_buf[20] = "ERROR CODE = 04";

	Calculate_Temprature_Value();
	
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);	
	
	// Look for temprature filter service variable	
	if(Temp_Fs_RXD_u8)
	{		
		if(!lm61_Result_outofrange_u8)
		{
			FLAGS.TS_Result_outofrange = 1;
			REST.set_response_payload(response, lmERR_buf, strlen(lmERR_buf));
		}	
		else
		{
			//temperature will be convreted from celcius to faranheat get that value
			// Data modification
			if(DFMS_Rxd_u8)
			{			
				Temprature_dec_Val = Temprature_Final_Res;
				Temprature_frac_Val = Temprature_Final_Res - Temprature_dec_Val;	 			
				REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Temperature = %d.%02lu F", Temprature_dec_Val, (unsigned int)(Temprature_frac_Val*100)));
			}		
			else
			{
				REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Temperature = %d deg C", Temp_Lm61_u32));
			}
		}
	}	
	else
	{
		//temperature will be convreted from celcius to faranheat get that value
		if(DFMS_Rxd_u8)
		{
				Temprature_dec_Val = Temprature_Final_Res;
				Temprature_frac_Val = Temprature_Final_Res - Temprature_dec_Val;
				REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Temperature = %d.%02u F", Temprature_dec_Val, (unsigned int)(Temprature_frac_Val*100)));
		}	
		else
		{
			REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Temperature = %d deg C", Temp_Lm61_u32));
		}
	}
}
EVENT_RESOURCE(res_lm61,"title=\"Event lm61\";obs",res_lm61_get_handler,NULL,NULL,NULL,NULL);
/***********************************************************************************************/



