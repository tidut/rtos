/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-Help.c
 * Description 	:		
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
#include "dev/GenericTypeDefs.h"

_FLAGS FLAGS;

/************************************* res_Help1_get_handler ********************************
***********************************************************************************************************
***********************************************************************************************************/

static void res_Help1_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	unsigned char Helpbuffer[400];
	memset(Helpbuffer,0,sizeof(Helpbuffer));

	strcat((char*)Helpbuffer,"\r\nSyntax to set a filter service or Data function modifier Service");
	strcat((char*)Helpbuffer,"\r\n<Filter Head><User expression><Remapable variable(s)>");
	strcat((char*)Helpbuffer,"\r\n    Eg: PotFS=(>=0.5&<=3.3)::P");
	
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, Helpbuffer,strlen(Helpbuffer));
}
EVENT_RESOURCE(res_Help1,"title=\"Help1\";obs",res_Help1_get_handler,NULL,NULL,NULL,NULL);


/************************************* res_Help2_get_handler ********************************
***********************************************************************************************************
***********************************************************************************************************/

static void res_Help2_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	unsigned char Helpbuffer[400];

	memset(Helpbuffer,0,sizeof(Helpbuffer));

	strcat((char*)Helpbuffer,"\r\nFilter header for POT FS is :: PotFS=");
	strcat((char*)Helpbuffer,"\r\nRemapable variable for POT FS is :: P");
	
	strcat((char*)Helpbuffer,"\r\nFilter header for Temperature FS is :: TempFS=");
	strcat((char*)Helpbuffer,"\r\nRemapable variable for Temperature FS is :: T");
	
	strcat((char*)Helpbuffer,"\r\nFilter header for Pressure FS is :: CurFS=");
	strcat((char*)Helpbuffer,"\r\nRemapable variable for Pressure FS is :: C");
	
	strcat((char*)Helpbuffer,"\r\nFilter header for Temperature DFMS is :: DFMS=");
	strcat((char*)Helpbuffer,"\r\nRemapable variable for Temperature DFMS is :: T");	
	
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, Helpbuffer,strlen(Helpbuffer));
}
EVENT_RESOURCE(res_Help2,"title=\"Help2\";obs",res_Help2_get_handler,NULL,NULL,NULL,NULL);


