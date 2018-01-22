/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-toggle.c
 * Description 	:		
 * Written by 	: 
 * *******************************************************
 * *******************************************************/

#include "contiki.h"

#if PLATFORM_HAS_LEDS

#include <string.h>
#include "contiki.h"
#include "rest-engine.h"
#include "dev/leds.h"


static void res_post_Yledhandler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_Pledhandler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset);


/***********Added by Krishna**********************************/
RESOURCE(
			res_toggle_led2,
			"title=\"Orange LED\";rt=\"Control\"",
			NULL,
			res_post_Yledhandler,
			NULL,
			NULL
		);
         
static void res_post_Yledhandler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
  leds_toggle(LEDS_YELLOW);
}


/***********Added by Gowrish**********************************/
RESOURCE(
			res_toggle_led1,
			"title=\"PC1 LED\";rt=\"Control\"",
			NULL,
			res_post_Pledhandler,
			NULL,
			NULL
		);
         
static void res_post_Pledhandler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
  leds_toggle(LEDS_RED);
}

#endif /* PLATFORM_HAS_LEDS */
