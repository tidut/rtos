/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-Relay.c
 * Description 	:	Relays
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


void Call_Relays_Sensor(void)
{
	/*
	if(Relay)
	{
		RELAY_1_ON();
		Relay = 0;
	}
	else
	{
		RELAY_1_OFF();
		Relay = 1;
	}
	*/
}
static void res_post_relay_1_off_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	RELAY_1_OFF();
	RELAY_2_OFF();
}

static void res_post_relay_1_on_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	RELAY_1_ON();
	RELAY_2_ON();
}

static void res_post_relay_2_off_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	RELAY_1_OFF();
	RELAY_2_OFF();
}

static void res_post_relay_2_on_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	RELAY_1_ON();
	RELAY_2_ON();
}

RESOURCE(
			res_relay_1_On,
			"title=\"Relay 1 on\";rt=\"Control\"",
			NULL,
			res_post_relay_1_on_handler,
			NULL,
			NULL
			);
		
RESOURCE(
			res_relay_1_Off,
			"title=\"Relay 1 off\";rt=\"Control\"",
			NULL,
			res_post_relay_1_off_handler,
			NULL,
			NULL
			);
         
RESOURCE(
			res_relay_2_On,
			"title=\"Relay 2 on\";rt=\"Control\"",
			NULL,
			res_post_relay_2_on_handler,
			NULL,
			NULL
			);
		
RESOURCE(
			res_relay_2_Off,
			"title=\"Relay 2 off\";rt=\"Control\"",
			NULL,
			res_post_relay_2_off_handler,
			NULL,
			NULL
			);

/***************************************************END******************************************/



