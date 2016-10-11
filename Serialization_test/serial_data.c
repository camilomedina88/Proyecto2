#include "serial_data.h"

int serialize(char * buffer, datosTemp * d_temp, datosLuz * d_luz, comandos * d_comandos, indices index)
{
	int i;
	sprintf(buffer,"{\"indeces\":[%d,%d,%d],\"temp\":[", index.indTemp, index.indLuz, index.indCom);
	for(i=0; i < index.indTemp - 1; i++)
	{
		sprintf(buffer,"%s{\"v\":%4.2f,\"ts\":%li},", buffer, d_temp[i].datoTemperatura, (long int)d_temp[i].tiempo);
	}
	sprintf(buffer,"%s{\"v\":%4.2f,\"ts\":%li}],\"lux\":[" , buffer, d_temp[i].datoTemperatura, (long int)d_temp[i].tiempo);

	for(i=0; i < index.indLuz - 1; i++)
	{
		sprintf(buffer,"%s{\"v\":%4.2f,\"ts\":%li},", buffer, d_luz[i].datoLuz, (long int)d_luz[i].tiempo);
	}
	sprintf(buffer,"%s{\"v\":%4.2f,\"ts\":%li}],\"com\":[" , buffer, d_luz[i].datoLuz, (long int)d_luz[i].tiempo);

	for(i=0; i < index.indCom - 1; i++)
	{
		sprintf(buffer,"%s{\"t\":%d,\"l\":%d,\"c\":%d,\"ts\":%li},", buffer, d_comandos[i].freqTemp, d_comandos[i].freqLuz, d_comandos[i].freqconsol , (long int)d_comandos[i].tiempo);
	}
	sprintf(buffer,"%s{\"t\":%d,\"l\":%d,\"c\":%d,\"ts\":%li}]}" , buffer, d_comandos[i].freqTemp, d_comandos[i].freqLuz, d_comandos[i].freqconsol , (long int)d_comandos[i].tiempo);

	printf("%s",buffer);
}

int deserialize(char * buffer, datosTemp * d_temp, datosLuz * d_luz, comandos * d_comandos, indices * index)
{
	int offset=0;
	long  int pos = 0;
	int i = 0;
	int result;
	long int time_aux = 0;
	float temp_aux = 0.0;
	float temp_lux = 0.0;
	
	//indices
	result = sscanf(buffer,"{\"indeces\":[%d,%d,%d],\"temp\":[%n", &(index->indTemp), &(index->indLuz), &(index->indCom), &offset);
	if(result != 3)
	{
		printf("Error serialización. indices\n");
		return -1;
	}
	pos += offset;
	printf("serializacion: %d,%d,%d, offset = %d, result = %d\n", index->indTemp, index->indLuz, index->indCom, offset, result);
	
	//temp
	if( (index->indTemp) > 1)
	{
		for(i=0; i < ((index->indTemp) - 1); i++)
		{
			result = sscanf(buffer + pos,"{\"v\":%f,\"ts\":%i},%n", &temp_aux, &time_aux, &offset);
			if(result != 2)
			{
				printf("Error serialización. temp\n");
				return -1;
			}
			d_temp[i].datoTemperatura = temp_aux;
			d_temp[i].tiempo = (time_t)time_aux;
			pos += offset;
		
			#ifdef DEBUG
				printf("temp: %f , %li, offset = %d, pos= %d\n", (d_temp[i].datoTemperatura), (long int)d_temp[i].tiempo, pos);
			#endif
		}	
	}

	result = sscanf(buffer + pos,"{\"v\":%f,\"ts\":%i}],\"lux\":[%n", &(d_temp[i].datoTemperatura), &time_aux, &offset);
	if(result != 2)
	{
		printf("Error serialización. temp\n");
		return -1;
	}
	pos += offset;
	d_temp[i].tiempo = (time_t)time_aux;
	#ifdef DEBUG
		printf("temp: %f , %li, offset = %d, pos = %d\n", d_temp[i].datoTemperatura, (long int)d_temp[i].tiempo, pos);
	#endif

	//Luz
	if( (index->indLuz) > 1)
	{
		for(i=0; i < ((index->indLuz) - 1); i++)
		{
			result = sscanf(buffer + pos,"{\"v\":%f,\"ts\":%i},%n", &(d_luz[i].datoLuz), &time_aux, &offset);
			if(result != 2)
			{
				printf("Error serialización. Luz\n");
				return -1;
			}
			d_luz[i].tiempo = (time_t)time_aux;
			pos += offset;
		
			#ifdef DEBUG
				printf("luz: %f , %li, offset = %d, result = %d\n", d_luz[i].datoLuz, (long int)d_luz[i].tiempo, pos);
			#endif
		}	
	}

	result = sscanf(buffer + pos,"{\"v\":%f,\"ts\":%i}],\"com\":[%n", &(d_luz[i].datoLuz), &time_aux, &offset);
	if(result != 2)
	{
		printf("Error serialización. Luz\n");
		return -1;
	}
	d_luz[i].tiempo = (time_t)time_aux;
	pos += offset;
	#ifdef DEBUG
		printf("luz: %f , %li, offset = %d, result = %d\n", d_luz[i].datoLuz, (long int)d_luz[i].tiempo, pos);
	#endif

	//Commands

	if( (index->indCom) > 1)
	{
	for(i=0; i < ((index->indCom) - 1); i++)
		{
			result = sscanf(buffer + pos,"{\"t\":%d,\"l\":%d,\"c\":%d,\"ts\":%li},%n", &d_comandos[i].freqTemp, &d_comandos[i].freqLuz, &d_comandos[i].freqconsol, &time_aux, &offset);
			if(result != 4)
			{
				printf("Error serialización. commanods\n");
				return -1;
			}
			d_comandos[i].tiempo = (time_t)time_aux;
			pos += offset;
		
			#ifdef DEBUG
				printf("Commandos: %d , %d, %d, %d, offset = %d\n", d_comandos[i].freqTemp, d_comandos[i].freqLuz, d_comandos[i].freqconsol, (long int)d_comandos[i].tiempo, pos);
			#endif
		}	
	}

	result = sscanf(buffer + pos,"{\"t\":%d,\"l\":%d,\"c\":%d,\"ts\":%li}]}%n", &d_comandos[i].freqTemp, &d_comandos[i].freqLuz, &d_comandos[i].freqconsol, &time_aux, &offset);
	if(result != 4)
	{
		printf("Error serialización. commandos\n");
		return -1;
	}
	d_comandos[i].tiempo = (time_t)time_aux;
	pos += offset;
	#ifdef DEBUG
		printf("Commandos: %d , %d, %d, %d, offset = %d\n", d_comandos[i].freqTemp, d_comandos[i].freqLuz, d_comandos[i].freqconsol, (long int)d_comandos[i].tiempo, pos);
	#endif

	return 1;
}