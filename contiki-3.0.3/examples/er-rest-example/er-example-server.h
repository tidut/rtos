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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "contiki.h"
#include "er-coap.h"
#include "diag_server.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "dev/als-sensor.h"
#include "dev/board.h"
#include "spi-arch.h"
#include "dev/spi.h"
#include "MDB.h"

/* Files reqired for Diagnostic server*/
#include "Max17043.h"
#include "dev/i2c.h"
#include "diag_server.h"
/**********************************/

#include "sys/etimer.h"
#include "sys/rtimer.h"
#include "dev/leds.h"
#include "dev/uart.h"

#include "sys/ctimer.h"
#include "sys/stimer.h"
#include "sys/clock.h"
#include "compile_time.h"
#include "version.h"


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

#define TOGGLE_INTERVAL 	15
#define SPI_CONF_DEFAULT_INSTANCE 1

/*
* Resources to be activated need to be imported through the extern keyword.
* The build system automatically compiles the resources in the corresponding sub-directory.
*/

extern resource_t res_chunks;
extern resource_t res_pot1;
extern resource_t res_pot2;
extern resource_t res_lm61;
extern resource_t res_UART;
extern resource_t res_DFMS;
extern resource_t res_FS;
extern resource_t res_current;
extern resource_t res_diag;
extern resource_t res_Help1;
extern resource_t res_Help2;

extern resource_t res_relay_1_On;
extern resource_t res_relay_1_Off;
extern resource_t res_relay_2_On;
extern resource_t res_relay_2_Off;

extern resource_t res_volt_energy;
extern resource_t res_curr_energy;
extern resource_t res_pf_energy;
extern resource_t res_freq_energy;
extern resource_t res_wph_energy;

extern resource_t res_Air_Quality;
extern resource_t res_thermocouple;

#if PLATFORM_HAS_LEDS
	extern resource_t res_leds;
	extern resource_t res_toggle;
	extern resource_t res_toggle_led1;
	extern resource_t res_toggle_led2;
#endif

extern void Max17043_I2c_Init(void);
extern void Calculate_Current(void);
extern void Calculate_AC_Energy(void);
extern void Calculate_DC_Energy(void);
extern void Calculate_POT_1_Value(void);
extern void Calculate_POT_2_Value(void);
extern void Calculate_Temprature_Value(void);


enum ENERGY_COMMAND
{
	MDB_VOLTAGE_R_ADDRESS_WRITE, 	
	MDB_VOLTAGE_R_ADDRESS_READ,
	MDB_CURRENT_R_ADDRESS_WRITE, 	
	MDB_CURRENT_R_ADDRESS_READ, 	
	MDB_PF_ADDRESS_WRITE, 					
	MDB_PF_ADDRESS_READ, 					
	MDB_FREQUENCY_ADDRESS_WRITE, 	
	MDB_FREQUENCY_ADDRESS_READ, 	
	MDB_WATTS_PER_HR_ADDRESS_WRITE,
	MDB_WATTS_PER_HR_ADDRESS_READ
};

#define SLAVE_ADDRESS_ENERGY		0X01
#define VOLTAGE_R_ADDRESS 			0x00003//0x0143
#define CURRENT_R_ADDRESS 			0x0151
#define PF_ADDRESS 					0x0117
#define FREQUENCY_ADDRESS 			0x0157
#define WATTS_PER_HR_ADDRESS 		0x0159

void Add_Sensors(void);

extern void Init_I2c(void);
extern void Call_Current_Sensor(void);
extern void Call_Pot_Temp_Sensor(void);
extern void Call_Energy_Sensor(void);
extern void Call_Air_Sensor(void);
extern void Call_Relays_Sensor(void);
extern void Call_Thermocouple_Sensor(void);

extern void Do_Mdb_VOLTAGE_R_ADDRESS(void);
extern void Do_Mdb_CURRENT_R_ADDRESS(void);	
extern void Do_Mdb_PF_ADDRESS(void);	 						
extern void Do_Mdb_FREQUENCY_ADDRESS(void);	 			
extern void Do_Mdb_WATTS_PER_HR_ADDRESS(void);	 	

