#ifndef MAX17043_H_
#define MAX17043_H_

//#define REGLEVEL_CONFIG
#define READ_BATTERY_VOLTAGE
#define ENABLE_DEBUG_UART

#define THRESHOLD_32
//#define THRESHOLD_10

#define TH10_VALUE 10
#define TH32_VALUE 32


#define MAX_I2C_WRITE_ADDR	0x36
#define MAX_I2C_READ_ADDR	0x37

#define NUM_I2C_DATA 2

/*********** MAX17043 Register Mapping******************************/

#define VCELL_MSB		0x02
#define VCELL_LSB		0x03
#define SOC_MSB			0x04
#define SOC_LSB			0x05
#define MODE_MSB		0x06
#define MODE_LSB		0x07
#define VERSION_MSB		0x08
#define VERSION_LSB		0x09
#define CONFIG_MSB		0x0C
#define CONFIG_LSB		0x0D
#define COMMAND_MSB		0xFE
#define COMMAND_LSB		0xFF
/********************************************************************/

extern unsigned int Battery_Vout_inMv;
extern volatile unsigned char Charger_Connection_Status;
extern volatile unsigned char Battery_Low_Alert_Flag;

/************* Function prototypes **************/

void delayus(void);
void Init_I2c(void);
void Read_Vcell(void);
void SetThreshold(void);
void ResetFuelGauge(void);
void SetBatalertFlag(void);
void PrintSOCforDebug(void);
void Max17043_I2c_Init(void);
void Clear_BatteryFlag(void);
void Max17043_I2c_Start(void);
void DelayMs(unsigned int msec);
void CheckChargerConnectionStatus(void);
void GPIOPortB_InterruptCallback(unsigned char port, unsigned char pin);

unsigned char ReadOnI2C(unsigned char);
unsigned int WriteOnI2C(unsigned char RegAddr , unsigned char Data_0,unsigned char Data_1);
/************************************************/

extern void BatAlert_Config(void);
extern void Read_SOC_Reg_and_CompareThreshold(void);
extern volatile unsigned char Battery_Low_Alert_Flag;

#endif /* MAX17043_H_ */
