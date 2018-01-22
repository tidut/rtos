/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-diagnostics.c
 * Description 	:	Used to display Battery Voltage 
 * 					Battery charger status
 * 					IPV6 address 
 * 					MAC address
 * 					No of sensors connected 
 * 					Shows sensor health 					
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
#include "er-example-server.h"

#if CONTIKI_TARGET_NATIVE
	#include "slip-config.h"
	#include "sys/stat.h"
	#include "native-rdc.h"
#endif

#if CETIC_CSMA_STATS
	#include "csma.h"
#endif

#if CETIC_NODE_CONFIG
	#include "node-config.h"
#endif

#include "net/ip/uip.h"
#include "contiki-lib.h"
#include "net/rpl/rpl.h"
#include "net/netstack.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6.h"
#include "net/rpl/rpl-private.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip-ds6-route.h"

/****************************************/
void AddTheDiagFrame(void);
UINT16_VAL Hex2Ascii_Frame (unsigned char DataByte);
unsigned char Hex2Ascii (unsigned char DataByte);

unsigned char Diagbuffer[350];
unsigned char tempDiagbuffer[60];

_FLAGS FLAGS;

extern unsigned int Battery_Vout_inMv;
extern volatile unsigned char Charger_Connection_Status;
extern volatile unsigned char Battery_Low_Alert_Flag;
extern struct DiagnosticParameters dpars;

/*********************************************************************8*****************************/

static void res_diag_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	int rssi = 0;
	unsigned char Index_u8;
	unsigned char temp_Index_u8,k,l;
		
	UINT16_VAL cRes;

	memset(Diagbuffer,0,sizeof(Diagbuffer));
	memset(tempDiagbuffer,0,sizeof(tempDiagbuffer));
	memset(dpars.Macid,0,sizeof(dpars.Macid));
	ieee_addr_cpy_to(&dpars.Macid[0], 8);
		
	strcat((char *)Diagbuffer,"\r\nMAC ID of the node is ::");
		
	printf("\r\nMAC ID of the node is ::");
	
	// copy the MAC ID to Diag buffer
	for(Index_u8 = 0, temp_Index_u8 = 0; Index_u8 < 8; Index_u8++) 
	{
		printf("%02x:", dpars.Macid[Index_u8]);		
		cRes = Hex2Ascii_Frame(dpars.Macid[Index_u8]);
		tempDiagbuffer[temp_Index_u8] =  cRes.byte.HB;
		temp_Index_u8++;
		tempDiagbuffer[temp_Index_u8] =  cRes.byte.LB;
		temp_Index_u8++;
		
		if(Index_u8 != 7)
		{
			tempDiagbuffer[temp_Index_u8] = ':';
			temp_Index_u8++;
		}	
	}
		
	strcat((char *)Diagbuffer,tempDiagbuffer);	
	memset(tempDiagbuffer,0,sizeof(tempDiagbuffer));
	memset(dpars.Ipv6address,0,sizeof(dpars.Ipv6address));
	Get_IPV6_addresses();
	strcat((char *)Diagbuffer,"\r\nIpv6 address of the  node is ::[");
	
	for(Index_u8 = 0,temp_Index_u8=0,k=0,l = 0; Index_u8 < 16; Index_u8++) 
	{
		cRes = Hex2Ascii_Frame(dpars.Ipv6address[Index_u8]);
		tempDiagbuffer[temp_Index_u8] =  cRes.byte.HB;
		tempDiagbuffer[temp_Index_u8+1] =  cRes.byte.LB;
		l++;
		
		if(l>=2)
		{
			l = 0;
			if(Index_u8 != 15)
			{
				tempDiagbuffer[temp_Index_u8+2] = ':';
				k= k+1;
			}
			else
			{
				tempDiagbuffer[temp_Index_u8+2] = ']';
			}
		}		
		temp_Index_u8= k+2;
		k+=2;
	}
	strcat((char *)Diagbuffer,tempDiagbuffer);
	
	#if 0 
	// commented becuase of 3.0
	// received signal strength indication added in disg buffer
	rssi = cc2538_rf_read_rssi();
	strcat((char *)Diagbuffer,"\r\nRSSI in dbs ::");
	strcat((char *)Diagbuffer,Convert_Integer_To_String(rssi));
	printf("\r\nRSSI in dbs :: %d",rssi);
	#endif
	
	AddTheDiagFrame();
	
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, Diagbuffer,strlen(Diagbuffer));
}
EVENT_RESOURCE(res_diag,"title=\"Diagnostics\";obs",res_diag_get_handler,NULL,NULL,NULL,NULL);


void AddTheDiagFrame(void)
{
	unsigned char NoofSensors = 0;
	
	#ifdef EM_DIAGNOSTICS
		strcat((char *)(char *)Diagbuffer,"\r\nEnergy Meter Health ::");
		
		if(FLAGS.MBResponseRxd)
		{			
			strcat((char *)(char *)Diagbuffer,"GOOD");
		}
		else
		{
			strcat((char *)Diagbuffer,"BAD");		
		}
	#endif
	
	#ifdef TS_DIAGNOSTICS
		strcat((char *)Diagbuffer,"\r\nTemperature Sensor Health ::");
		if(!FLAGS.TS_Result_outofrange)
		{		
			strcat((char *)Diagbuffer,"GOOD");
		}
		else
		{
			FLAGS.TS_Result_outofrange = 0;
			strcat((char *)Diagbuffer,"BAD");		
		}
	#endif
			
	#ifdef PS_DIAGNOSTICS
		strcat((char *)Diagbuffer,"\r\nSensor Health ::");
		if(FLAGS.PS_Result_outofrange)
		{			
			strcat((char *)Diagbuffer,"GOOD");
		}
		else
		{
			strcat((char *)Diagbuffer,"BAD");	
		}
	#endif
	

	#ifdef TEMP_P1_P2	
		strcat((char *)Diagbuffer,"\r\nNumber of sensors connected is ::");
		NoofSensors = NOOF_SENSORS_CONNECTED;
		strcat((char *)Diagbuffer,Convert_Integer_To_String(NoofSensors));
		strcat((char *)Diagbuffer,"\r\nBattery Voltage ::");
		strcat((char *)Diagbuffer,Convert_Integer_To_String(Battery_Vout_inMv));
		strcat((char *)Diagbuffer," mV");
	#endif
	
	#if 0
		if(Battery_Low_Alert_Flag)
		{
			strcat((char *)Diagbuffer,"\r\nBattery Low...");
		}
		
		strcat((char *)Diagbuffer,"\r\nCharger Connected ::");
		if(Charger_Connection_Status)
		{
			strcat((char *)Diagbuffer,"Yes");
		}
		else
		{
			strcat((char *)Diagbuffer,"No");
		}
	#endif
}


/*******************************************************************************
 * Function Name :				Hex2Ascii_Frame					 
 * Function Description :		Get the Ascii data 
 * *****************************************************************************/
UINT16_VAL Hex2Ascii_Frame (unsigned char DataByte)
{
    unsigned char Temp_MSB;
    UINT16_VAL AsciBytes;

    Temp_MSB = (DataByte & 0xF0);
    Temp_MSB = Temp_MSB >>4;
    AsciBytes.byte.HB = Hex2Ascii(Temp_MSB);
    AsciBytes.byte.LB = Hex2Ascii(DataByte & 0x0F);
    return AsciBytes;
}


/*******************************************************************************
 * Function Name :				Hex2Ascii					 
 * Function Description :		Converts Hex to Ascii
 * *****************************************************************************/
unsigned char Hex2Ascii (unsigned char DataByte)
{
    unsigned char Temp_DB_LSB	;
    unsigned char ch = 0;

    Temp_DB_LSB = DataByte ;

    switch ( Temp_DB_LSB )
    {
        case 0x00:
            ch = '0';
            break;

        case 0x01:
            ch = '1';
            break;

        case 0x02:
            ch = '2';
            break;

        case 0x03:
            ch = '3';
            break;

        case 0x04:
            ch = '4';
            break;

        case 0x05:
            ch = '5';
            break;

        case 0x06:
            ch = '6';
            break;

        case 0x07:
            ch = '7';
            break;

        case 0x08:
            ch = '8';
            break;

        case 0x09:
            ch = '9';
            break;

        case 0x0A:
            ch = 'A';
            break;

        case 0x0B:
            ch = 'B';
            break;

        case 0x0C:
            ch = 'C';
            break;

        case 0x0D:
            ch = 'D';
            break;

        case 0x0E:
            ch = 'E';
            break;

        case 0x0F:
            ch = 'F';
            break;
    }
    return ch;
}
