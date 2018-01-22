/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-FS.c
 * Description 	:	Filter services has been done for 
 * 					POT	
 * 					Tempratrure
 * 					Current (4to20)
 * Written by 	: 
 * *******************************************************
 * *******************************************************/
 
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "common.h"
#include "res-FS.h"
#include "contiki.h"
#include "contiki.h"
#include "er-coap.h"
#include "diag_server.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "dev/als-sensor.h"
#include "er-coap-engine.h"
#include "dev/GenericTypeDefs.h"

#define DEBUG

extern int top;
extern unsigned char PushIndex;

extern float stack[64];
double PotPop_value[20];
double CurPop_value[20];
double TempPop_value[20];

unsigned char PotRemap_vars[10];
unsigned char CurRemap_vars[10];
unsigned char TempRemap_vars[10];

volatile float cSensorParameter = 0;

unsigned char PostAck[32];
unsigned char PotPop_Symbols[20];
unsigned char CurPop_Symbols[20];
unsigned char TempPop_Symbols[20];

unsigned char PotFS_buffer[100];
unsigned char CurFS_buffer[100];
unsigned char TempFS_buffer[100];

unsigned char Pot_Fs_RXD_u8 = 0;
unsigned char Temp_Fs_RXD_u8 = 0;
unsigned char Current_Fs_RXD_u8 = 0;

extern unsigned int Current_Value_u32;
extern volatile float POT_1_Voltage;
extern volatile float POT_2_Voltage;
extern unsigned int Temp_Lm61_u32;

/******************************* Filter service POST and GET  handlers  *******************************
***********************************************************************************************************
***********************************************************************************************************/

static void res_FS_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	//Copy global buffer content to payload buffer
	strcpy((unsigned char *)buffer,PotFS_buffer);
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, buffer,strlen(buffer));
}

static void res_FS_post_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	char *ptr;
	size_t len = 0;
	unsigned char Index_u8;
	unsigned char Exp_Result = 0;
	unsigned char *incoming = NULL;

	printf("\r\nExecuting FS POST handler.........................................");
	memset(PostAck,0,sizeof(PostAck));	
	printf("\r\nData received:: %s",request);

	/* Check POST handler for Potentiometer */
	if(len = REST.get_post_variable(request, "PotFS", &incoming))
	{
    	printf("incoming data:: %s %d\n",incoming, len);
		memset(PotFS_buffer,0,sizeof(PotFS_buffer));
		strcat((unsigned char *)PotFS_buffer,(const unsigned char *)incoming);

		for (Index_u8 = 0; Index_u8 < strlen((const unsigned char *)PotFS_buffer) ;Index_u8++)
		{
			printf("\r\nServer received::%c", PotFS_buffer[Index_u8]);
		}

		printf("\r\nCopy to buffer is  finished........................................");
		printf("\r\nGLobal buffer is ::%s",PotFS_buffer);
		incoming = NULL;
		Exp_Result = Validate_Exp(0);
		printf("\r\nresult var = .......................................%d",Exp_Result);

	 	if(!Exp_Result)
	 	{
	 		Pot_Fs_RXD_u8 = 1;
	 		strcat (PostAck,"Success");
	 	}
	 	else
	 	{
			strcat (PostAck,"ERROR CODE  = ");
			ptr = Convert_Integer_To_String(Exp_Result);
			strcat(PostAck,ptr);
		}
		printf("\r\nPOST ACK Buffer data is ::%s",PostAck);
		REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
		REST.set_response_payload(response, PostAck,strlen(PostAck));
	}	
	/* Check POST handler for Temprature sensor */
	else if(len = REST.get_post_variable(request, "TempFS", &incoming))
	{
        printf("incoming data:: %s %d\n",incoming, len);
        memset(TempFS_buffer,0, sizeof(TempFS_buffer));
        strcat((unsigned char *)TempFS_buffer,(const unsigned char *)incoming);

        for (Index_u8 = 0; Index_u8<strlen((const unsigned char *)TempFS_buffer) ;Index_u8++)
        {
			printf("\r\nServer received::%c", TempFS_buffer[Index_u8]);
        }

        printf("\r\nCopy to TempFS buffer is  finished........................................");
        printf("\r\nGLobal TEMP buffer is ::%s",TempFS_buffer);
        incoming = NULL;
        Exp_Result = Validate_Exp(1);
        printf("\r\nresult var = .......................................%d",Exp_Result);

        if(!Exp_Result)
        {
	        Temp_Fs_RXD_u8 = 1;
            strcat (PostAck,"Success");
        }
        else
        {
            strcat (PostAck,"ERROR CODE  = ");
            ptr = Convert_Integer_To_String(Exp_Result);
            strcat(PostAck,ptr);
        }
        printf("\r\nPOST ACK Buffer data is :: %s",PostAck);
        REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
        REST.set_response_payload(response, PostAck,strlen(PostAck));
	}	
	/* Check POST handler for Current Sensor */
	else if(len = REST.get_post_variable(request, "CurFS", &incoming))
	{
        printf("incoming data:: %s %d\n",incoming, len);
        memset(CurFS_buffer,0, sizeof(CurFS_buffer));
        strcat((unsigned char *)CurFS_buffer,(const unsigned char *)incoming);

        for (Index_u8 = 0; Index_u8<strlen((const unsigned char *)CurFS_buffer) ;Index_u8++)
        {
           printf("\r\nServer received::%c", CurFS_buffer[Index_u8]);
        }

        printf("\r\nCopy to Current buffer is  finished........................................");
        printf("\r\nGLobal CUR buffer is ::%s",CurFS_buffer);
        incoming = NULL;
        Exp_Result = Validate_Exp(2); //need to change this
        printf("\r\nresult var = .......................................%d",Exp_Result);

        if(!Exp_Result)
        {
	        Current_Fs_RXD_u8 = 1;
            strcat (PostAck,"Success");
        }
        else
        {
            strcat (PostAck,"ERROR CODE  = ");
            ptr = Convert_Integer_To_String(Exp_Result);
            strcat(PostAck,ptr);
        }
        printf("\r\nPOST ACK Buffer data is :: %s",PostAck);
        REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
        REST.set_response_payload(response, PostAck,strlen(PostAck));
	}
	else
	{
		printf("\r\nFailed to fetch data.........................................");
		Exp_Result = 03;
		strcat (PostAck,"ERROR CODE  = ");
		ptr = Convert_Integer_To_String(Exp_Result);
		strcat(PostAck,ptr);
		printf("\r\nPOST ACK Buffer data is ::%s",PostAck);
		REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
		REST.set_response_payload(response, PostAck,strlen(PostAck)); //testing
		REST.set_response_status(response, REST.status.BAD_REQUEST);
	}
}
EVENT_RESOURCE(res_FS,"title=\"Test: FS Service\";obs",res_FS_get_handler,res_FS_post_handler,NULL,NULL,NULL);



/**********************************************************************************
	* Function Name 	: 	Parse_Filterdata
	* Description		:	Parse the Data segregate the filter sybols & range
**********************************************************************************/
int Parse_Filterdata(unsigned char *FS_buffer,unsigned char *Remap_vars,unsigned char *Pop_Symbols,double *Pop_value)
{
    unsigned char *ptr;
    unsigned char Index_u8 = 0;
    unsigned char Val_Index_u8 = 0;
    unsigned int dec;
	float frac;
    float Pop_Result = 0;
    unsigned char tempbuf[20];

    ptr = strstr(FS_buffer,"::");
    ptr++;
    ptr++;    
    top = -1;
    PushIndex = 0;

	// get the remap variables	
    memset(Remap_vars,0,sizeof(Remap_vars));
    strcat(Remap_vars,(char *)ptr);

    printf("\r\nRemappable variable is ::%s ",Remap_vars);
	printf("\r\nGLobal buffer is ::%s ",FS_buffer);
    
	// e.g. "(>=0.5&<=3.3)::P"
	
    for(PushIndex = 0; FS_buffer[PushIndex]!= ':'; PushIndex++)
    {
        if(FS_buffer[PushIndex]=='('||FS_buffer[PushIndex]=='['||FS_buffer[PushIndex]=='{')
        {
            Push(FS_buffer[PushIndex]);
            printf("\r\n %c is Pushed into Stack ",FS_buffer[PushIndex]);
        }        
        else if((FS_buffer[PushIndex] >= 0x30 && FS_buffer[PushIndex] <= 0x39) || (FS_buffer[PushIndex] == '.'))
        {
			// For 0 to 9
            Convert_Ascii_to_Float_push(&FS_buffer[PushIndex]);
        }
        else if(FS_buffer[PushIndex] == '<' || FS_buffer[PushIndex] == '=' || FS_buffer[PushIndex] == '>' || FS_buffer[PushIndex] == '&')
        {
            Push(FS_buffer[PushIndex]);
            printf("\r\n %c is Pushed into Stack ",FS_buffer[PushIndex]);
        }
        else if(FS_buffer[PushIndex]==')'||FS_buffer[PushIndex]==']'||FS_buffer[PushIndex]=='}')
        {
			printf("\r\n POP started ");
            Pop_Result = Pop();
            memset(Pop_value,0,sizeof(Pop_value));
            memset(Pop_Symbols,0,sizeof(Pop_Symbols));

            if(Pop_Result == '<' || Pop_Result == '=' || Pop_Result == '>' || Pop_Result == '&')
            {
                Pop_Symbols[Index_u8++] = Pop_Result;
                printf("\r\n %c outof <,=,>,& is Popped form the Stack",(char)Pop_Result);
                printf("\r\n Push indexx = %d ",top);
            }
            else if((Pop_Result == '(')||(Pop_Result == '[')||(Pop_Result == '{') || (Pop_Result == ')')||(Pop_Result == ']')||(Pop_Result == '}') ||(Pop_Result == '$')||(Pop_Result == '#')||(Pop_Result == '!'))
            {
                // Do nothing
                printf("\r\n Braces ");
            }
            else
            {
                Pop_value[Val_Index_u8++] = (float)Pop_Result;
				dec = Pop_Result;		
				frac = Pop_Result - dec;	
				printf("\r\n %d.%02u is Popped form the Stack", dec, (unsigned int)(frac*100));
            }
            
            while(Pop_Result != '(' && Pop_Result != '[' && Pop_Result != '{')
            {
                Pop_Result = Pop();
                
                if(Pop_Result == '<' || Pop_Result == '=' || Pop_Result == '>' || Pop_Result == '&')
                {
                    Pop_Symbols[Index_u8++] = Pop_Result;
                    printf("\r\n %c outof <,=,>,& symbol is Popped form the Stack",(char )Pop_Result);                    
                }
                else if((Pop_Result == '(')||(Pop_Result == '[')||(Pop_Result == '{') || (Pop_Result == ')')||(Pop_Result == ']')||(Pop_Result == '}') ||(Pop_Result == '$')||(Pop_Result == '#')||(Pop_Result == '!'))
		        {
		            printf("\r\n Braces ");
		        }
                else
                {
                    Pop_value[Val_Index_u8++] = (float)Pop_Result;
					dec = Pop_Result;		
					frac = Pop_Result - dec;	
					printf("\r\n %d.%02u is Popped form the Stack", dec, (unsigned int)(frac*100));
                }
            }

            Pop_Symbols[Index_u8++]  = 0;
            ptr = strrev(Pop_Symbols);
            memset(tempbuf,0,sizeof(tempbuf));
            strcat(tempbuf,(char *)ptr);
            strcpy((unsigned char *)Pop_Symbols,(char *)tempbuf);
            Pop_value[Val_Index_u8] = 0;
            ReorderPopvalues(Pop_value);
            Index_u8 = 0;
            Val_Index_u8 = 0;
            printf("\r\n Popped values are %s ",Pop_value);
            printf("\r\n Pop symbols buffer data is  ::  %s",((char *)Pop_Symbols));
        }
        else
        {
            //operator not found
            return OPERATOR_NOT_FOUND;
        }
    }
    
    printf("\r\n top value = %d",top);
    if (top == -1)
    {
        printf("\r\n Balanced expression....................");
        return BALANCED_EXPRESSION;
    }
    else
    {
        printf("\r\n un balanced expression...................");
        return UNBALANCED_EXPRESSION;
    }
}


/**********************************************************************************
	* Function Name 	: 	ReorderPopvalues
	* Description		:	Revers the index
**********************************************************************************/
void ReorderPopvalues(double *Pop_value)
{
    float rtemp[10];
    int  Index_u8 = 0,len = 0, j = 0;

    memset(rtemp,0,sizeof(rtemp));
    for( Index_u8 = 0; Pop_value [Index_u8] != '\0'; Index_u8++)
    {
        rtemp[Index_u8] = Pop_value [Index_u8];
        len ++;
    }

    for( Index_u8 = len-1 ; rtemp [j] != '\0'; j++,Index_u8--)
    {
        Pop_value [j] = rtemp[Index_u8];
    }
    Pop_value [j] = 0;
}

/**********************************************************************************
	* Function Name 	: 	Validate_Exp
	* Description		:	Checking Paranthesis & parsing the fiter input string
**********************************************************************************/
unsigned char Validate_Exp(unsigned char SelOption)
{
    unsigned char Exp_output_u8;

	switch(SelOption)
	{
		case 0:		
			if(Check_For_Paranthesis(PotFS_buffer))
			{
				Exp_output_u8 = Parse_Filterdata(PotFS_buffer,PotRemap_vars,&PotPop_Symbols,&PotPop_value);
				printf("\r\nPop symbols buffer data is  ::  %s",((char *)PotRemap_vars));
				printf("\r\nPop symbols buffer data is  ::  %s",((char *)PotPop_Symbols));
				printf("\r\nPop values  buffer data is  ::  %s",((char *)PotPop_value));				
			}
		break;
		case 1:
			if(Check_For_Paranthesis(TempFS_buffer))
			{
				Exp_output_u8 = Parse_Filterdata(TempFS_buffer,TempRemap_vars,TempPop_Symbols,&TempPop_value);	
				printf("\r\nPop symbols buffer data is  ::  %s",((char *)TempRemap_vars));
				printf("\r\nPop symbols buffer data is  ::  %s",((char *)TempPop_Symbols));
				printf("\r\nPop values  buffer data is  ::  %s",((char *)TempPop_value));		
			}
		break;
		case 2:
			if(Check_For_Paranthesis(CurFS_buffer))
			{
				Exp_output_u8 = Parse_Filterdata(CurFS_buffer,CurRemap_vars,CurPop_Symbols,&CurPop_value);
				printf("\r\nPop symbols buffer data is  ::  %s",((char *)CurRemap_vars));
				printf("\r\nPop symbols buffer data is  ::  %s",((char *)CurPop_Symbols));
				printf("\r\nPop values  buffer data is  ::  %s",((char *)CurPop_value));				
			}
		break;		
		default:
		break;			
	}
	return Exp_output_u8;
}



/**********************************************************************************
	* Function Name 	: 	DoFiltering;
	* 
	* Description		:	Filter the input data w.r.t. current filter range
**********************************************************************************/
unsigned char DoFiltering(unsigned char Filter_sel)
{
	unsigned int dec;
	float frac;
	unsigned char retval = 0;		
			
	switch (Filter_sel)
	{
		case 0:		   
			if(PotRemap_vars[0] == 'P')
			{	
				dec = PotPop_value[0];		
				frac = PotPop_value[0] - dec;	
				printf("\r\n Filter 1 = %d.%02u", dec, (unsigned int)(frac*100));				
				dec = PotPop_value[1];		
				frac = PotPop_value[1] - dec;	
				printf("\r\n Filter 2 = %d.%02u", dec, (unsigned int)(frac*100));
	
				printf("\r\n POT 1 Filter active");	
				cSensorParameter = POT_1_Voltage;
				dec = cSensorParameter;		
				frac = cSensorParameter - dec;	
				printf("\r\n Sensor Raw  Value  = %d.%02u", dec, (unsigned int)(frac*100));
				retval = Applyfilter(PotPop_Symbols,PotPop_value[0],PotPop_value[1]);
			}
			else 
			{
				printf("\r\n POT1 --- Do Filter process failed");
			}			
		break;
		
		case 1:		   
			if(PotRemap_vars[0] == 'P')
			{	
				dec = PotPop_value[0];		
				frac = PotPop_value[0] - dec;	
				printf("\r\n Filter 1 = %d.%02u", dec, (unsigned int)(frac*100));				
				dec = PotPop_value[1];		
				frac = PotPop_value[1] - dec;	
				printf("\r\n Filter 2 = %d.%02u", dec, (unsigned int)(frac*100));
	
				printf("\r\n POT 2 Filter active");	
				cSensorParameter = POT_2_Voltage;
				dec = cSensorParameter;		
				frac = cSensorParameter - dec;	
				printf("\r\n Sensor Raw  Value  = %d.%02u", dec, (unsigned int)(frac*100));
				retval = Applyfilter(PotPop_Symbols,PotPop_value[0],PotPop_value[1]);
			}
			else 
			{
				printf("\r\n POT 2 --- Do Filter process failed");
			}			
		break;
		
		case 2:
			if(TempRemap_vars[0] == 'T')
			{
				dec = TempPop_value[0];		
				frac = TempPop_value[0] - dec;	
				printf("\r\n Filter 1 = %d.%02u", dec, (unsigned int)(frac*100));				
				dec = TempPop_value[1];		
				frac = TempPop_value[1] - dec;	
				printf("\r\n Filter 2 = %d.%02u", dec, (unsigned int)(frac*100));
	
				printf("\r\n TEMPRATURE Filter active");	
				cSensorParameter = Temp_Lm61_u32;
				dec = cSensorParameter;		
				frac = cSensorParameter - dec;	
				printf("\r\n Sensor Raw  Value  = %d.%02u", dec, (unsigned int)(frac*100));
				retval = Applyfilter(TempPop_Symbols,TempPop_value[0],TempPop_value[1]);
			}
			else
			{
				printf("\r\n TEMPRATURE --- Do Filter process failed");
			}
		break;
		
		case 3:
			if(CurRemap_vars[0] == 'C')
			{
				dec = CurPop_value[0];		
				frac = CurPop_value[0] - dec;	
				printf("\r\n Filter 1 = %d.%02u", dec, (unsigned int)(frac*100));				
				dec = CurPop_value[1];		
				frac = CurPop_value[1] - dec;	
				printf("\r\n Filter 2 = %d.%02u", dec, (unsigned int)(frac*100));
				
				printf("\r\n Current Filter active");	
				cSensorParameter = Current_Value_u32;
				dec = cSensorParameter;		
				frac = cSensorParameter - dec;	
				printf("\r\n Sensor Raw  Value  = %d.%02u", dec, (unsigned int)(frac*100));
				retval = Applyfilter(CurPop_Symbols,CurPop_value[0],CurPop_value[1]);
			}
			else
			{
				printf("\r\n Do Filter process failed");
			}
		break;
		
		default:
		break;
	}
	
	
    if(retval)
    {
	    //Exp_Result is within range
        printf("\r\n Filter process pass ........................................");
        return 1;
    }
    else
    {
	    //Exp_Result is outof range	
        printf("\r\n Filter process fail........................................");
        return 0;
    }
}

/**********************************************************************************
	* Function Name 	: 	Applyfilter
	* Description		:	
**********************************************************************************/
unsigned char Applyfilter(unsigned  char *string,double filt1,double filt2)
{
	static int dec;
	static float frac;
    unsigned  char Index_u8 = 0, j = 0,temp[20];

    FS.LimitValue = filt1;
    
    memset(temp,0,sizeof(temp));
    strcat(temp,(const char*)string);
    printf("\r\nTemp buffer data is %s ",temp);    
	dec = FS.LimitValue;		
	frac = FS.LimitValue - dec;	
	printf("\r\n struct filter value  =%d.%02u", dec, (unsigned int)(frac*100));	
	dec = cSensorParameter;		
	frac = cSensorParameter - dec;	
	printf("\r\n cSensorParameter Value  =%d.%02u", dec, (unsigned int)(frac*100));

    for (Index_u8 = 0; temp[Index_u8] != '\0'; Index_u8++)
    {
        switch(temp[Index_u8])
        {
            case '<':
                if(cSensorParameter < FS.LimitValue )
                {
                    FS.Flag = TRUE;
                    FS.SecondFilterRes = FS.Flag;
                }
                else
                {
                    FS.Flag = FALSE;
                    FS.SecondFilterRes = FS.Flag;
                }
                break;

            case '>':
                if(cSensorParameter > FS.LimitValue)
                {
                    FS.Flag = TRUE;
                    FS.FirstFilterRes = FS.Flag;
                }
                else
                {
                    FS.Flag = FALSE;
                    FS.FirstFilterRes = FS.Flag;
                }
                break;

            case '=':
                j =  Index_u8;

                //To avoid error condition of Index_u8 = 0 then j = -255
                if(Index_u8)
					j--;

                if(temp[j] == '<')
                {
                    if(cSensorParameter <= FS.LimitValue )
                    {
                        FS.Flag = TRUE;
                        FS.SecondFilterRes = FS.Flag;
                    }
                    else
                    {
                        FS.Flag = FALSE;
                        FS.SecondFilterRes = FS.Flag;
                    }
                }
                else if(temp[j] == '>')
                {
                    if(cSensorParameter >= FS.LimitValue )
                    {
                        FS.Flag = TRUE;
                        FS.FirstFilterRes = FS.Flag;
                    }
                    else
                    {
                        FS.Flag = FALSE;
                        FS.FirstFilterRes = FS.Flag;
                    }
                }
                else
                {
                   if(cSensorParameter == FS.LimitValue )
                    {
                        FS.Flag = TRUE;
                    }
                    else
                    {
                        FS.Flag = FALSE;
                    }
                }
                break;

            case '&':
                //Change limit value to filter condition2
                FS.LimitValue = filt2;
                break;

            default:
                printf("\n Default case....................");
                break;
        }
    }

    #ifdef DEBUG
		printf("\r\nFirst filter result is ::%d........................................",FS.FirstFilterRes);
		printf("\r\nSecond filter result is :: %d........................................",FS.SecondFilterRes);
	#endif

    if( FS.SecondFilterRes && FS.FirstFilterRes)
    {
        FS.Result = TRUE;
        #ifdef DEBUG
			printf("\r\n & op of two results is %d........................................",FS.Result);
        #endif
        return FS.Result;
    }
    else
    {
        FS.Result = FALSE;
        #ifdef DEBUG
        printf("\r\nelse condition %d........................................",FS.Result);
		#endif
        return FS.Result;
    }
}


