/* *******************************************************
 * *******************************************************
 * File Name 	: 	Max17043.c
 * Description 	:		
 * Written by 	: 
 * *******************************************************
 * *******************************************************/
 
#include <board.h>
#include "reg.h"
#include "contiki-net.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <contiki.h>
#include <stdbool.h>
#include <stdint.h>
#include "Max17043.h"
#include "./dev/i2c.h"
#include "dev/GenericTypeDefs.h"

#define ALERT_PIN			1
#define CHARGE_PIN			0

#define Delay_4us()   		do{ asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
								asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
								asm("nop"); asm("nop"); }while(0)


extern void DelayMs (unsigned int seconds);

UINT16_BITS ReadByte,Vbat,WriteByte;
volatile unsigned char Battery_Reset_Flag = 0;
volatile unsigned char Battery_Low_Alert_Flag = 0;
volatile unsigned char Charger_Connection_Status = 0;
volatile unsigned char Reset_MAX17043_IC_Counter = 0;
unsigned int Battery_Vout_inMv = 0;


/*******************************************************************************
 * Function Name :					Init_I2c
 * Function Description : 			Initilaize the I2C 
 * *****************************************************************************/
void Init_I2c(void)
{
	i2c_init(I2C_SDA_PORT,I2C_SDA_PIN,I2C_SCL_PORT, I2C_SCL_PIN,I2C_SCL_NORMAL_BUS_SPEED);
}

/*******************************************************************************
 * Function Name :					Max17043_I2c_Init
 * Function Description : 			Initilaize the I2C engine for charger IC
 * *****************************************************************************/
void Max17043_I2c_Init(void)
{
	Max17043_I2c_Start();
}

void Max17043_I2c_Start(void)
{
	/** Reset the Fuel guage IC */
	ResetFuelGauge();

	/** Set the threshold value */
	SetThreshold();
	
	Read_SOC_Reg_and_CompareThreshold();
	printf("\r\nMax17043 Config Done\r\n");
	
	Battery_Low_Alert_Flag = 0;
}


/*******************************************************************************
 * Function Name :			ResetFuelGauge 				 
 * Function Description : 	Resets the FuelGuage IC
 * *****************************************************************************/
void ResetFuelGauge(void)
{
	printf("\r\nReset the Fuel guage IC ");
	Battery_Low_Alert_Flag = 0;
	WriteOnI2C(COMMAND_MSB,0x00,0x54);
	DelayMs(300);
}

/*******************************************************************************
 * Function Name :			SetThreshold				 
 * Function Description : 	To set alert threshold in %
 * *****************************************************************************/
void SetThreshold(void)
{
	unsigned int I2C_Write_Error = 0;
	
	printf("\r\nSet the threshold");
	ReadOnI2C(CONFIG_MSB);
	printf("\r\ni2c Master Read LB return value Threshold Register ::%d",ReadByte.byte.LB);
	printf("\r\ni2c Master Read HB return value Threshold Register ::%d",ReadByte.byte.HB);
		
	WriteByte.byte.LB = ReadByte.byte.LB;
	WriteByte.byte.LB &= 0xE0;
	
	//default value 
	WriteByte.byte.HB = 0x97;
	
	#ifdef THRESHOLD_32
		WriteByte.byte.LB |= 0x00;
		printf("\r\nSet the threshold on IC 32 %");		
		printf("\r\ni2c Master Write LB value Threshold Register ::%d",WriteByte.byte.LB);
		printf("\r\ni2c Master Write HB value Threshold Register ::%d",WriteByte.byte.HB);	
		I2C_Write_Error = WriteOnI2C(CONFIG_MSB,WriteByte.byte.HB,WriteByte.byte.LB);
		printf("\r\ni2c Master write error value is::%d",I2C_Write_Error);		
	#elif THRESHOLD_10	
		WriteByte.byte.LB |= 0x16;
		printf("\r\ni2c Master Write LB value Threshold Register ::%d",WriteByte.byte.LB);
		printf("\r\ni2c Master Write HB value Threshold Register ::%d",WriteByte.byte.HB);
		WriteOnI2C(CONFIG_MSB,WriteByte.byte.HB,WriteByte.byte.LB);
		printf("\r\ni2c Master write error value is::%d",I2C_Write_Error);
	#endif
	
	DelayMs(300);
	
	/** Read the threshold value to compare */ 
	ReadOnI2C(CONFIG_MSB);
	printf("\r\ni2c Master Read return LB value Threshold Register ::%d",ReadByte.byte.LB);
	printf("\r\ni2c Master Read return HB value Threshold Register ::%d",ReadByte.byte.HB);
	
	if( WriteByte.byte.LB == ReadByte.byte.LB)
	{
		printf("\r\nIC threshold value is ::%d",ReadByte.byte.LB);
		printf("\r\nThreshold is ok");
	}
	else
	{
		printf("\r\nIC threshold value is ::%d",ReadByte.byte.LB);
		printf("\r\nThreshold is not ok");
	}
}

/*******************************************************************************
 * Function Name :					Read_SOC_Reg_and_CompareThreshold
 * Function Description : 			Reads SOC register & compares with 
 * 									Threshold being set based on macro 
 * 									either 10% or 32% clearing alert flag
 * *****************************************************************************/
void Read_SOC_Reg_and_CompareThreshold(void)
{
	printf("\r\n\r\nReading SOC");
	
	ReadOnI2C(SOC_MSB);
	printf("\r\ni2c Master Read LB return value SOC Register ::%d",ReadByte.byte.LB);
	printf("\r\ni2c Master Read HB return value SOC Register ::%d",ReadByte.byte.HB);	

	#ifdef THRESHOLD_10
		if(ReadByte.byte.HB  >= TH10_VALUE)
	#endif
	#ifdef THRESHOLD_32
		if(ReadByte.byte.HB >= TH32_VALUE)
	#endif
	{
		if(Battery_Reset_Flag == 1)
		{
			Clear_BatteryFlag();
		}
		Battery_Reset_Flag = 0;
	}	
	else
	{
		Battery_Reset_Flag = 1;
	}
	
	DelayMs(300);
}

/*******************************************************************************
 * Function Name :					Read_Vcell
 * Function Description : 			Read the battery voltage 
 * *****************************************************************************/

void Read_Vcell(void)
{
	Vbat.Val = 0;	
	
	printf("\r\n\r\nReading SOC");
	ReadOnI2C(SOC_MSB);
	printf("\r\ni2c Master Read LB return value SOC Register ::%d",ReadByte.byte.LB);
	printf("\r\ni2c Master Read HB return value SOC Register ::%d",ReadByte.byte.HB);
		
	if(Reset_MAX17043_IC_Counter > 1) 
	{
		printf("\r\nAlert BIT is cleared");
		/** Reset the Fuel guage IC */
		ResetFuelGauge();
		/** Set the threshold value */
		SetThreshold();		
		Reset_MAX17043_IC_Counter = 0;
	}	
	Reset_MAX17043_IC_Counter++;
	printf("\r\n\r\nReading VCELL");
	ReadOnI2C(VCELL_MSB);
	printf("\r\ni2c Master Read LB return value Vcell Register ::%d",ReadByte.byte.LB);
	printf("\r\ni2c Master Read HB return value Vcell Register ::%d",ReadByte.byte.HB);

	Vbat.byte.LB = ReadByte.byte.LB;
	Vbat.byte.HB = ReadByte.byte.HB;
	Vbat.Val = ((Vbat.byte.LB|(Vbat.byte.HB << 8)) >> 4);
	Battery_Vout_inMv = 1.25* Vbat.Val;

	if(Battery_Vout_inMv <= 3300)
	{
		//Read_SOC_Reg_and_CompareThreshold();
	}	
	else if(Battery_Vout_inMv > 4100)
	{		
		//Battery_Vout_inMv = 0;
	}

	printf("\r\nBattery voltage ====== %d",Battery_Vout_inMv);
	printf("\r\n");
	DelayMs(60);
}

/*******************************************************************************
 * Function Name :				Clear_BatteryFlag			 
 * Function Description : 		Clear the battery flag
 * *****************************************************************************/
void Clear_BatteryFlag(void)
{
	printf("\r\nBattery Alert Flag *********************************\r\n");
	WriteByte.byte.LB = ReadByte.byte.LB;
	WriteByte.byte.LB = (WriteByte.byte.LB & 0x1F);
	
	WriteByte.byte.HB = 0x97;
	printf("\r\ni2c Master Write LB value Threshold Register ::%d",WriteByte.byte.LB);
	printf("\r\ni2c Master Write HB value Threshold Register ::%d",WriteByte.byte.HB);
		
	WriteOnI2C(CONFIG_MSB,WriteByte.byte.HB,WriteByte.byte.LB);	
	Battery_Low_Alert_Flag = 0;	
}

/*******************************************************************************
 * Function Name :			BatAlert_Config		 
 * Function Description : 
 * *****************************************************************************/
void BatAlert_Config(void)
{
	//Charge Status pin config
	GPIO_PERIPHERAL_CONTROL(GPIO_PORT_TO_BASE(GPIO_B_NUM), GPIO_PIN_MASK(CHARGE_PIN));
	GPIO_SET_INPUT(GPIO_PORT_TO_BASE(GPIO_B_NUM), GPIO_PIN_MASK(CHARGE_PIN));
	ioc_set_over(GPIO_B_NUM, CHARGE_PIN, IOC_OVERRIDE_DIS);
	
	// Set the port B pin 1 as interrrupt pin 
	//Alert pin config
	GPIO_SOFTWARE_CONTROL(GPIO_PORT_TO_BASE(GPIO_B_NUM), GPIO_PIN_MASK(ALERT_PIN)); //set GPIO mode
	GPIO_SET_INPUT(GPIO_PORT_TO_BASE(GPIO_B_NUM), GPIO_PIN_MASK(ALERT_PIN));
	ioc_set_over(GPIO_B_NUM, ALERT_PIN, IOC_OVERRIDE_PUE);
	
	//Alert Interrupt config
	GPIO_DISABLE_INTERRUPT(GPIO_PORT_TO_BASE(GPIO_B_NUM), GPIO_PIN_MASK(ALERT_PIN));
	GPIO_CLEAR_INTERRUPT(GPIO_PORT_TO_BASE(GPIO_B_NUM), GPIO_PIN_MASK(ALERT_PIN));
	gpio_register_callback(GPIOPortB_InterruptCallback, GPIO_B_NUM, ALERT_PIN);
	
	GPIO_DETECT_FALLING(GPIO_PORT_TO_BASE(GPIO_B_NUM), GPIO_PIN_MASK(ALERT_PIN));
	GPIO_ENABLE_INTERRUPT(GPIO_PORT_TO_BASE(GPIO_B_NUM), GPIO_PIN_MASK(ALERT_PIN));
}

/*******************************************************************************
 * Function Name :			GPIOPortB_InterruptCallback			 
  * *****************************************************************************/
void GPIOPortB_InterruptCallback(unsigned char port, unsigned char pin)
{
	if ((port == GPIO_B_NUM) && (pin == ALERT_PIN))
	{ 
		GPIO_CLEAR_INTERRUPT(GPIO_PORT_TO_BASE(GPIO_B_NUM), GPIO_PIN_MASK(ALERT_PIN));
		Battery_Low_Alert_Flag = 1;
		printf("\r\nReceived Alert signal From the MAX17043################################\r\n");
	}
}

/*******************************************************************************
 * Function Name :			DelayMs			 
 * *****************************************************************************/
void DelayMs(unsigned int msec)
{
	unsigned int i = 0;	
	for( i = 0; i < msec; i++)
	{
		delayus();	
	}
}

/*******************************************************************************
 * Function Name :			delayus	 
 * *****************************************************************************/
void delayus(void)
{
	unsigned int i = 0;	
	for( i = 0; i < 250; i++)
	{
		Delay_4us();
	}
}

/*******************************************************************************
 * Function Name :					QuiuckStart
 * Function Description : 			?????????????????
 * *****************************************************************************/
void QuiuckStart(void)
{
	WriteOnI2C(MODE_MSB,0x40,0x00);
}

/*******************************************************************************
 * Function Name :					WriteOnI2C
 * Function Description : 			Write Data on I2C bus
 * *****************************************************************************/
unsigned int WriteOnI2C(unsigned char RegAddr , unsigned char Data_0,unsigned char Data_1)
{
	unsigned char I2CData[3];
	unsigned char Index_u8 = 0;

	I2CData[0] = Data_0;
	I2CData[1] = Data_1;
	i2c_master_set_slave_address((MAX_I2C_WRITE_ADDR), I2C_SEND); 		//write data	
	i2c_master_data_put(RegAddr);
	i2c_master_command(I2C_MASTER_CMD_BURST_SEND_START);				// Intiate Start Write for more than 1 byte(burst)	
	while(i2c_master_busy());

	for(Index_u8 = 0; Index_u8 < NUM_I2C_DATA; Index_u8++)
	{
		i2c_master_data_put(I2CData[Index_u8]);		
		if(Index_u8 == (NUM_I2C_DATA-1))
		{
			i2c_master_command(I2C_MASTER_CMD_BURST_SEND_FINISH);		//Stop
		}
		else
		{
			i2c_master_command(I2C_MASTER_CMD_BURST_SEND_CONT);			// Continue after writing 1 byte	
		}		
		while(i2c_master_busy());
	}
	while(i2c_master_busy());
	return i2c_master_error();
}

/*******************************************************************************
 * Function Name :					ReadOnI2C
 * Function Description : 			Read Data from I2C bus
 * *****************************************************************************/
unsigned char ReadOnI2C(unsigned char RegAddress)
{
	//reset the values
	ReadByte.byte.LB  = 0;
	ReadByte.byte.HB  = 0;
	
	i2c_master_set_slave_address((MAX_I2C_WRITE_ADDR), I2C_SEND);
	i2c_master_data_put(RegAddress);
	i2c_master_command(I2C_MASTER_CMD_BURST_SEND_START);
	while(i2c_master_busy());
	
	i2c_master_set_slave_address((MAX_I2C_WRITE_ADDR), I2C_RECEIVE);
	
	i2c_master_command(I2C_MASTER_CMD_BURST_RECEIVE_START);
	while(i2c_master_busy());	
	ReadByte.byte.HB  = i2c_master_data_get();
	
	i2c_master_command(I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(i2c_master_busy());
	ReadByte.byte.LB    = i2c_master_data_get();
	
 	return ReadByte.byte.HB;
}

/*******************************************************************************
 * Function Name :			CheckChargerConnectionStatus		 
 * Function Description :   HW dependence + issues
 * *****************************************************************************/
void CheckChargerConnectionStatus(void)
{
	unsigned char res = 0xFF; 
		
	res = GPIO_READ_PIN(GPIO_PORT_TO_BASE(GPIO_B_NUM), GPIO_PIN_MASK(CHARGE_PIN));
	printf("\r\n PB0 value is ::: %d",res);

	if(res & 0xFF)
	{
		Charger_Connection_Status = 1;
		printf("\r\n *****************************************************");
		printf("\r\n Charger Connected.....");
		printf("\r\n *****************************************************");		
	}	
	else
	{
		Charger_Connection_Status = 0;
		printf("\r\n *****************************************************");		
		printf("\r\n Charger Not Connected.......");
		printf("\r\n *****************************************************");		
	}
}


