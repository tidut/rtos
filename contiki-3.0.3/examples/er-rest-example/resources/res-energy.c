/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-energy.c
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
#include "sys/ctimer.h"
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

void Do_Mdb_PF_ADDRESS(void);	
void Call_Energy_Sensor(void); 						
void Do_Mdb_VOLTAGE_R_ADDRESS(void);
void Do_Mdb_CURRENT_R_ADDRESS(void);	
void Do_Mdb_FREQUENCY_ADDRESS(void);	 			
void Do_Mdb_WATTS_PER_HR_ADDRESS(void);	

unsigned char VOL_Buffer_u8[50];
unsigned char CURR_Buffer_u8[50];
unsigned char PF_Buffer_u8[50];
unsigned char FRE_Buffer_u8[50];
unsigned char WH_Buffer_u8[50];
extern unsigned char UART_1_RxBuffer_u8[100];

float Voltage_Fractional_Value;
float Current_Fractional_Value;
float PF_Fractional_Value;
float Freq_Fractional_Value;
float Wph_Fractional_Value;

unsigned int Voltage_Decimal_u32;	
unsigned int Current_Decimal_u32;	
unsigned int PF_Decimal_u32;	
unsigned int Freq_Decimal_u32;	
unsigned int Wph_Decimal_u32;	

/**********************************************************************************
	* Function Name 	: 		Do_Mdb_VOLTAGE_R_ADDRESS
	* Description		:		Reads Voltage from Energy Meter
**********************************************************************************/

void Call_Energy_Sensor(void)
{
	printf("\r\n Energy Sensor is working  ...............");	
}


void Do_Mdb_VOLTAGE_R_ADDRESS(void)
{
	unsigned char Index_u8;	
	unsigned char Raw_Buffer_u8[4];	
	unsigned int Temp_Data_u32;	
	
	float Fract_Data;
	UINT32_VAL result;
	
	for(Index_u8 = 0;Index_u8 <= 8;Index_u8++)
	{
		VOL_Buffer_u8[Index_u8] = UART_1_RxBuffer_u8[Index_u8];
		UART_1_RxBuffer_u8[0] = 0;
		printf("\r\n VOL_Buffer_u8 value = 0x%x",VOL_Buffer_u8[Index_u8]);
	}
	
	for(Index_u8 = 0;Index_u8<=3;Index_u8++)
	{
		Raw_Buffer_u8[Index_u8] = VOL_Buffer_u8[Index_u8+3];		
	}
	
	result.byte.LB = Raw_Buffer_u8[3];
	result.byte.HB = Raw_Buffer_u8[2];	
	result.byte.UB = Raw_Buffer_u8[1];	
	result.byte.MB = Raw_Buffer_u8[0];
	
	printf("\r\n LB = 0x%X",result.byte.LB);
	printf("\r\n HB = 0x%X",result.byte.HB);
	printf("\r\n UB = 0x%X",result.byte.UB);
	printf("\r\n MB = 0x%X",result.byte.MB);

	Temp_Data_u32 = result.Val;
	Fract_Data = *((float*)&result.Val);	
	printf("\r\n Temp_Data_u32 : %d",Temp_Data_u32); 	
	printf("\r\nRESULT VALUE = %d", result.Val);
	
	Voltage_Decimal_u32 = Fract_Data;
	Voltage_Fractional_Value = Fract_Data - Voltage_Decimal_u32;
	
	printf("\r\nVolatge value rceceived from energy meter is = %d.%04lu \n",Voltage_Decimal_u32,(unsigned int)(Voltage_Fractional_Value*10000));
}

/**********************************************************************************
	* Function Name 	: 	Do_Mdb_CURRENT_R_ADDRESS
	* Description			:		Reads Current from Energy Meter
**********************************************************************************/
void Do_Mdb_CURRENT_R_ADDRESS(void)
{
	unsigned char Index_u8;	
	unsigned char Raw_Buffer_u8[4];	
	unsigned int Temp_Data_u32;	
	
	float Fract_Data;
	UINT32_VAL result;
	
	for(Index_u8 = 0;Index_u8 <= 8;Index_u8++)
	{
		CURR_Buffer_u8[Index_u8] = UART_1_RxBuffer_u8[Index_u8];
		UART_1_RxBuffer_u8[0] = 0;
		printf("\r\n CURR_Buffer_u8 value = 0x%x",CURR_Buffer_u8[Index_u8]);
	}

	for(Index_u8 = 0;Index_u8<=3;Index_u8++)
	{
		Raw_Buffer_u8[Index_u8] = CURR_Buffer_u8[Index_u8+3];		
	}
	
	result.byte.LB = Raw_Buffer_u8[3];
	result.byte.HB = Raw_Buffer_u8[2];	
	result.byte.UB = Raw_Buffer_u8[1];	
	result.byte.MB = Raw_Buffer_u8[0];
	
	printf("\r\n LB = 0x%X",result.byte.LB);
	printf("\r\n HB = 0x%X",result.byte.HB);
	printf("\r\n UB = 0x%X",result.byte.UB);
	printf("\r\n MB = 0x%X",result.byte.MB);

	Temp_Data_u32 = result.Val;
	Fract_Data = *((float*)&result.Val);	
	printf("\r\n Temp_Data_u32 : %d",Temp_Data_u32); 	
	printf("\r\nRESULT VALUE = %d", result.Val);
	
	Current_Decimal_u32 = Fract_Data;
	Current_Fractional_Value = Fract_Data - Current_Decimal_u32;
	
	printf("\r\nCurrent value rceceived from energy meter is = %d.%04lu \n",Current_Decimal_u32,(unsigned int)(Current_Fractional_Value*10000));
}

/**********************************************************************************
	* Function Name 	: 	Do_Mdb_PF_ADDRESS
	* Description			:		Reads PF from Energy Meter
**********************************************************************************/

void Do_Mdb_PF_ADDRESS(void)
{
	unsigned char Index_u8;	
	unsigned char Raw_Buffer_u8[4];	
	unsigned int Temp_Data_u32;	
	
	float Fract_Data;
	UINT32_VAL result;
	
	for(Index_u8 = 0;Index_u8 <= 8;Index_u8++)
	{
		PF_Buffer_u8[Index_u8] = UART_1_RxBuffer_u8[Index_u8];
		UART_1_RxBuffer_u8[0] = 0;
		printf("\r\n PF_Buffer_u8 value = 0x%x",PF_Buffer_u8[Index_u8]);
	}
	
	for(Index_u8 = 0;Index_u8<=3;Index_u8++)
	{
		Raw_Buffer_u8[Index_u8] = PF_Buffer_u8[Index_u8+3];		
	}
	
	result.byte.LB = Raw_Buffer_u8[3];
	result.byte.HB = Raw_Buffer_u8[2];	
	result.byte.UB = Raw_Buffer_u8[1];	
	result.byte.MB = Raw_Buffer_u8[0];
	
	printf("\r\n LB = 0x%X",result.byte.LB);
	printf("\r\n HB = 0x%X",result.byte.HB);
	printf("\r\n UB = 0x%X",result.byte.UB);
	printf("\r\n MB = 0x%X",result.byte.MB);

	Temp_Data_u32 = result.Val;
	Fract_Data = *((float*)&result.Val);	
	printf("\r\n Temp_Data_u32 : %d",Temp_Data_u32); 	
	printf("\r\nRESULT VALUE = %d", result.Val);
	
	PF_Decimal_u32 = Fract_Data;
	PF_Fractional_Value = Fract_Data - PF_Decimal_u32;
	
	printf("\r\nPF value rceceived from energy meter is = %d.%04lu \n",PF_Decimal_u32,(unsigned int)(PF_Fractional_Value*10000));
}

/**********************************************************************************
	* Function Name 	: 	Do_Mdb_FREQUENCY_ADDRESS
	* Description			:		Reads FREQUENCY from Energy Meter
**********************************************************************************/ 						
void Do_Mdb_FREQUENCY_ADDRESS(void)
{
	unsigned char Index_u8;	
	unsigned char Raw_Buffer_u8[4];	
	unsigned int Temp_Data_u32;	
	
	float Fract_Data;
	UINT32_VAL result;
	
	for(Index_u8 = 0;Index_u8 <= 8;Index_u8++)
	{
		FRE_Buffer_u8[Index_u8] = UART_1_RxBuffer_u8[Index_u8];
		UART_1_RxBuffer_u8[0] = 0;
		printf("\r\n FRE_Buffer_u8 value = 0x%x",FRE_Buffer_u8[Index_u8]);
	}
	
	for(Index_u8 = 0;Index_u8<=3;Index_u8++)
	{
		Raw_Buffer_u8[Index_u8] = FRE_Buffer_u8[Index_u8+3];		
	}
	
	result.byte.LB = Raw_Buffer_u8[3];
	result.byte.HB = Raw_Buffer_u8[2];	
	result.byte.UB = Raw_Buffer_u8[1];	
	result.byte.MB = Raw_Buffer_u8[0];
	
	printf("\r\n LB = 0x%X",result.byte.LB);
	printf("\r\n HB = 0x%X",result.byte.HB);
	printf("\r\n UB = 0x%X",result.byte.UB);
	printf("\r\n MB = 0x%X",result.byte.MB);

	Temp_Data_u32 = result.Val;
	Fract_Data = *((float*)&result.Val);	
	printf("\r\n Temp_Data_u32 : %d",Temp_Data_u32); 	
	printf("\r\nRESULT VALUE = %d", result.Val);
	
	Freq_Decimal_u32 = Fract_Data;
	Freq_Fractional_Value = Fract_Data - Freq_Decimal_u32;
	
	printf("\r\nFrequency value rceceived from AC energy meter is = %d.%04lu \n",Freq_Decimal_u32,(unsigned int)(Freq_Fractional_Value*10000));
}

/**********************************************************************************
	* Function Name 	: 	Do_Mdb_WATTS_PER_HR_ADDRESS
	* Description			:		Reads watts per hour from Energy Meter
**********************************************************************************/ 		
void Do_Mdb_WATTS_PER_HR_ADDRESS(void)
{
	unsigned char Index_u8;	
	unsigned char Raw_Buffer_u8[4];	
	unsigned int Temp_Data_u32;	
	
	float Fract_Data;
	UINT32_VAL result;
	
	for(Index_u8 = 0;Index_u8 <= 8;Index_u8++)
	{
		WH_Buffer_u8[Index_u8] = UART_1_RxBuffer_u8[Index_u8];
		UART_1_RxBuffer_u8[0] = 0;
		printf("\r\n WH_Buffer_u8 value = 0x%x",WH_Buffer_u8[Index_u8]);
	}
	
	for(Index_u8 = 0;Index_u8<=3;Index_u8++)
	{
		Raw_Buffer_u8[Index_u8] = WH_Buffer_u8[Index_u8+3];		
	}
	
	result.byte.LB = Raw_Buffer_u8[3];
	result.byte.HB = Raw_Buffer_u8[2];	
	result.byte.UB = Raw_Buffer_u8[1];	
	result.byte.MB = Raw_Buffer_u8[0];
	
	printf("\r\n LB = 0x%X",result.byte.LB);
	printf("\r\n HB = 0x%X",result.byte.HB);
	printf("\r\n UB = 0x%X",result.byte.UB);
	printf("\r\n MB = 0x%X",result.byte.MB);

	Temp_Data_u32 = result.Val;
	Fract_Data = *((float*)&result.Val);	
	printf("\r\n Temp_Data_u32 : %d",Temp_Data_u32); 	
	printf("\r\nRESULT VALUE = %d", result.Val);
	
	Wph_Decimal_u32 = Fract_Data;
	Wph_Fractional_Value = Fract_Data - Wph_Decimal_u32;
	
	printf("\r\nvalue rceceived from AC energy meter is = %d.%04lu \n",Wph_Decimal_u32,(unsigned int)(Wph_Fractional_Value*10000));
}

/************************************* Energy meter AC Voltage GET handler ********************************
***********************************************************************************************************
***********************************************************************************************************/

static void res_PM1_energy_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "AC Voltage = %d.%04lu V", Voltage_Decimal_u32, (unsigned int)	(Voltage_Fractional_Value*10000)));
}
EVENT_RESOURCE(res_volt_energy,"title=\"Test: PM1\";obs",res_PM1_energy_get_handler,NULL,NULL,NULL,NULL);

static void res_PM2_energy_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "AC Current = %d.%04lu AM", Current_Decimal_u32, (unsigned int)	(Current_Fractional_Value*10000)));
}
EVENT_RESOURCE(res_curr_energy,"title=\"Test: PM2\";obs",res_PM2_energy_get_handler,NULL,NULL,NULL,NULL);

static void res_PM3_energy_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "AC PF = %d.%04lu ", PF_Decimal_u32, (unsigned int)	(PF_Fractional_Value*10000)));
}
EVENT_RESOURCE(res_pf_energy,"title=\"Test: PM3\";obs",res_PM3_energy_get_handler,NULL,NULL,NULL,NULL);

static void res_PM4_energy_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "AC Freq = %d.%04lu Hz", Freq_Decimal_u32, (unsigned int)	(Freq_Fractional_Value*10000)));
}
EVENT_RESOURCE(res_freq_energy,"title=\"Test: PM4\";obs",res_PM4_energy_get_handler,NULL,NULL,NULL,NULL);

static void res_PM5_energy_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "AC Wph = %d.%04lu ", Wph_Decimal_u32, (unsigned int)	(Wph_Fractional_Value*10000)));
}
EVENT_RESOURCE(res_wph_energy,"title=\"Test: PM5\";obs",res_PM5_energy_get_handler,NULL,NULL,NULL,NULL);




 



