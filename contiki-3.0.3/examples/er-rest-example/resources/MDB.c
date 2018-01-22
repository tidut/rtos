#include "MDB.h"

 const unsigned CRCHigh[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
};

/* Table of CRC values for low�order byte */
const unsigned CRCLow[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};

MDB_REQ_ADU mdbReqAdu;
MDB_RSP_ADU mdbRspAdu;
/**************************************************************************************************************************************************
                                                ...................................... Variable Declaration .......................
***************************************************************************************************************************************************/
char mbresp[30];
int  mbrespctr;
short int noOfBytesToTx;
short int noOfBytesToRx;
volatile unsigned char  MDB_Status;
extern unsigned char UART_1_RxBuffer_u8[100];;
extern volatile unsigned char MBResponse;


/***************************************************************************************************************************************
****************************************************************************************************************************************
	Description:
	This method is the main function that drives Modbus communication on a network.
	Parameters:
	a) networkNo (IN):      A number identifying the �port� on which Modbus communication has to happen.
	b) slaveNo (IN):        A single byte value containing the slave ID of the device from which data is being requested.
	c) functionCode (IN):   A single byte value of the Modbus function code that defines the Modbus service request.
	d) startAddress (IN):   A two-byte value that is the first address in the range of data being requested for.
	e) numItems (IN):       A two-byte value that is the number of data items starting from startAddress that are being requested for.
	f) dataBuffer-> (OUT):  Pointer to an array of bytes into which the requested data must be copied into in the correct format for 'Read' FCs.
				    -> (IN):   Pointer to an array of bytes containing the data that has to be 'written' to slave.
	g) numRetries (IN):     The number of times to retry communication with slave.
	
	Returns:
	Status Code.

	For Ex:
	ModbusOP(0x02, 0x03, 0x0F49, 0x0002, (unsigned char *)HoldingReg, 15);    
	NUMITEMS  SHOWS THAT NUMBER OF REGISTER ARE TOBE READ FROM THAT ADDRESS FROM 1 REGISTER TWO BYTES ARE STORED
**************************************************************************************************************************************
***************************************************************************************************************************************/

unsigned char  Write_MDB_Command(unsigned char  slaveNo, unsigned char  functionCode, short int startAddress, short int  numItems, unsigned char  *dataBuffer, unsigned char  numRetries)
{
	MDB_Status = MMPL_NO_ERROR;
	
	MDB_Status = ConstructRequest(slaveNo, functionCode, startAddress, numItems,dataBuffer, &mdbReqAdu, &mdbRspAdu);
	if( MDB_Status == MMPL_NO_ERROR)
	{
		/* Calculate size of request frame to be transmitted to the slave */
		noOfBytesToTx = mdbReqAdu.pduSize + 2 /* CRC bytes */ ;
		noOfBytesToRx = mdbRspAdu.pduSize + 2;
		memset(UART_1_RxBuffer_u8,0,100); 
		UART_1_Tx_Bytes(mdbReqAdu.TxBuffer,noOfBytesToTx);	

	}
	return MDB_Status;
}

unsigned char  Read_MDB_Response(unsigned char  slaveNo, unsigned char  functionCode, short int startAddress, short int  numItems, unsigned char  *dataBuffer, unsigned char  numRetries)
{
	MDB_Status = ReadPort(noOfBytesToRx, &mdbRspAdu);
	if (MDB_Status == MMPL_NO_ERROR)
	{
		MDB_Status = DecodeResponse(&mdbReqAdu, &mdbRspAdu, dataBuffer, numItems);
	}
}


/*****************************************************************************************************************************************
******************************************************************************************************************************************
	Description:
	This method constructs the Modbus request ADU that has to be sent to the slave.
	Parameters:
	a) slaveNo (IN):        	A single byte value containing the slave ID of the device from which data is being requested.
	b) functionCode (IN):   	A single byte value of the Modbus function code that defines the Modbus service request.
	c) startAddress (IN):   	A two-byte value that is the first address in the range of data being requested for.
	d) numItems (IN):       	A two-byte value that is the number of data items starting from startAddress that are being requested for.
	e) dataBuffer -> (OUT):  	Pointer to an array of bytes into which the requested data must be copied into in the correct format for 'Read' FCs.
				  -> (IN) :  	Pointer to an array of bytes containing the data that has to be 'written' to slave.
	f) pMbReqAdu (OUT):      	Pointer to a structure where the request ADU has to be stored.
	g) pMbRspAdu (OUT):      	Pointer to a structure of the response ADU (response PDU size is estimated in some cases).
	
	Returns:
	Status Code.

	PDU Size = FUNCTION CODE + START ADDRESS + NUMITEMS
	PDU SIZE  = 1 + 2 + 2	SO Always Fixed Size.......

	Role of this Function:

	1. This function is used to fill the transmit packet 
	2. Transmit packet contains the Slave number, Function Code, Num of Items
	3. MbReqAdu->TxBuffer Fill the transmit buffer
	4. From the Num of items to be read it decides the response Pdu size 

		byteCount  			= 	(unsigned char )(numItems * 2);	// byte count  
		MbRspAdu->pduSize 	= 	byteCount + 2; 					// Slave number + FC + ByteCnt  1+1+4 

*****************************************************************************************************************************************
*******************************************************************************************************************************************/
unsigned char  ConstructRequest(unsigned char  slaveNo, unsigned char  functionCode, short int startAddress, short int  numItems, unsigned char  *dataBuffer,MDB_REQ_ADU *MbReqAdu, MDB_RSP_ADU *MbRspAdu)
{
	unsigned int  byteCount;
	short int NumItems  = numItems;
	
	MDB_Status = MMPL_NO_ERROR;
	
	/********************** Check the Slave number It should not be grater than 247 & less than 0 *******************/	 
	if(! ((slaveNo >= 0)&&(slaveNo <= 247)) )
	{
		MDB_Status = INVALID_SLAVE_ADDR;
	} 
	else
	{	
		/*********************** Check the What type of Function Code used in Modbus transaction************************
		***********************	 Fill thw buffer which will be transmitted over MOdbus	******************************** 
		***********************  TxBuffer Contains the AddressField + Function Code + Data + CRC************************/				
		switch(functionCode)
		{				
			case FC_WRITE_SINGLE_COIL:
       		case FC_WRITE_SINGLE_REG :
                MbReqAdu->TxBuffer[INDEX_SLAVE_ID] 		    = slaveNo;
                MbReqAdu->TxBuffer[INDEX_FUNC_CODE] 	    = functionCode;
                MbReqAdu->TxBuffer[INDEX_START_ADDR1] 		= ((startAddress) & 0xFF00)>>8;
                MbReqAdu->TxBuffer[INDEX_START_ADDR2] 		= (startAddress) & 0x00FF;
                ShortIntsToBuffer((short int *)dataBuffer, &(MbReqAdu->TxBuffer[INDEX_VALUE1]), 1);
				MbReqAdu->pduSize = 6;			                                                
				AppendCrc(MbReqAdu);			
				MbRspAdu->pduSize = 6;
			break;

			case FC_READ_COILS:						
				MbReqAdu->TxBuffer[INDEX_SLAVE_ID]           	=  slaveNo;
				MbReqAdu->TxBuffer[INDEX_FUNC_CODE]       		=  functionCode;
				MbReqAdu->TxBuffer[INDEX_START_ADDR1]   		=  ((startAddress) & 0xFF00)>>8;			// Converting into Byte by Byte
				MbReqAdu->TxBuffer[INDEX_START_ADDR2]   		=  (startAddress) & 0x00FF;			        // Converting into Byte by Byte
				ShortIntsToBuffer((short int *)dataBuffer, &(MbReqAdu->TxBuffer[INDEX_VALUE1]), 1);
				MbReqAdu->pduSize = 6;		                                                              	// Fill the PDU Size
				AppendCrc(MbReqAdu);	
				MbRspAdu->pduSize = 4;                                                                      // CRC Check
			break;
		
			case FC_READ_HOLD_REGS:						
				MbReqAdu->TxBuffer[INDEX_SLAVE_ID]      = slaveNo;
				MbReqAdu->TxBuffer[INDEX_FUNC_CODE]     = functionCode;
				MbReqAdu->TxBuffer[INDEX_START_ADDR1]   = ((startAddress) & 0xFF00)>>8;						// Converting into Byte by Byte
				MbReqAdu->TxBuffer[INDEX_START_ADDR2]   = (startAddress) & 0x00FF;			        		// Converting into Byte by Byte
				MbReqAdu->TxBuffer[INDEX_NUM_ITEMS1]    = (NumItems & 0xFF00)>>8;							// Converting into Byte by Byte
				MbReqAdu->TxBuffer[INDEX_NUM_ITEMS2]    =  NumItems & 0x00FF;					    		// Converting into Byte by Byte					
				MbReqAdu->pduSize = 6;	                                        	                		// Fill the PDU Size
				AppendCrc(MbReqAdu);	                                                                	// CRC Check 
				byteCount  = NumItems*2;	                             
				MbRspAdu->pduSize = 3+byteCount; 		                                    
			break;		

			case FC_READ_IP_REGS:						
				MbReqAdu->TxBuffer[INDEX_SLAVE_ID]      = slaveNo;
				MbReqAdu->TxBuffer[INDEX_FUNC_CODE]     = functionCode;
				MbReqAdu->TxBuffer[INDEX_START_ADDR1]   = ((startAddress) & 0xFF00)>>8;						// Converting into Byte by Byte
				MbReqAdu->TxBuffer[INDEX_START_ADDR2]   = (startAddress) & 0x00FF;			        		// Converting into Byte by Byte
				MbReqAdu->TxBuffer[INDEX_NUM_ITEMS1]    = (NumItems & 0xFF00)>>8;							// Converting into Byte by Byte
				MbReqAdu->TxBuffer[INDEX_NUM_ITEMS2]    =  NumItems & 0x00FF;					    		// Converting into Byte by Byte					
				MbReqAdu->pduSize = 6;	                                        	                		// Fill the PDU Size
				AppendCrc(MbReqAdu);	                                                                	// CRC Check 
				byteCount  = NumItems*2;	                             
				MbRspAdu->pduSize = 3+byteCount; 		                                    
			break;
					   			
			default:
				MDB_Status = INVALID_FC;
			break;		   
		}
    }
    return MDB_Status;
}


/******************************************************************************************************************************************
*******************************************************************************************************************************************
	Description:
	This method decodes the Modbus response ADU received from the slave.
	Parameters:
	a) pMbReqAdu (IN): Pointer to a structure of the request ADU sent to slave.
	b) pMbRspAdu (IN): Pointer to a structure of the response ADU received from slave.
	c) dataBuffer (OUT): Pointer to an array of bytes into which the requested data must be copied into in the correct format for 'Read' FCs.
	d) numItems (IN): A two-byte value that is the number of data items starting from startAddress that are being requested for.
	
	Returns:
	Status Code.
	 Validate various parameters of response like transactionID, protocolCode, CRC, Function code etc.  
******************************************************************************************************************************************
/********************************************************************************************************************************************/

unsigned char  DecodeResponse(MDB_REQ_ADU *pMbReqAdu, MDB_RSP_ADU *pMbRspAdu, unsigned char  *dataBuffer, short int  numItems){

	short int  byteCount;
	MDB_Status = MMPL_NO_ERROR;

	if ((pMbReqAdu->TxBuffer[INDEX_SLAVE_ID]) != (pMbRspAdu->RxBuffer[INDEX_SLAVE_ID])){
	      MDB_Status = ID_MISMATCH;	         
      	  printf("\r\nModBus Error ID MISMATCH");      	
  	}
	else if( CheckCrc(pMbRspAdu) != TRUE ){
        MDB_Status = CRC_ERR;
        printf("\r\nModBus Error CRC_ERR");        	         
	}
	else if ((pMbRspAdu->RxBuffer[INDEX_FUNC_CODE]) & 0x80){
        dataBuffer[0] = pMbRspAdu->RxBuffer[INDEX_FUNC_CODE+1];	           
		MDB_Status = EXCEPTION_RESPONSE;
	    printf("\r\nModBus Error EXCEPTION_RESPONSE \n");		        	   
        
	}
	else if ((pMbReqAdu->TxBuffer[INDEX_FUNC_CODE]) != (pMbRspAdu->RxBuffer[INDEX_FUNC_CODE])){
        MDB_Status = FC_MISMATCH;
        printf("\r\nModBus Error FC_MISMATCH");        	           
	}
	else{
		switch(pMbRspAdu->RxBuffer[INDEX_FUNC_CODE]){		
		
			case FC_READ_HOLD_REGS:
			case FC_READ_IP_REGS:			
				byteCount  = (numItems * 2);       	         
				if( byteCount != (pMbRspAdu->RxBuffer[INDEX_RESP_BYTE_CNT]) ){
    				MDB_Status = INVALID_BYTECNT;  				    
    				printf("\r\nModBus Error INVALID_BYTECNT ");    			    
				}
				else{
					BufferToShortInts(&(pMbRspAdu->RxBuffer[INDEX_RESP_START_DATA]), (short int *)dataBuffer, numItems);
					MDB_Status = 0;
				}
			break;		
				
			case FC_WRITE_MULTIPLE_COILS:
			case FC_WRITE_MULTIPLE_REGS:

				if((pMbReqAdu->TxBuffer[INDEX_START_ADDR1] != pMbRspAdu->RxBuffer[INDEX_START_ADDR1]) ||(pMbReqAdu->TxBuffer[INDEX_START_ADDR2] != pMbRspAdu->RxBuffer[INDEX_START_ADDR2]) ||
				    (pMbReqAdu->TxBuffer[INDEX_VALUE1]      != pMbRspAdu->RxBuffer[INDEX_VALUE1]) 	   || (pMbReqAdu->TxBuffer[INDEX_VALUE2]      != pMbRspAdu->RxBuffer[INDEX_VALUE2]) ){
					MDB_Status = INVALID_DATA_VALUE;
    				printf("\r\nModBus Error INVALID_BYTECNT");					  	 
				}
			break;		
				
			case FC_READ_COILS:	
			   	BufferToShortInts(&(pMbRspAdu->RxBuffer[3]), (short int *)dataBuffer, numItems);
    			MDB_Status = 0;
			break;
    	}
    }
	return MDB_Status;
}


/*******************************************************************************************************************************************
*******************************************************************************************************************************************
	Description:
	This function validates the CRC bytes at the end of the Modbus ADU.
	
	Parameters:
	a) pMbRspAdu (IN):  Pointer to a structure holding the response ADU.
	
	Returns:
	TRUE if CRC is OK, else FALSE.

	  The total no. of bytes to be used to make the CRC (excludes the CRC bytes themselves)  
******************************************************************************************************************************************
********************************************************************************************************************************************/

unsigned char  CheckCrc(MDB_RSP_ADU *pMbRspAdu)
{
	unsigned char CRCHi = 0xFF;	                           	/* high byte of CRC initialized */
	unsigned char CRCLo = 0xFF;	                           	/* low byte of CRC initialized */
	unsigned Index;	                                       	/* will index into CRC lookup table */
	unsigned char  totalBytes = pMbRspAdu->pduSize;       	/* additional address */
	unsigned char ctr;

	printf("\r\nValidate CRC");	
	for(ctr=0; ctr<(totalBytes); ctr++)
	{    
		printf("\r\n CRC Bytes 0x%x = ",pMbRspAdu->RxBuffer[ctr]);	
		                									/* pass through message buffer to calculate CRC */
		Index = CRCLo ^ (pMbRspAdu->RxBuffer[ctr]);	        /* calculate the CRC */
		CRCLo = CRCHi ^ CRCHigh[Index];
		CRCHi = CRCLow[Index];
	} 
	if( (CRCLo == pMbRspAdu->RxBuffer[totalBytes]) && (CRCHi == pMbRspAdu->RxBuffer[totalBytes+1]))
	{
		return TRUE;
	}
	else
	{
		printf("\r\n CRC Bytes 0x%x != 0x%x",CRCLo,pMbRspAdu->RxBuffer[totalBytes]);
		printf("\r\n CRC Bytes 0x%x != 0x%x",CRCHi,pMbRspAdu->RxBuffer[totalBytes+1]);	
	}
	return FALSE;
}

/*******************************************************************************************************************************************
*******************************************************************************************************************************************
	Description:
	This function appends the CRC bytes at the end of the Modbus ADU.
	
	Parameters:
	a) pMbReqAdu (IN):  Pointer to a structure holding the request ADU.
	
	Returns:
	None.
******************************************************************************************************************************************
********************************************************************************************************************************************/
void AppendCrc(MDB_REQ_ADU *pMbReqAdu){

	unsigned char  CRCHi = 0xFF;	                            /* high byte of CRC initialized */
	unsigned char  CRCLo = 0xFF;	                            /* low byte of CRC initialized */
	short int Index;	                                        /* will index into CRC lookup table */
	unsigned char  ctr;	                                    	/* The total no. of bytes to be used to make the CRC (excludes the CRC bytes themselves) */
	unsigned char  totalBytes = pMbReqAdu->pduSize ;       		/* additional address */

	for(ctr=0; ctr<(totalBytes); ctr++)
	{                     										/* pass through message buffer to calculate CRC */
		Index = CRCLo ^ (pMbReqAdu->TxBuffer[ctr]);	         	/* calculate the CRC */
		CRCLo = CRCHi ^ CRCHigh[Index];
		CRCHi = CRCLow[Index];
	}
	pMbReqAdu->TxBuffer[totalBytes] = CRCLo;
	pMbReqAdu->TxBuffer[totalBytes+1] = CRCHi;
}


/******************************************************************************************************************************************
*******************************************************************************************************************************************
	Description:
	This method puts data into the destination buffer in such a way that a pair of bytes of the destination buffer
	is used to hold the value of one two-byte register (source buffer).
	
	Parameters:
	a) pSrcBuffer (IN): Buffer containing two-byte register values.
	b) pDstBuffer (OUT): A pre-allocated buffer into which the register values will have to be put.
	c) noOfRegs (IN): The no. of register values to copy from the source buffer.
	
	Returns:
	None
******************************************************************************************************************************************
********************************************************************************************************************************************/

void ShortIntsToBuffer(short int *pSrcBuffer, unsigned char  *pDstBuffer, short int noOfRegs){

	unsigned char  regNo, srcBufCtr, dstBufCtr;
	srcBufCtr = 0;
	dstBufCtr = 0;
	
	for(regNo=0; regNo<noOfRegs; regNo++)	
	{
		pDstBuffer[dstBufCtr++] = (pSrcBuffer[srcBufCtr] & 0xFF00)>>8;
		pDstBuffer[dstBufCtr++] = pSrcBuffer[srcBufCtr] & 0x00FF;
		srcBufCtr++;
	}
}

/*******************************************************************************************************************************************
*******************************************************************************************************************************************
	Description:
	This method puts data into the destination buffer in such a way that a pair of bytes of the source buffer
	is used to form the value of one two-byte register.
	
	Parameters:
	a) pSrcBuffer (IN): A pre-allocated buffer which has data of two-byte register in a pair of bytes.
	b) pDstBuffer (OUT): Buffer into which the two-byte register values will be put.
	c) noOfRegs (IN): The no. of register values to be copied into the destination buffer.
	
	Returns:
	None
******************************************************************************************************************************************
********************************************************************************************************************************************/
void BufferToShortInts(unsigned char  *pSrcBuffer, short int *pDstBuffer, short int noOfRegs)
{
	unsigned char  regNo, srcBufCtr, dstBufCtr;

	srcBufCtr = 0;
	dstBufCtr = 0;
	for(regNo=0; regNo<noOfRegs; regNo++)
	{
		pDstBuffer[dstBufCtr] = (pSrcBuffer[srcBufCtr])<<8;
		pDstBuffer[dstBufCtr] |= pSrcBuffer[srcBufCtr + 1];
		dstBufCtr++;
		srcBufCtr += 2;
	}
}

/*****************************************************************************************************************************************************
*****************************************************************************************************************************************************
	Description: 
	This method is called by the library when it requires reading data bytes from a communication port. If less than the requested number of 
	bytes could be read before timeout occurs, then the function should return TRUE and set pNoOfBytesRead to the actual number of bytes read. 
	If the number of bytes read is zero even after timeout occurs, then the function should return FALSE.
	Parameters: 
	a) networkNo (IN): 			A number identifying the �port� to be read.
	b) noOfBytesToRead (IN): 	The number of bytes to read on this port.
	c) pNoOfBytesRead (OUT): 	A pointer to the variable that receives the actual number of bytes read.
	d) pBuffer (OUT): 			A pointer to the buffer that receives the data read from the port.
	e) pErrorCode (OUT): 		A pointer to the variable that receives an error code in case of failure of this function.
	Returns: 
	TRUE if the function succeeds, else FALSE. If the return value is FALSE, then an error code indicating 
	the reason for failure should be stored in the pErrorCode parameter.
******************************************************************************************************************************************************
*****************************************************************************************************************************************************/

unsigned char  ReadPort(short int noOfBytesToRead , unsigned char  *pBuffer)
{
	int rcv_ctr=0;
	unsigned int wait_ctr = 0;
	
	mbrespctr=0;
	/*************************************************************************
	1.wait_ctr BASICALY USED TO TIME OUT IF WITH IN THE TIME PERIOD RECEIVER 
	IS NOT ABLE TO READ THE DATA ON RECEIVER LINE OF UART 
	2. mbrespctr<noOfBytesToRead ALWAYS TRUE for ex: IN OUR CASE WE ARE READING ONLY TWO BYTES 0<2 
	3. SO AFTER WAIT_CTR OVERFLOW IT WILL COME OUT 
	***************************************************************************/
      
    printf("\r\nModbus is Wating for this no of Bytes From Slave %d",(noOfBytesToRead));
    clock_delay_usec(25000);
    while(wait_ctr<0x7FFFFFFF)
	{
		wait_ctr++;
	}

	wait_ctr = 0;
    while(wait_ctr<0x7FFFFFFF)
	{
		wait_ctr++;
	}
  
	wait_ctr = 0;
    while(wait_ctr<0x7FFFFFFF)
	{
		wait_ctr++;
	}  
  
   	do{
		printf("\r\nRx has received the Bytes 0x%x",UART_1_RxBuffer_u8[rcv_ctr]);
		*(pBuffer)=UART_1_RxBuffer_u8[rcv_ctr];
		pBuffer++;
		rcv_ctr++;	
	}while((rcv_ctr<noOfBytesToRead));
	
	return MMPL_NO_ERROR;
}	   
		   



