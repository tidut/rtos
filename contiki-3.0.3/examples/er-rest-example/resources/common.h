#ifndef _COM_
#define _COM_

typedef enum 
{	
	BALANCED_EXPRESSION = 0,
	OPERATOR_NOT_FOUND,
	UNBALANCED_EXPRESSION,
}ERROR_CODES;

char *strrev(char *string);
char *Convert_Integer_To_String(int value);

float Pop(void);
float Convert_String_Float(unsigned char *str,float resu);

void Push(float c);
void Convert_Ascii_to_Float_push(unsigned char *str);
void Get_IPV6_addresses(void);

int Check_For_Paranthesis(char *string);


#endif
