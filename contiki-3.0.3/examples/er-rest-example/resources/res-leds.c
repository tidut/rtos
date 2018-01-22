/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-leds.c
 * Description 	:		
 * Written by 	: 
 * *******************************************************
 * *******************************************************/

#include "contiki.h"

#if PLATFORM_HAS_LEDS

#include <string.h>
#include "rest-engine.h"
#include "dev/leds.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((unsigned char *)addr)[0], ((unsigned char *)addr)[1], ((unsigned char *)addr)[2], ((unsigned char *)addr)[3], ((unsigned char *)addr)[4], ((unsigned char *)addr)[5], ((unsigned char *)addr)[6], ((unsigned char *)addr)[7], ((unsigned char *)addr)[8], ((unsigned char *)addr)[9], ((unsigned char *)addr)[10], ((unsigned char *)addr)[11], ((unsigned char *)addr)[12], ((unsigned char *)addr)[13], ((unsigned char *)addr)[14], ((unsigned char *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

static void res_post_put_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset);

/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
RESOURCE(
			res_leds,
			"title=\"LEDs: ?color=r|g|b, POST/PUT mode=on|off\";rt=\"Control\"",
			NULL,
			res_post_put_handler,
			res_post_put_handler,
			NULL
	);

static void res_post_put_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
  const char *color = NULL;
  const char *mode = NULL;
  
  size_t len = 0;
  unsigned char led = 0;
  int success = 1;
  
  if((len = REST.get_query_variable(request, "color", &color))) 
  {
    PRINTF("color %.*s\n", len, color);

    if(strncmp(color, "r", len) == 0) 
    {
      led = LEDS_RED;
    } 
    else if(strncmp(color, "g", len) == 0) 
    {
      led = LEDS_GREEN;
    } 
    else if(strncmp(color, "b", len) == 0) 
    {
      led = LEDS_BLUE;
    } 
    else 
    {
      success = 0;
    }
    PRINTF("---led=%d len=%d success=%d", led, len, success);
  } 
  else 
  {
    success = 0;
  } 
  
  if(success && (len = REST.get_post_variable(request, "mode", &mode))) 
  {
    PRINTF("mode %s %d\n", mode, len);

    if(strncmp(mode, "on", len) == 0) 
    {
    	PRINTF("---led %s\n", mode);
		leds_on(led);
    } 
    else if(strncmp(mode, "off", len) == 0) 
    {
		leds_off(led);
      	PRINTF("---led %s\n", mode);
    } 
    else 
    {
		success = 0;
	}
  } 
  else 
  {
    success = 0;
  } 
  
  if(!success) 
  {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}
#endif /* PLATFORM_HAS_LEDS */
