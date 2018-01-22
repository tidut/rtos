/* *******************************************************
 * *******************************************************
 * File Name 	: 	res-DFMS.c 
 * Description 	:	Data Function modifier service 
 * 					This file is to set conversion formulas not for filter services
 * 					Used only when there is a Client node + some server 
 * 				
 * 					Data modifier function has been written only for Tempratrure formula conversion
 * 					e.g convert celsius to farhen vice-versa
 * Written by 	: 
 * *******************************************************
 * *******************************************************/
#include <math.h>
#include <string.h>
#include "uart.h"
#include "common.h"
#include "contiki.h"
#include "contiki.h"
#include "er-coap.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "dev/als-sensor.h"
#include "er-coap-engine.h"

unsigned char Parse_data(void);
double dpower(double value,int pow);
void DoOpeartion(unsigned char op);
void Perform_opeartion(unsigned char *string);

extern int top;
extern float stack[64];

float Pop_Operands[20];
double Pop_value[20];

extern unsigned char PushIndex;
extern unsigned int Temp_Lm61_u32;

unsigned char DFMS_buffer[200];
unsigned char Pop_Operators[20];

volatile float Temprature_Final_Res;
unsigned char DFMS_Rxd_u8 = 0;

/******************************* DFMS POST and GET handlers  *******************************
***********************************************************************************************************
***********************************************************************************************************/

static void res_DFMS_get_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{	
}

static void res_DFMS_post_handler(void *request, void *response, unsigned char *buffer, uint16_t preferred_size, int32_t *offset)
{
	char *ptr;
	size_t len = 0;
	
	unsigned char Index_u8;
	unsigned char Parse_Data_Res_u8 = 1;
	unsigned char *incoming = NULL;
	unsigned char PostAck[100];

	printf("\r\nExecuting DFMS POST handler.........................................");
	memset(PostAck,0,sizeof(PostAck));
	
	if(len = REST.get_post_variable(request, "DFMS", &incoming)) 
	{
    	printf("incoming data:: %s %d\n",incoming, len);    	
		memset(DFMS_buffer,0, sizeof(DFMS_buffer));
		strcat((unsigned char *)DFMS_buffer,(const unsigned char *)incoming);

		for (Index_u8 = 0; Index_u8 < strlen(DFMS_buffer) ;Index_u8++)
		{
			printf("\r\nServer received::%c", DFMS_buffer[Index_u8]);
		}
		printf("\r\nCopy to buffer is  finished........................................");		
		incoming = NULL;	
		
		// check for {}	or [] or ()
		if(Check_For_Paranthesis(DFMS_buffer))
		{		
			DFMS_Rxd_u8 = 1;
			// help to remap & what operations needs to be done
			Parse_Data_Res_u8 = Parse_data();
		}		

	 	printf("\r\nresult var = .......................................%d",Parse_Data_Res_u8);	 	
	 	if(!Parse_Data_Res_u8)
	 	{
	 		strcat (PostAck,"Success");
	 	}	 	
	 	else
	 	{
			strcat (PostAck,"ERROR CODE = ");		
			ptr = Convert_Integer_To_String(Parse_Data_Res_u8);
			strcat(PostAck,ptr);
		}
		printf("\r\nPOST ACK Buffer data is ::%s",PostAck);		
		REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
		REST.set_response_payload(response, PostAck,strlen(PostAck)); 
	}	
	else
	{
		printf("\r\nFailed to fetch data.........................................");
		Parse_Data_Res_u8 = 03;
		strcat (PostAck,"ERROR CODE  = ");		
		ptr = Convert_Integer_To_String(Parse_Data_Res_u8);
		strcat(PostAck,ptr);
		printf("\r\nPOST ACK Buffer data is %s",PostAck);		
		REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
		REST.set_response_payload(response, PostAck,strlen(PostAck)); 
		REST.set_response_status(response, REST.status.BAD_REQUEST);
	}	
}
EVENT_RESOURCE(res_DFMS,"title=\"Test: DFMS Service\";obs",res_DFMS_get_handler,res_DFMS_post_handler,NULL,NULL,NULL);


/**********************************************************************************
	* Function Name 	:	Parse_data
	* Description		:	This function seprates the Data, operator ,operands & paranthesis
	* 						does the remapping also		
**********************************************************************************/
unsigned char Parse_data(void)
{
    unsigned char *ptr;
    unsigned char OP_Index_u8 = 0;
    unsigned char Val_Index_u8 = 0;
    unsigned char Remap_vars[10];
    float I_Value = 2.34;
    float Pop_Result = 0;

	/* For example "(V*I)::V,I" v,i are remapable variables */
    ptr = strstr(DFMS_buffer,"::");
    ptr++;
    ptr++;
    memset(Remap_vars,0,sizeof(Remap_vars));
    strcat(Remap_vars,(char *)ptr);
    
    top = -1;

    for(PushIndex = 0; DFMS_buffer[PushIndex]!= ':'; PushIndex++)
    {
        if(DFMS_buffer[PushIndex]=='('||DFMS_buffer[PushIndex]=='['||DFMS_buffer[PushIndex]=='{')
        {
            Push(DFMS_buffer[PushIndex]);
            printf("\r\n %c is Pushed into Stack....................",DFMS_buffer[PushIndex]);
        }
        else if(DFMS_buffer[PushIndex] >= 0x30 && DFMS_buffer[PushIndex] <= 0x39)
        {
			// convert ASCI  to Float For 0 to 9
            Convert_Ascii_to_Float_push(&DFMS_buffer[PushIndex]);
        }        
        else if((DFMS_buffer[PushIndex] >= 0x41 && DFMS_buffer[PushIndex] <= 0x5A) ||(DFMS_buffer[PushIndex] >= 0x61 && DFMS_buffer[PushIndex] <= 0x7A))
        {	        
	        //Mapping to actual variables
	        //For a to z and A to Z
            if(((Remap_vars[0] == DFMS_buffer[PushIndex]) && (Remap_vars[0] !='!') && (Remap_vars[0] !='$') && (Remap_vars[0] !='#') && (Remap_vars[0] !='&')))
                Push((float)Temp_Lm61_u32);

            else if(Remap_vars[2] == DFMS_buffer[PushIndex] && (Remap_vars[2] !='!') && (Remap_vars[2] !='$') && (Remap_vars[2] !='#') && (Remap_vars[2] !='&'))
                Push(I_Value);

            printf("\r\n %d is Pushed into Stack....................",DFMS_buffer[PushIndex]);
        }
        else if(DFMS_buffer[PushIndex] == '+' || DFMS_buffer[PushIndex] == '-' || DFMS_buffer[PushIndex] == '*' || DFMS_buffer[PushIndex] == '/' || DFMS_buffer[PushIndex] == '^' || DFMS_buffer[PushIndex] == '%')
        {
            Push(DFMS_buffer[PushIndex]);
            printf("\r\n %c is Pushed into Stack....................",DFMS_buffer[PushIndex]);
        }
        else if(DFMS_buffer[PushIndex]==')'||DFMS_buffer[PushIndex]==']'||DFMS_buffer[PushIndex]=='}')
        {
            printf("\r\n push index when fist closed brace is occured = %d",PushIndex);
            
            //  first look for closed brace
            Pop_Result = Pop();
            memset(Pop_Operators,0,sizeof(Pop_Operators));
            memset(Pop_value,0,sizeof(Pop_value));

            //For a to z and A to Z
            if((Pop_Result >= 0x41 && Pop_Result <= 0x5A) ||(Pop_Result >= 0x61 && Pop_Result <= 0x7A))
            {
                printf("\r\n %d is Popped form the Stack....................",Pop_Result);
            }
            else if(Pop_Result == '+' || Pop_Result == '-' || Pop_Result == '*' || Pop_Result == '/' || Pop_Result == '^' || Pop_Result == '%')
            {
                Pop_Operators[OP_Index_u8++] = Pop_Result;
                printf("\r\n %d is Popped form the Stack....................",Pop_Result);
            }
            else if( (Pop_Result == '(')||(Pop_Result == '[')||(Pop_Result == '{') || (Pop_Result == ')')||(Pop_Result == ']')||(Pop_Result == '}') || (Pop_Result == '$')||(Pop_Result == '#')||(Pop_Result == '!'))
            {
                // Do nothing
            }
            else
            {
                Pop_value[Val_Index_u8++] = (float)Pop_Result;
                printf("\r\n %f is Popped form the Stack....................",Pop_Result);
            }

			//  WAIT untill open brace comes
			while(Pop_Result != '(' && Pop_Result != '[' && Pop_Result != '{')
            {	            	             
                Pop_Result = Pop();
                if((Pop_Result >= 0x41 && Pop_Result <= 0x5A) ||(Pop_Result >= 0x61 && Pop_Result <= 0x7A))
                {
                    printf("\r\n %f is Popped form the Stack....................",Pop_Result);

                }
                else if(Pop_Result == '+' || Pop_Result == '-' || Pop_Result == '*' || Pop_Result == '/' || Pop_Result == '^' || Pop_Result == '%')
                {
                    Pop_Operators[OP_Index_u8++] = Pop_Result;
                    printf("\r\n %f is Popped form the Stack....................",Pop_Result);
                }
                else if( (Pop_Result == '(')||(Pop_Result == '[')||(Pop_Result == '{') || (Pop_Result == ')')||(Pop_Result == ']')||(Pop_Result == '}') || (Pop_Result == '$')||(Pop_Result == '#')||(Pop_Result == '!'))
                {
                    // Do nothing
		        }
                else
                {
                    Pop_value[Val_Index_u8++] = (float)Pop_Result;
                    printf("\r\n %f is Popped form the Stack....................",Pop_Result);
                }
            }

            Pop_Operators[OP_Index_u8] = 0;
            Pop_value[Val_Index_u8] = 0;
            OP_Index_u8 = 0; Val_Index_u8 = 0;

            printf("\r\nOperators are %s...................",Pop_Operators);
            printf("\r\nPopped values are %d...................",Pop_value);
            Perform_opeartion(Pop_Operators);
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
	* Function Name 	: 	 Perform_opeartion
	* Description		:	 This functions takes operator as input  (12 * 23) 
	* 						 thses operands will be there in Pop_value buffer
**********************************************************************************/
void Perform_opeartion(unsigned char *string)
{
    unsigned char Index_u8 = 0;
    unsigned char temp[20];
    
    memset(temp,0,sizeof(temp));
    strcat(temp,(const unsigned char*)string);
    printf("\r\nTemp buufer data is %s...................",temp);
    
    for (Index_u8 = 0; temp[Index_u8] != '\0'; Index_u8++)
    {
        switch(temp[Index_u8])
        {
            case '+':
                DoOpeartion('+');
                break;
            case '-':
                DoOpeartion('-');
                break;
            case '*':
                DoOpeartion('*');
                break;
            case '/':
                DoOpeartion('/');
                break;
            case '^':
                DoOpeartion('^');
                break;                
            case 's':
                break;
            default:
                printf("\n Default case....................");
                break;
        }
    }
}

/**********************************************************************************
	* Function Name 	: 	 DoOpeartion
	* Description		:	 perform the operation
**********************************************************************************/
void DoOpeartion(unsigned char op)
{
	uint16_t dec = 0;
	float fractional = 0;		
   
    switch(op)
    {
        case '+':
            Temprature_Final_Res = Pop_value[1]+Pop_value[0];
            break;
        case '-':
            Temprature_Final_Res = Pop_value[1]-Pop_value[0]; 
            break;
        case '*':
            Temprature_Final_Res = Pop_value[1]*Pop_value[0];
            break;
        case '/':
            Temprature_Final_Res = Pop_value[1]/Pop_value[0];
            break;
        case '^':
            Temprature_Final_Res = dpower(Pop_value[1],Pop_value[0]);
            break;
    }
    
	dec = Temprature_Final_Res;
	fractional = Temprature_Final_Res-dec;
	printf("\r\n / result is  = %d.%4u",dec,(unsigned int)(fractional*10000));
    printf("\r\n top is %d ....................",top);
    
    if (top != -1)
    {
		//push back the result back to stack
        Push(Temprature_Final_Res);
        printf("\r\n Temprature_Final_Res %f is Pushed into stack....................",Temprature_Final_Res);
    }
    printf("\r\n performed %c opearation....................",op);    
}

/**********************************************************************************
	* Function Name 	: 	 dpower
	* Description		:	 Conti-ki wont support power function also so 
	* 						 through multiplication only this implementaion is possible
**********************************************************************************/
double dpower(double value,int pow)
{
	double Output = 0 ;
	if(pow == 2)
	{
		Output = (value * value);
	}	
	else if(pow == 3)
	{
		Output = (value * value * value);
	}	
	return Output;
}


