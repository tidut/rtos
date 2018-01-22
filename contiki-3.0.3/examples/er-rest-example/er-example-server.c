/* *******************************************************
 * *******************************************************
 * File Name 		: 	er-example-server.c
 * Description 	:		Mainly a resource file
 * Written by 	: 
 * *******************************************************
 * *******************************************************/
#include "er-example-server.h"
/**********************************************************************************
													Global variables
**********************************************************************************/
unsigned char	Energy_Command_u8 = MDB_VOLTAGE_R_ADDRESS_WRITE;

unsigned char UART_1_RxBuffer_u8[100];
unsigned char UART_1_BufferIndex_u8 = 0;

extern unsigned int Current_Value_u32;
extern unsigned int Battery_Vout_inMv;

/**********************************************************************************
							Function Dclaration
**********************************************************************************/

static void Get_Volatge_FromEnergyMeter(void *ptr);
unsigned int UART_1_RX_Callback (unsigned char uByte_u8);

/**********************************************************************************
							Structure Variable declaration
**********************************************************************************/

_FLAGS FLAGS;
static struct ctimer ct_timer;
static struct etimer et_timer;

/**********************************************************************************
							Process declaration
**********************************************************************************/

PROCESS(er_example_server, "Erbium Example Server");
PROCESS(UART1_transmit_receive_process, "UART Tx_Rx process");
AUTOSTART_PROCESSES(&er_example_server,&UART1_transmit_receive_process);


/**********************************************************************************
	* Thread Name 		: 	er_example_server
	* Description		:	Tirgger all nodes
**********************************************************************************/

void Add_Sensors(void)
{
	/**********************************************************************************
					rest_activate_resources activates here
	**********************************************************************************/
	#ifdef EN_4TO20
		rest_activate_resource(&res_current, "Sensor/Current");
		rest_activate_resource(&res_FS, "Test/Filter");
		PRINTF("\r\nCurrent Sensor activated ");
	#elif defined TEMP_P1_P2
		rest_activate_resource(&res_pot1, "Sensor/Pot_1");
		rest_activate_resource(&res_pot2, "Sensor/Pot_2");
		rest_activate_resource(&res_lm61, "Sensor/Temperature");	
		rest_activate_resource(&res_DFMS, "Test/DFMS");
		rest_activate_resource(&res_FS,   "Test/Filter");
		PRINTF("\r\n Temparature + POT Sensor activated");
	#elif defined  EN_ENERGY_METER
		rest_activate_resource(&res_volt_energy,"Sensor/Em_Voltage");
		rest_activate_resource(&res_curr_energy,"Sensor/Em_Current");
		rest_activate_resource(&res_pf_energy, "Sensor/Em_PF");
		rest_activate_resource(&res_freq_energy,"Sensor/Em_Freq");
		rest_activate_resource(&res_wph_energy, "Sensor/Em_WPH");		
		PRINTF("\r\n Energy Sensor activated");
	#elif defined  AIR_QUALITY
		rest_activate_resource(&res_Air_Quality,"Sensor/Air_Qu");		
		PRINTF("\r\n AIR Sensor activated");	
	#elif defined  RELAYS_SHIELD		
		PRINTF("\r\n RELAYS_SHIELD activated");
		rest_activate_resource(&res_relay_1_On, "actuators/Relay1_On");
		rest_activate_resource(&res_relay_1_Off, "actuators/Relay1_Off");
		rest_activate_resource(&res_relay_2_On, "actuators/Relay2_On");
		rest_activate_resource(&res_relay_2_Off, "actuators/Relay2_Off");
	#elif defined  THERMO_COUPLE
		PRINTF("\r\n Thermocouple Sensor activated");
		rest_activate_resource(&res_thermocouple, "actuators/Temprature");
	#endif
	
	rest_activate_resource(&res_toggle_led1, "actuators/Toggle_led_PC0");
	rest_activate_resource(&res_toggle_led2, "actuators/Toggle_led_PC1");
	rest_activate_resource(&res_diag, "Diagnostics");	
	rest_activate_resource(&res_Help1, "Help/H1");
	rest_activate_resource(&res_Help2, "Help/H2");	
	
	/*********************************************************/

	Init_I2c();
	
	#ifdef TEMP_P1_P2
		Max17043_I2c_Init();
		BatAlert_Config();
	#endif
}

volatile int Var=0;
PROCESS_THREAD(er_example_server, ev, data)
{
	unsigned char d[6]={0};
	
	PROCESS_BEGIN();
	PROCESS_PAUSE();

	PRINTF("\r\nStarting Erbium Example Server");

	#ifdef RF_CHANNEL
		PRINTF("\r\nRF channel: %u", RF_CHANNEL);
	#endif
	
	#ifdef IEEE802154_PANID
		PRINTF("\r\n PAN ID: 0x%04X", IEEE802154_PANID);
	#endif

	PRINTF("\r\n uIP buffer: %u", UIP_BUFSIZE);
	PRINTF("\r\n LL header: %u", UIP_LLH_LEN);
	PRINTF("\r\n IP+UDP header: %u", UIP_IPUDPH_LEN);
	PRINTF("\r\n REST max chunk: %u", REST_MAX_CHUNK_SIZE);

	RELAY_1_SELECT();
	RELAY_1_MAKE_OUTPUT();
	
	RELAY_2_SELECT();
	RELAY_2_MAKE_OUTPUT();
	
	SPI_1_SELECT();
	SPI_1_MAKE_OUTPUT();
	
	/* Initialize the REST engine. */
	rest_init_engine();

	/*
	* Bind the resources to their Uri-Path.
	* WARNING: Activating twice only means alternate path, not two instances!
	* All static variables are the same for each URI path.
	*/
	//SENSORS_ACTIVATE(button_sensor);

	Add_Sensors();
#if 0
	SPI_1_ON();		
	SPI_FLUSH();	
  	printf("\r\n SPI Tx done ");    
	SPI_READ(d[0]);	
	printf("\r\nDigi 0 0x%x",d[0]);
	SPI_READ(d[1]);
	printf("\r\nDigi 1 0x%x",d[1]);
	SPI_READ(d[2]);
	printf("\r\nDigi 2 0x%x",d[2]);
	SPI_READ(d[3]);
	printf("\r\nDigi 3 0x%x",d[3]);
	SPI_1_OFF();
#endif

	FLAGS.MBResponseRxd = 0;
	/* Set the event timer */	
	etimer_set(&et_timer, CLOCK_SECOND * TOGGLE_INTERVAL );
	
		
	/* Define application-specific events here. */
	while(1)
	{
		PROCESS_WAIT_EVENT();
		if(ev == PROCESS_EVENT_TIMER) 
		{	
			printf("\r\n----------------------------------");	
			printf("\r\nVersion %s",VERSION_NAME);
			printf("\r\nCompile Time of this code %s",COMPILE_TIME);			
			printf("\r\n ----------------------------------");
			
			#ifdef EN_4TO20
				Call_Current_Sensor();
			#elif defined TEMP_P1_P2	
				Call_Pot_Temp_Sensor();
			#elif defined  EN_ENERGY_METER
				Call_Energy_Sensor();
			#elif defined  AIR_QUALITY
				Call_Air_Sensor();	
			#elif defined  RELAYS_SHIELD
				Call_Relays_Sensor();
			#elif defined  THERMO_COUPLE
				Call_Thermocouple_Sensor();
			#endif	
#if 0
		SPI_1_ON();		
		SPI_FLUSH();	
		printf("\r\n SPI Tx done ");    
		SPI_READ(d[0]);	
		printf("\r\n");
		printf("\r\nDigi 0 0x%x",d[0]);
		SPI_READ(d[1]);
		printf("\r\nDigi 1 0x%x",d[1]);
		SPI_READ(d[2]);
		printf("\r\nDigi 2 0x%x",d[2]);
		SPI_READ(d[3]);
		printf("\r\nDigi 3 0x%x",d[3]);
		SPI_READ(d[4]);
		printf("\r\nDigi 4 0x%x",d[4]);
			printf("\r\n");
				printf("\r\n");
		SPI_1_OFF();
#endif
			// Reset the event timer again			
			etimer_set(&et_timer, CLOCK_SECOND * TOGGLE_INTERVAL );
		}	
	}       
	PROCESS_END();
}

/**********************************************************************************
	* Thread Name 		: 		UART1_transmit_receive_process
	* Description		:		Activates the timer for send command over uart
**********************************************************************************/

PROCESS_THREAD(UART1_transmit_receive_process, ev, data)
{
	PROCESS_BEGIN();
	RS485_TRANSMIT_SELECT();
	RS485_TRANSMIT_MAKE_OUTPUT();
	
	#if defined  EN_ENERGY_METER
		printf("\r\nExecuting Energy meter Thread");
		ctimer_set(&ct_timer, 15 * CLOCK_SECOND, Get_Volatge_FromEnergyMeter, (void *)NULL);
		PROCESS_WAIT_UNTIL(ctimer_expired(&ct_timer));	
	#endif	
	PROCESS_END();
}


/**********************************************************************************
	* Function Name 	: 	UART_1_Tx_Bytes
	* Description		:	Transmits Data over UART 1
**********************************************************************************/

unsigned int UART_1_Tx_Bytes( unsigned char *Data_ptr, unsigned int Data_Len_u32)
{
	unsigned char Index_u8 = 0;

	printf("\r\nset pin");

	/* This for loop transmits data over UART 1*/ 
	for(Index_u8 = 0;Index_u8 < Data_Len_u32;Index_u8++)
	{
		clock_delay_usec(10000);		
		/*Set the pin high to transmit data over modbus*/
		RS485_TRANSMIT_SET();
		/*Send data over UART*/		
		printf("\r\nUart Transmit byte 0x%x",*Data_ptr);
		uart_write_byte(1,*Data_ptr++);	
		clock_delay_usec(25000);
		/*Reset the pin low to stop transmit data over modbus*/
		RS485_TRANSMIT_CLEAR();
	}	
	printf("\r\nclr pin");
	return Index_u8;
}

/**********************************************************************************
	* Function Name 	: UART_1_RX_Callback
	* Description		:	UART interrupt Call Back
**********************************************************************************/

unsigned int UART_1_RX_Callback(unsigned char uByte_u8)
{
	UART_1_RxBuffer_u8[UART_1_BufferIndex_u8] = uByte_u8;
	printf("\r\nUART0 received 0x%x over UART1.........",UART_1_RxBuffer_u8[UART_1_BufferIndex_u8]);
	UART_1_BufferIndex_u8++;
	return 1;
}

/**********************************************************************************
	* Function Name 	: 	Get_Volatge_FromEnergyMeter
	* Description		:	Reads Voltage from Energy Meter
**********************************************************************************/

static void Get_Volatge_FromEnergyMeter(void *ptr)
{	
	#ifdef EN_ENERGY_METER
	
		unsigned char Index_u8;
		short int MdbResponse = 0;	
		unsigned char HoldingReg[10];
				
		uart_set_input(1,UART_1_RX_Callback);	
		
		switch (Energy_Command_u8)
		{				
			case MDB_VOLTAGE_R_ADDRESS_WRITE: 	
				UART_1_BufferIndex_u8 = 0;
				MdbResponse = Write_MDB_Command(SLAVE_ADDRESS_ENERGY,FC_READ_IP_REGS, VOLTAGE_R_ADDRESS, 0x0002, (unsigned char *)HoldingReg, 1);
				printf("\r\n Packet 1 sent>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>..");
				Energy_Command_u8 = Energy_Command_u8 + 1;
			break;
			case MDB_VOLTAGE_R_ADDRESS_READ:
				MdbResponse = MMPL_NO_ERROR;
				MdbResponse = Read_MDB_Response(SLAVE_ADDRESS_ENERGY,FC_READ_IP_REGS, VOLTAGE_R_ADDRESS, 0x0002, (unsigned char *)HoldingReg, 1);		
				if (MdbResponse == MMPL_NO_ERROR)
				{
					FLAGS.MBResponseRxd = 1;
					Do_Mdb_VOLTAGE_R_ADDRESS();
				}
				else
				{
					printf("\r\n AC VOLT READ MDB Error ");
					FLAGS.MBResponseRxd = 0;
				}	
				Energy_Command_u8 = Energy_Command_u8 + 1;
			break;


			case MDB_CURRENT_R_ADDRESS_WRITE: 	
				UART_1_BufferIndex_u8 = 0;
				MdbResponse = Write_MDB_Command(SLAVE_ADDRESS_ENERGY,FC_READ_IP_REGS, CURRENT_R_ADDRESS, 0x0002, (unsigned char *)HoldingReg, 1);
				printf("\r\n Packet 2 sent>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>..");
				Energy_Command_u8 = Energy_Command_u8 + 1;
			break;
			case MDB_CURRENT_R_ADDRESS_READ:	
				MdbResponse = MMPL_NO_ERROR;
				MdbResponse = Read_MDB_Response(SLAVE_ADDRESS_ENERGY,FC_READ_IP_REGS, CURRENT_R_ADDRESS, 0x0002, (unsigned char *)HoldingReg, 1);		
				if (MdbResponse == MMPL_NO_ERROR)
				{
					Do_Mdb_CURRENT_R_ADDRESS();
				}
				else
				{
					printf("\r\n AC current READ MDB Error ");
					FLAGS.MBResponseRxd = 0;
				}	
				Energy_Command_u8 = Energy_Command_u8 + 1;
			break;

			case MDB_PF_ADDRESS_WRITE: 	
				MdbResponse = Write_MDB_Command(SLAVE_ADDRESS_ENERGY,FC_READ_IP_REGS, PF_ADDRESS, 0x0002, (unsigned char *)HoldingReg, 1);
				Energy_Command_u8 = Energy_Command_u8 + 1;
				printf("\r\n Packet 3 sent>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>..");
			break;				
			case MDB_PF_ADDRESS_READ: 
				MdbResponse = MMPL_NO_ERROR;
				MdbResponse = Read_MDB_Response(SLAVE_ADDRESS_ENERGY,FC_READ_IP_REGS,PF_ADDRESS , 0x0002, (unsigned char *)HoldingReg, 1);		
				if (MdbResponse == MMPL_NO_ERROR)
				{
					FLAGS.MBResponseRxd = 1;
					Do_Mdb_PF_ADDRESS();
				}
				else
				{
					printf("\r\n AC PF READ MDB Error ");
					FLAGS.MBResponseRxd = 0;
				}		
				Energy_Command_u8 = Energy_Command_u8 + 1;	
			break;	

			case MDB_FREQUENCY_ADDRESS_WRITE:
			MdbResponse = Write_MDB_Command(SLAVE_ADDRESS_ENERGY,FC_READ_IP_REGS, FREQUENCY_ADDRESS, 0x0002, (unsigned char *)HoldingReg, 1);
				Energy_Command_u8 = Energy_Command_u8 + 1;
				printf("\r\n Packet 4 sent>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>..");
			break;
			case MDB_FREQUENCY_ADDRESS_READ: 
				MdbResponse = MMPL_NO_ERROR;
				MdbResponse = Read_MDB_Response(SLAVE_ADDRESS_ENERGY,FC_READ_IP_REGS, FREQUENCY_ADDRESS, 0x0002, (unsigned char *)HoldingReg, 1);		
				if (MdbResponse == MMPL_NO_ERROR)
				{
					FLAGS.MBResponseRxd = 1;
					Do_Mdb_FREQUENCY_ADDRESS();
				}
				else
				{
					printf("\r\n AC freq READ MDB Error ");
					FLAGS.MBResponseRxd = 0;
				}	
				Energy_Command_u8 = Energy_Command_u8 + 1;	
			break;

			case MDB_WATTS_PER_HR_ADDRESS_WRITE:
				MdbResponse = Write_MDB_Command(SLAVE_ADDRESS_ENERGY,WATTS_PER_HR_ADDRESS, FREQUENCY_ADDRESS, 0x0002, (unsigned char *)HoldingReg, 1);
				Energy_Command_u8 = Energy_Command_u8 + 1;
				printf("\r\n Packet 5 sent>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>..");
			break;
			case MDB_WATTS_PER_HR_ADDRESS_READ:
				MdbResponse = MMPL_NO_ERROR;
				MdbResponse = Read_MDB_Response(SLAVE_ADDRESS_ENERGY,FC_READ_IP_REGS,WATTS_PER_HR_ADDRESS , 0x0002, (unsigned char *)HoldingReg, 1);		
				if (MdbResponse == MMPL_NO_ERROR)
				{
					FLAGS.MBResponseRxd = 1;
					Do_Mdb_WATTS_PER_HR_ADDRESS();
				}
				else
				{
					printf("\r\n AC wph READ MDB Error ");
					FLAGS.MBResponseRxd = 0;
				}	
				Energy_Command_u8 = 0;
			break;

			
			default:
			break;
		}
		ctimer_reset(&ct_timer);		
	#endif
}





