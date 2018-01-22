#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"

#ifndef DIAG_SERVER_H_
#define DIAG_SERVER_H_

extern volatile unsigned char FS_buffer[100];

struct Rplinfo
{
};

struct Batterystatus
{
	unsigned char 	Batterylowindication;
	uint16_t 	Batterycapacity;
	uint16_t 	chargeConnectionstatus;
};
	 
struct DiagnosticParameters
{
	unsigned char Macid[10]; 
	unsigned char Ipv6address[32]; 
	uint16_t Rssi;
	struct Rplinfo info;
	struct Batterystatus Batinfo;
};
struct DiagnosticParameters dpars;


struct FilterService
{
    float LimitValue;
    unsigned char Flag;
    unsigned char Result;
    unsigned char FirstFilterRes;
    unsigned char SecondFilterRes;
};
struct FilterService FS;


typedef struct
{
    unsigned MBResponseRxd:1;
    unsigned UART_RxDone:1;
    unsigned TS_Result_outofrange:1;
    unsigned PS_Result_outofrange:1;

}_FLAGS;

extern _FLAGS FLAGS;
 
 #endif /* DIAG_SERVER_H_ */
