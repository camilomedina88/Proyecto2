#ifndef _SERIAL_DATA
#define _SERIAL_DATA

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct datosTemp {
	time_t tiempo;
	float datoTemperatura;
} datosTemp;

typedef struct datosluz {
	time_t tiempo;
	float datoLuz;
} datosLuz;

typedef struct comandos {
	time_t tiempo;
	int freqTemp, freqLuz, freqconsol;
} comandos;

typedef struct indices {
	int indLuz, indTemp, indCom;
} indices;

#define DEBUG

int serialize(char * buffer, datosTemp * d_temp, datosLuz * d_luz, comandos * d_comandos, indices index);
int deserialize(char * buffer, datosTemp * d_temp, datosLuz * d_luz, comandos * d_comandos, indices * index);

#endif