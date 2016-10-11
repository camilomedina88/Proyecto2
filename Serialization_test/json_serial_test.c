#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "serial_data.h"

// int serialize(char * buffer, datosTemp * d_temp, datosLuz * d_luz, comandos * d_comandos, indices index)
// {
// 	int i;
// 	sprintf(buffer,"{\"indeces\":[%d,%d,%d],\"temp\":[", index.indTemp, index.indLuz, index.indCom);
// 	for(i=0; i < index.indTemp - 1; i++)
// 	{
// 		sprintf(buffer,"%s{\"v\":%4.2f,\"ts\":%li},", buffer, d_temp[i].datoTemperatura, (long int)d_temp[i].tiempo);
// 	}
// 	sprintf(buffer,"%s{\"v\":%4.2f,\"ts\":%li}],\"lux\":[" , buffer, d_temp[i].datoTemperatura, (long int)d_temp[i].tiempo);

// 	for(i=0; i < index.indLuz - 1; i++)
// 	{
// 		sprintf(buffer,"%s{\"v\":%4.2f,\"ts\":%li},", buffer, d_luz[i].datoLuz, (long int)d_luz[i].tiempo);
// 	}
// 	sprintf(buffer,"%s{\"v\":%4.2f,\"ts\":%li}],\"com\":[" , buffer, d_luz[i].datoLuz, (long int)d_luz[i].tiempo);

// 	for(i=0; i < index.indCom - 1; i++)
// 	{
// 		sprintf(buffer,"%s{\"t\":%d,\"l\":%d,\"c\":%d,\"ts\":%li},", buffer, d_comandos[i].freqTemp, d_comandos[i].freqLuz, d_comandos[i].freqconsol , (long int)d_comandos[i].tiempo);
// 	}
// 	sprintf(buffer,"%s{\"t\":%d,\"l\":%d,\"c\":%d,\"ts\":%li}]}" , buffer, d_comandos[i].freqTemp, d_comandos[i].freqLuz, d_comandos[i].freqconsol , (long int)d_comandos[i].tiempo);

// 	printf("%s",buffer);
// }


// int deserialize(char * buffer, datosTemp * d_temp, datosLuz * d_luz, comandos * d_comandos, indices * index)
// {
// 	int offset=0;
// 	long  int pos = 0;
// 	int i = 0;
// 	int result;
// 	long int time_aux = 0;
// 	float temp_aux = 0.0;
// 	float temp_lux = 0.0;
	
// 	//indices
// 	result = sscanf(buffer,"{\"indeces\":[%d,%d,%d],\"temp\":[%n", &(index->indTemp), &(index->indLuz), &(index->indCom), &offset);
// 	if(result != 3)
// 	{
// 		printf("Error serialización. indices\n");
// 		return -1;
// 	}
// 	pos += offset;
// 	printf("serializacion: %d,%d,%d, offset = %d, result = %d\n", index->indTemp, index->indLuz, index->indCom, offset, result);
	
// 	//temp
// 	if( (index->indTemp) > 1)
// 	{
// 		for(i=0; i < ((index->indTemp) - 1); i++)
// 		{
// 			result = sscanf(buffer + pos,"{\"v\":%f,\"ts\":%i},%n", &temp_aux, &time_aux, &offset);
// 			if(result != 2)
// 			{
// 				printf("Error serialización. temp\n");
// 				return -1;
// 			}
// 			d_temp[i].datoTemperatura = temp_aux;
// 			d_temp[i].tiempo = (time_t)time_aux;
// 			pos += offset;
		
// 			#ifdef DEBUG
// 				printf("temp: %f , %li, offset = %d, pos= %d\n", (d_temp[i].datoTemperatura), (long int)d_temp[i].tiempo, pos);
// 			#endif
// 		}	
// 	}

// 	result = sscanf(buffer + pos,"{\"v\":%f,\"ts\":%i}],\"lux\":[%n", &(d_temp[i].datoTemperatura), &time_aux, &offset);
// 	if(result != 2)
// 	{
// 		printf("Error serialización. temp\n");
// 		return -1;
// 	}
// 	pos += offset;
// 	d_temp[i].tiempo = (time_t)time_aux;
// 	#ifdef DEBUG
// 		printf("temp: %f , %li, offset = %d, pos = %d\n", d_temp[i].datoTemperatura, (long int)d_temp[i].tiempo, pos);
// 	#endif

// 	//Luz
// 	if( (index->indLuz) > 1)
// 	{
// 		for(i=0; i < ((index->indLuz) - 1); i++)
// 		{
// 			result = sscanf(buffer + pos,"{\"v\":%f,\"ts\":%i},%n", &(d_luz[i].datoLuz), &time_aux, &offset);
// 			if(result != 2)
// 			{
// 				printf("Error serialización. Luz\n");
// 				return -1;
// 			}
// 			d_luz[i].tiempo = (time_t)time_aux;
// 			pos += offset;
		
// 			#ifdef DEBUG
// 				printf("luz: %f , %li, offset = %d, result = %d\n", d_luz[i].datoLuz, (long int)d_luz[i].tiempo, pos);
// 			#endif
// 		}	
// 	}

// 	result = sscanf(buffer + pos,"{\"v\":%f,\"ts\":%i}],\"com\":[%n", &(d_luz[i].datoLuz), &time_aux, &offset);
// 	if(result != 2)
// 	{
// 		printf("Error serialización. Luz\n");
// 		return -1;
// 	}
// 	d_luz[i].tiempo = (time_t)time_aux;
// 	pos += offset;
// 	#ifdef DEBUG
// 		printf("luz: %f , %li, offset = %d, result = %d\n", d_luz[i].datoLuz, (long int)d_luz[i].tiempo, pos);
// 	#endif

// 	//Commands

// 	if( (index->indCom) > 1)
// 	{
// 	for(i=0; i < ((index->indCom) - 1); i++)
// 		{
// 			result = sscanf(buffer + pos,"{\"t\":%d,\"l\":%d,\"c\":%d,\"ts\":%li},%n", &d_comandos[i].freqTemp, &d_comandos[i].freqLuz, &d_comandos[i].freqconsol, &time_aux, &offset);
// 			if(result != 4)
// 			{
// 				printf("Error serialización. commanods\n");
// 				return -1;
// 			}
// 			d_comandos[i].tiempo = (time_t)time_aux;
// 			pos += offset;
		
// 			#ifdef DEBUG
// 				printf("Commandos: %d , %d, %d, %d, offset = %d\n", d_comandos[i].freqTemp, d_comandos[i].freqLuz, d_comandos[i].freqconsol, (long int)d_comandos[i].tiempo, pos);
// 			#endif
// 		}	
// 	}

// 	result = sscanf(buffer + pos,"{\"t\":%d,\"l\":%d,\"c\":%d,\"ts\":%li}]}%n", &d_comandos[i].freqTemp, &d_comandos[i].freqLuz, &d_comandos[i].freqconsol, &time_aux, &offset);
// 	if(result != 4)
// 	{
// 		printf("Error serialización. commandos\n");
// 		return -1;
// 	}
// 	d_comandos[i].tiempo = (time_t)time_aux;
// 	pos += offset;
// 	#ifdef DEBUG
// 		printf("Commandos: %d , %d, %d, %d, offset = %d\n", d_comandos[i].freqTemp, d_comandos[i].freqLuz, d_comandos[i].freqconsol, (long int)d_comandos[i].tiempo, pos);
// 	#endif

// 	return 1;
// }

#define N_TEMP 1
#define N_LUZ 1
#define N_COM 1
#define N_MAX 600

int main()
{
	datosLuz dLuz[N_TEMP];
	datosTemp dTemp[N_LUZ];
	comandos dcom[N_COM];
	indices index_datos;
	index_datos.indTemp = N_TEMP;
	index_datos.indLuz = N_LUZ;
	index_datos.indCom = N_COM;

	char * buffer_aux;

	int i;
	time_t tiempo = time(NULL);
	for(i=0;i<N_TEMP;i++)
	{
		dTemp[i].datoTemperatura = 1.23 + i;
		dTemp[i].tiempo = tiempo;
	}
	for(i=0;i<N_LUZ;i++)
	{
		dLuz[i].datoLuz = 3.21 + i;
		dLuz[i].tiempo = tiempo;
	}
	for(i=0;i<N_COM;i++)
	{
		dcom[i].freqTemp = 1 + i;
		dcom[i].freqLuz = 1 + i;
		dcom[i].freqconsol = 1 + i;
		dcom[i].tiempo = tiempo + i;
	}

	buffer_aux = (char*)malloc(sizeof(char)*100*(N_TEMP+N_COM+N_LUZ));

	serialize(buffer_aux, dTemp, dLuz, dcom, index_datos);
	

	printf("\n\n\n");
	// on the other side
	datosLuz dLuz2[N_MAX];
	datosTemp dTemp2[N_MAX];
	comandos dcom2[N_MAX];
	indices index_datos2;
	deserialize(buffer_aux, dTemp2, dLuz2, dcom2, &index_datos2);

	free(buffer_aux);
	return 0;
}