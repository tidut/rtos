/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-thermo.c
 * Description 	:	Thermocouple
 * Written by 	: 
 * *******************************************************
 * *******************************************************/


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
#include "er-example-server.h"
#include "er-coap-transactions.h"
#include "er-coap-separate.h"
#include "er-plugtest.h"
#include "./dev/i2c.h"

#define READ					0x01
#define WRITE   				0x00
#define LTC2483_SLAVE_ADDRESS  	0x28

#define BUSYWAIT_UNTIL(cond, max_time)                                       \
  do {                                                                       \
    rtimer_clock_t t0;                                                       \
    t0 = RTIMER_NOW();                                                       \
    while(!(cond) && RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + (max_time))) {}      \
  } while(0)
  
struct fourbytes
{		
	signed char te0;
	signed char te1;	
	signed char te2;		
	signed char te3;		
};
union
{
	signed int bits32;
	struct fourbytes by;
} adc_code;
	
unsigned int Temprature_Value_u32 = 0;

// bitwise OR with address for read or write
// The one and only LTC248X in this circuit,
// with both address lines floating.

void Calculate_temprature(void);
signed int Read_LTC2483(char addr);

/**************************************************************************
 * Function 	: 	Call_Thermocouple_Sensor
 * Description	:	To get the value from Thermocouple_Sensor
***************************************************************************/
void Call_Thermocouple_Sensor(void)
{
	printf("\r\nThermocouple Sensor is working ...............");
	printf("\r\n");
	Calculate_temprature();
}

/**************************************************************************
 * Function 	: 	Calculate_temprature
 * Description	:	Calculate Thermocouple_Sensor
***************************************************************************/
void Calculate_temprature(void)
{
	signed int x; // Integer result from LTC2481
	float voltage;  // Variable for floating point math

	x = Read_LTC2483(LTC2483_SLAVE_ADDRESS|READ);
	
	x ^= 0x80000000;
	// Invert MSB, result is 2’s complement
	voltage = (float) x;
	// convert to float
	voltage = voltage * 3.3 / 2147483648.0;// Multiply by Vref, divide by 2^31
	printf("\r\n Thermocouple Temprature Value is %d",voltage);	
}


/**************************************************************************
 * Function 	: 	Calculate_temprature
 * Description	:	To read the value from Thermocouple_Sensor
***************************************************************************/

signed int Read_LTC2483(char addr)
{
	printf("\r\n STEP 0");
	//i2c_master_set_slave_address(0x48, I2C_SEND);
	//i2c_master_command(I2C_MASTER_CMD_SINGLE_SEND);
	
	i2c_master_set_slave_address((0x48), I2C_RECEIVE);	
	i2c_master_command(I2C_MASTER_CMD_BURST_RECEIVE_START);
    BUSYWAIT_UNTIL(!(i2c_master_busy()), RTIMER_SECOND/100);
	if (i2c_master_busy() || (i2c_master_error() != I2C_MASTER_ERR_NONE)) 
	{
		printf("\r\n Timeout or I2C error");
	} 
	else 
	{
		printf("\r\n I2C fine"); 

		i2c_master_set_slave_address((LTC2483_SLAVE_ADDRESS), I2C_RECEIVE);	
		i2c_master_command(I2C_MASTER_CMD_BURST_RECEIVE_START);
		printf("\r\n STEP 1");
		while(i2c_master_busy());	
		
		printf("\r\n STEP 2");
		adc_code.by.te3  = i2c_master_data_get();
		
		i2c_master_command(I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		while(i2c_master_busy());	
		adc_code.by.te2  = i2c_master_data_get();
		
		printf("\r\n STEP 3");
		i2c_master_command(I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		while(i2c_master_busy());
		adc_code.by.te1   = i2c_master_data_get();

		adc_code.by.te0 = 0;
		printf("\r\n STEP 4");
	}
	return adc_code.bits32;
} 

/************************************* Air Sensor GET handler ********************************
 **********************************************************************************************************
***********************************************************************************************************/


static void res_thermocouple_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	Calculate_temprature();
	
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);	
	REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "Temparature = %d celsius", Temprature_Value_u32));
}

EVENT_RESOURCE(res_thermocouple,"title=\"Event thermocouple\";obs",res_thermocouple_get_handler,NULL,NULL,NULL,NULL);


/***************************************************END******************************************/



