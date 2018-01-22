#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "uart.h"
#include "common.h"
#include "contiki.h"
#include "contiki.h"
#include "er-coap.h"
#include "contiki-net.h"
#include "diag_server.h"
#include "rest-engine.h"
#include "dev/als-sensor.h"
#include "er-coap-engine.h"
#include "dev/GenericTypeDefs.h"

/* Files reqired for Diagnostic server*/

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

// its normal buffer used for stack implementation
float stack[64];
int top = -1;

unsigned char PushIndex;

/*******************************************************************************
 * Function Name :					Convert_Ascii_to_Float_push
 * Function Description : 			This function look for paranthesis in string
 * *****************************************************************************/
int Check_For_Paranthesis(char *string)
{
    uint16_t Index_u8 = 0;
    uint16_t Left_pars_Count = 0,Right_pars_Count = 0;

	// "(V*I)::V,I"
    for( Index_u8 = 0; Index_u8 < strlen(string);Index_u8++)
    {
        if(string[Index_u8]=='('||string[Index_u8]=='['||string[Index_u8]=='{')
        {
            Left_pars_Count++;
        }
        if(string[Index_u8]==')'||string[Index_u8]==']'||string[Index_u8]=='}')
        {
            Right_pars_Count++;
        }
    }

    if( Left_pars_Count  == Right_pars_Count)
    {
        printf("\r\n Valid expresssion............................");
        return 1;
    }
    else
    {
        printf("\r\n Not a valid expression.......................");
         return 0;
    }
}

/*******************************************************************************
 * Function Name :					Convert_Ascii_to_Float_push
 * Function Description : 			This function convert ascii to 
 * 									float value will push to stack buffer
 * *****************************************************************************/
void Convert_Ascii_to_Float_push(unsigned char *str)
{
    unsigned char tmpbuf[20], Index_u8 = 0,digitCount = 1;
    float temp = 0;
    unsigned char ValisSigned = 0;

    if(*str == '-')
    {
        ValisSigned = 1;
        str++;
        PushIndex++; //To avoid executing data push for loop 1st time
    }

    memset(tmpbuf,0,sizeof(tmpbuf));
    tmpbuf[Index_u8++] = *str;
    str++;

    while((*str >= 0x30 && *str <= 0x39) || (*str == '.')|| (*str== '-'))
    {    
        if(*str == '-')
        {
            ValisSigned = 1;
            str++;
            PushIndex++; //To avoid executing  for loop 1st time
        }        
        tmpbuf[Index_u8++] = *str;
        str++;
        digitCount++;
        PushIndex++; //To avoid executing  data push for loop digitCount times
    }

	// if it only single digit then make 5 to 5.0
    if(digitCount == 1)
    {
        tmpbuf[Index_u8++] = '.';
        tmpbuf[Index_u8++] = '0';
        temp = Convert_String_Float((unsigned char *)tmpbuf,temp);
        
        if(ValisSigned)
        {
           temp = - temp;
           ValisSigned = 0;
        }
        Push(temp);            
        printf("\r\n %d is Pushed to Stack....................",temp);
    }
    else
    {
        temp = Convert_String_Float((unsigned char  *)tmpbuf,temp);        
        if(ValisSigned)
        {
           temp = - temp;
           ValisSigned = 0;
        }
        Push(temp);
        printf("\r\n %d is Pushed to Stack....................",temp);
    }    
    printf("\r\n %s is Pushed to Stack....................",tmpbuf);
}

/*******************************************************************************
 * Function Name :					Convert_String_Float
 * Function Description : 			This function convert ascii to 
 * 									float value 
 * *****************************************************************************/
float Convert_String_Float(unsigned char *str,float resu)
{
    unsigned char len = 0,n = 0;
    unsigned char dotpos = 0;

    len = strlen(str);
    for (n = 0; n < len; n++)
    {
        if (str[n] == '.')
        {
            dotpos = len - n  - 1;
        }
        else
        {
            resu = resu * 10.0f + (str[n]-'0');
        }
    }
    while ( dotpos--)
    {
        resu /= 10.0f;
    }
    return resu;
}

/*******************************************************************************
 * Function Name :					strrev
 * Function Description : 			reverse the string
 * *****************************************************************************/
char *strrev(char *string)
{
    unsigned char len = 0 ;
    unsigned char Index_u8 = 0;
    unsigned char str_index_u8 = 0;
    unsigned char strrev_result[20];
 
	len = strlen(string);

    for( Index_u8 = len -1; Index_u8 != 0; str_index_u8++,Index_u8--)
    {
        strrev_result[str_index_u8] = string[Index_u8];        
    }
    strrev_result[str_index_u8] = string[Index_u8];
    
    printf("\r\n Reverse string ---->>> %s ",strrev_result);
    return strrev_result;
}

/*******************************************************************************
 * Function Name :					Push
 * Function Description : 			Push that Float value to stack
 * *****************************************************************************/
void Push(float c)
{
	// here top is stack it starts with -1 once data getting filled in buffer then it will start incrementing
    if(top == sizeof(stack))
    {
        printf("\r\n stack is full..................");
        return;
    }
    stack[++top] = c; 
    printf("\r\n stack PUSH Index %d ",top);
  
}


/*******************************************************************************
 * Function Name :					Pop
 * Function Description : 			Pop that Float value from stack
 * *****************************************************************************/

float Pop(void)
{
	// here top is stack it starts with -1 once data getting removed from buffer then it will start decrementing
    if(top == -1)
    {
        printf("\r\n stack is empty....................");
        return;
    }
    printf("\r\n stack POP Index %d ",top);
    return stack[top--];
}

/*******************************************************************************
 * Function Name :					Convert_Integer_To_String 
 * Function Description : 			Convert integer value to string
 * *****************************************************************************/
char *Convert_Integer_To_String(int value)
{
    static char buffer[12];        	// 12 bytes is big enough for an INT32
    int original = value;        	// save original value

    int c = sizeof(buffer)-1;

    buffer[c] = 0;                	// write trailing null in last byte of buffer
    if (value < 0)                 	// if it's negative, note that and take the absolute value
    value = -value;

    do                             // write least significant digit of value that's left
    {
		buffer[--c] = (value % 10) + '0';
		value /= 10;
    }while (value);

    if (original < 0)
		buffer[--c] = '-';
		
    return &buffer[c];
}

/*******************************************************************************
 * Function Name :					Get_IPV6_addresses 
 * Function Description : 			Print the IPV6 address
 * *****************************************************************************/
void Get_IPV6_addresses(void)
{
	int Index_u8 , j;
	unsigned char state;

	PRINTF("\r\n node addresses: ");
	
	for(Index_u8 = 0; Index_u8 < UIP_DS6_ADDR_NB - 1; Index_u8++) 
	{
		state = uip_ds6_if.addr_list[Index_u8].state;
		if(state == ADDR_PREFERRED) 
		{
			PRINT6ADDR(&uip_ds6_if.addr_list[Index_u8].ipaddr);
			memcpy(dpars.Ipv6address,&uip_ds6_if.addr_list[Index_u8].ipaddr,16);								
			PRINTF("\n");
			/* hack to make address "final" */			
		}
	}

	printf("\r\nIpv6 address of the  node is ::");
	for(Index_u8 = 0; Index_u8 < 16; Index_u8++) 
	{
		printf("%02x:", dpars.Ipv6address[Index_u8]);
	}
}
