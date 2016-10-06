//============================================================================
// Name : TestSensores.cpp
// Author : CAMM NEVS
// Version :
// Copyright : Your copyright notice
// Description :
//============================================================================
#include "mraa.hpp"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <sstream>
#define size 1024
using namespace std;
typedef struct sensorLuz {
	int IDmemory;
	mraa::Aio* pinLuz;
} sensorLuz;
typedef struct sensorTemp {
	int IDmemory;
	mraa::Aio* pinTemp;
} sensorTemp;
typedef struct infoUsuario {
	int IDmemoryLuz;
	int IDmemoryTemp;
	int IDmemoryConso;
} infoUsuario;
typedef struct datosluz {
	time_t tiempo;
	float datoLuz;
} datosLuz;
typedef struct datosTemp {
	time_t tiempo;
	float datoTemperatura;
} datosTemp;
typedef struct indices {
	int indLuz, indTemp, indCom;
} indices;
typedef struct comandos {
	time_t tiempo;
	int freqTemp, freqLuz, freqconsol;
} comandos;
int shmidtempdatos;
int shmidluzdatos;
int shmidindices;
int shmidcomandos;
pthread_mutex_t mutex_shidtempdatos = PTHREAD_MUTEX_INITIALIZER; //Mutex Buffer Datos Temperatura
pthread_mutex_t mutex_shidluzdatos = PTHREAD_MUTEX_INITIALIZER; // Mutex buffer datos luz
pthread_mutex_t mutex_shidindices = PTHREAD_MUTEX_INITIALIZER; //Mutex indices
pthread_mutex_t mutex_shidluz = PTHREAD_MUTEX_INITIALIZER; //Mutex frecuencia luz
pthread_mutex_t mutex_shidtemp = PTHREAD_MUTEX_INITIALIZER; // Mutex frecuencia temperatura
pthread_mutex_t mutex_shidconso = PTHREAD_MUTEX_INITIALIZER; //Mutex Frecuencia Consolidacion
pthread_mutex_t mutex_shidcomandos = PTHREAD_MUTEX_INITIALIZER; //Mutex Buffer Comandos
void *consolidacion(void *arg) {
	char* shared_memory_conso;
	int freqMuestras = 12;

	//Buffer Temperatura
	datosTemp* buffer_Temp;
	buffer_Temp = (datosTemp*) shmat(shmidtempdatos, 0, 0);

	//Buffer Comandos
	comandos* buffer_comandos;
	buffer_comandos = (comandos*) shmat(shmidcomandos, 0, 0);

	//Indices
	indices *indicestodos;
	indicestodos = (indices*) shmat(shmidindices, 0, 0);
	//Frecuencias
	infoUsuario *frecuencias = (infoUsuario *) arg;
	int IDFreqMuestras = frecuencias->IDmemoryConso;
	shared_memory_conso = (char*) shmat(IDFreqMuestras, 0, 0);

	//Buffer Luz
	datosLuz* buffer_luz;
	buffer_luz = (datosLuz*) shmat(shmidluzdatos, 0, 0);

	datosLuz* buffLuzActual;
	datosTemp* buffTempActual;
	comandos* bufferComActual;
	int indiceLuz,indiceTemp, indiceCom;


	for (;;) {
		//cout<<"Frecuencia Muestreo Actual: "<<freqMuestras<<endl;
		sleep(freqMuestras);
		cout<<"CONSOLIDACION"<<endl;
		pthread_mutex_lock(&mutex_shidindices);
		indiceLuz=indicestodos->indLuz;
		indiceTemp=indicestodos->indTemp;
		indiceCom=indicestodos->indCom;

		pthread_mutex_lock(&mutex_shidluzdatos);
		buffLuzActual=buffer_luz;
		pthread_mutex_unlock(&mutex_shidluzdatos);

		pthread_mutex_lock(&mutex_shidtempdatos);
		buffTempActual=buffer_Temp;
		pthread_mutex_unlock(&mutex_shidtempdatos);

		pthread_mutex_lock(&mutex_shidcomandos);
		bufferComActual=buffer_comandos;
		pthread_mutex_unlock(&mutex_shidcomandos);


		/*
		cout<<"VOY A MOSTRAR LOS DATOS DE LUZ"<<endl;
		for(int i=1;i<=indiceLuz;i++){
			cout<<"indice: "<<i<<" Luz: "<<buffLuzActual[i].datoLuz<<endl;

		}*/

		/* OPCION 1
		pthread_mutex_lock(&mutex_shidluzdatos);
		memcpy ( &buffLuzActual, &buffer_luz, sizeof(datosLuz)*indiceLuz);
		pthread_mutex_unlock(&mutex_shidluzdatos);

		pthread_mutex_lock(&mutex_shidtempdatos);
		memcpy ( &buffTempActual, &buffer_Temp, sizeof(datosTemp)*indiceTemp);
		pthread_mutex_unlock(&mutex_shidtempdatos);

		pthread_mutex_lock(&mutex_shidcomandos);
		memcpy ( &bufferComActual, &buffer_comandos, sizeof(comandos)*indiceCom);
		pthread_mutex_unlock(&mutex_shidcomandos);
		 */

		indicestodos->indLuz =0;
		indicestodos->indCom =0;
		indicestodos->indTemp=0;
		pthread_mutex_unlock(&mutex_shidindices);


		pthread_mutex_lock(&mutex_shidconso); //Mutex shmidconso lock
		sscanf(shared_memory_conso, "%d", &freqMuestras);
		pthread_mutex_unlock(&mutex_shidconso); //Mutex shmidconso lock
	}
	return NULL;
}
void *usuario(void *arg) {
	infoUsuario *luz = (infoUsuario *) arg;
	comandos* buffer_comandos;
	comandos comandoActual;
	buffer_comandos = (comandos*) shmat(shmidcomandos, 0, 0);
	int tempfreqdefault;
	int luzfreqdefault;
	int consofreqdefault;
	char* shared_memory_luz;
	char* shared_memory_temp;
	char* shared_memory_conso;
	int identificadorLuz = luz->IDmemoryLuz;
	shared_memory_luz = (char*) shmat(identificadorLuz, 0, 0);
//printf("USR hnmshared memory attached at address %p\n", shared_memory_luz);
	int identificadorTemp = luz->IDmemoryTemp;
	shared_memory_temp = (char*) shmat(identificadorTemp, 0, 0);
//printf("USR hnmshared memory attached at address %p\n", shared_memory_temp);
	int identificadorConso = luz->IDmemoryConso;
	shared_memory_conso = (char*) shmat(identificadorConso, 0, 0);
//printf("USR hnmshared memory attached at address %p\n", shared_memory_temp);
	indices *indicecomando;
	indicecomando = (indices*) shmat(shmidindices, 0, 0);
	pthread_mutex_lock(&mutex_shidindices);
	indicecomando->indCom = 0;
	pthread_mutex_unlock(&mutex_shidindices);
	for (;;) {
		time_t tiempo = time(0);
		cout << "INGRESE FRECUENCIA LUZ: ";
		scanf("%d", &luzfreqdefault);
		pthread_mutex_lock(&mutex_shidluz);
		sprintf(shared_memory_luz, "%d", luzfreqdefault);
		printf("%s\n", shared_memory_luz);
		pthread_mutex_unlock(&mutex_shidluz);
		comandoActual.freqLuz = luzfreqdefault;
		pthread_mutex_lock(&mutex_shidindices);
		pthread_mutex_lock(&mutex_shidcomandos);
		buffer_comandos[indicecomando->indCom] = comandoActual;
		pthread_mutex_unlock(&mutex_shidcomandos);
		indicecomando->indLuz += 1;
		pthread_mutex_unlock(&mutex_shidindices);
		cout << "INGRESE FRECUENCIA TEMPERATURA: ";
		scanf("%d", &tempfreqdefault);
		pthread_mutex_lock(&mutex_shidtemp);
		sprintf(shared_memory_temp, "%d", tempfreqdefault);
		printf("%s\n", shared_memory_temp);
		pthread_mutex_unlock(&mutex_shidtemp);
		comandoActual.freqTemp = tempfreqdefault;
		pthread_mutex_lock(&mutex_shidindices);
		pthread_mutex_lock(&mutex_shidcomandos);
		buffer_comandos[indicecomando->indTemp] = comandoActual;
		pthread_mutex_unlock(&mutex_shidcomandos);
		indicecomando->indTemp += 1;
		pthread_mutex_unlock(&mutex_shidindices);
		cout << "INGRESE FRECUENCUA CONSOLIDACION:";
		scanf("%d", &consofreqdefault);
		pthread_mutex_lock(&mutex_shidconso);
		sprintf(shared_memory_conso, "%d", consofreqdefault);
		printf("%s\n", shared_memory_conso);
		pthread_mutex_unlock(&mutex_shidconso);
		comandoActual.freqconsol = consofreqdefault;
		comandoActual.tiempo = tiempo;
		pthread_mutex_lock(&mutex_shidindices);
		pthread_mutex_lock(&mutex_shidcomandos);
		buffer_comandos[indicecomando->indCom] = comandoActual;
		pthread_mutex_unlock(&mutex_shidcomandos);
		indicecomando->indCom += 1;
		pthread_mutex_unlock(&mutex_shidindices);
	}
	return NULL;
}
void *luz(void *parametros) {
	sensorLuz *luz = (sensorLuz *) parametros;
	mraa::Aio* a_pin2 = luz->pinLuz;
	int IDmemory = luz->IDmemory;
	char* shared_memory_luz;
	datosLuz* buffer_luz;
	int frecuenciaLuz = 10;
	shared_memory_luz = (char*) shmat(IDmemory, 0, 0);
//printf("LUZ hnmshared memory attached at address %p\n", shared_memory_luz);
	int indiceactual = 0;
	datosLuz datoactual;
	indices *indiceluz;
	buffer_luz = (datosLuz*) shmat(shmidluzdatos, 0, 0);
	indiceluz = (indices*) shmat(shmidindices, 0, 0);
//pthread_mutex_lock(&mutex_shidindices);
	indiceluz->indLuz = 0;
//pthread_mutex_unlock(&mutex_shidindices);
	for (;;) {
		pthread_mutex_lock(&mutex_shidluz);
		sscanf(shared_memory_luz, "%d", &frecuenciaLuz);
		pthread_mutex_unlock(&mutex_shidluz);
		time_t tiempo = time(0);
		struct tm * actual = localtime(&tiempo);
		uint16_t pin_value = a_pin2->read();
		float Rsensor;
		Rsensor = (float) (1023 - pin_value) * 10 / pin_value;
std::cout  << "Luz: " << actual->tm_mon + 1 << "/" << actual->tm_mday

				<< "/" << actual->tm_hour << ":" << actual->tm_min << ": "
				<< Rsensor << std::endl;
		datoactual.datoLuz = Rsensor;
		datoactual.tiempo = tiempo;
		pthread_mutex_lock(&mutex_shidluzdatos); //Mutex Buffer Datos Luz Lock
		pthread_mutex_lock(&mutex_shidindices);
		buffer_luz[indiceluz->indLuz] = datoactual;
		indiceluz->indLuz += 1;
		pthread_mutex_unlock(&mutex_shidindices);
		pthread_mutex_unlock(&mutex_shidluzdatos); //Mutex Buffer Datos luz unLock
		sleep(frecuenciaLuz);
	}
	return NULL;
}
void *temperatura(void *parametros) {
	sensorTemp *temp = (sensorTemp *) parametros;
	mraa::Aio* a_pin_tempe = temp->pinTemp;
	int IDmemory = temp->IDmemory;
	char* shared_memory_temp;
	int frecuenciaTemp = 10;
	datosTemp* buffer_Temp;
	datosTemp datoActual;
	indices *indicetemp;
	shared_memory_temp = (char*) shmat(IDmemory, 0, 0);
//printf("UJUUUU AQUI VOYnmshared memory attached at address %p\n",shared_memory_temp);
	buffer_Temp = (datosTemp*) shmat(shmidtempdatos, 0, 0);
	indicetemp = (indices*) shmat(shmidindices, 0, 0);
	pthread_mutex_lock(&mutex_shidindices);
	indicetemp->indTemp = 0;
	pthread_mutex_unlock(&mutex_shidindices);
	for (;;) {
		pthread_mutex_lock(&mutex_shidtemp);
		sscanf(shared_memory_temp, "%d", &frecuenciaTemp);
		pthread_mutex_unlock(&mutex_shidtemp);
		uint16_t pin_value = a_pin_tempe->read();
		float R = 1023.0 / ((float) pin_value) - 1.0;
		R = 100000.0 * R;
		float temperature = 1.0 / (log(R / 100000.0) / 4275 + 1 / 298.15)
				- 273.15;
		time_t tiempo = time(0);
		struct tm * actual = localtime(&tiempo);
std::cout  << "Tem: " << actual->tm_mon + 1 << "/" << actual->tm_mday

				<< "/" << actual->tm_hour << ":" << actual->tm_min << ": "
				<< temperature << std::endl;
		datoActual.datoTemperatura = temperature;
		datoActual.tiempo = tiempo;
		pthread_mutex_lock(&mutex_shidtempdatos); //Mutex Buffer Datos Temp Lock
		pthread_mutex_lock(&mutex_shidindices);
		buffer_Temp[indicetemp->indTemp] = datoActual;
		indicetemp->indTemp += 1;
		pthread_mutex_unlock(&mutex_shidindices);
		pthread_mutex_unlock(&mutex_shidtempdatos); //Mutex Buffer Datos Temp Unlock
//mutex
		sleep(frecuenciaTemp);
	}
	return NULL;
}
// ----------------------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------//
int main() {
	mraa_platform_t platform = mraa_get_platform_type();
	if ((platform != MRAA_INTEL_GALILEO_GEN1)
			&& (platform != MRAA_INTEL_GALILEO_GEN2)
			&& (platform != MRAA_INTEL_EDISON_FAB_C)) {
		std::cerr << "Unsupported platform, exiting" << std::endl;
		return MRAA_ERROR_INVALID_PLATFORM;
	}
// Creacion Instancia sensor Luz
	mraa::Aio* a_pin_luz = new mraa::Aio(0);
	if (a_pin_luz == NULL) {
		std::cerr << "Can't create mraa::Aio object, exiting" << std::endl;
		return MRAA_ERROR_UNSPECIFIED;
	}
// Creacion sensor temperatura
	mraa::Aio* a_pin_tmp = new mraa::Aio(1);
	if (a_pin_tmp == NULL) {
		std::cerr << "Can't create mraa::Aio object, exiting" << std::endl;
		return MRAA_ERROR_UNSPECIFIED;
	}
	int shmidluz; //shared memory ID LUZ Frecuencia
	if ((shmidluz = shmget(IPC_PRIVATE, size,
	IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) {
		perror("shmget");
		cout << "Esta Jodido en luz" << endl;
		exit(1);
	}
	int shmidtemp; //shared memory ID Temp
	if ((shmidtemp = shmget(IPC_PRIVATE, size,
	IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) {
		perror("shmget");
		cout << "Esta Jodido en temperatura" << endl;
		exit(1);
	}
	int shmidconso; //shared memory ID Freq Consolidacion
	if ((shmidconso = shmget(IPC_PRIVATE, size,
	IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) {
		perror("shmget");
		cout << "Esta Jodido en consolidacion" << endl;
		exit(1);
	}
//shared memory ID Datos Luz
	if ((shmidluzdatos = shmget(IPC_PRIVATE, sizeof(datosLuz),
	IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) {
		perror("shmget");
		cout << "Esta Jodido en datos Luz" << endl;
		exit(1);
	}
//shared memory ID Datos Temperatura
	if ((shmidtempdatos = shmget(IPC_PRIVATE, sizeof(datosTemp),
	IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) {
		perror("shmget");
		cout << "Esta Jodido en datos Temperatura" << endl;
		exit(1);
	}
//shared memory ID Indices
	if ((shmidindices = shmget(IPC_PRIVATE, sizeof(indices),
	IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) {
		perror("shmget");
		cout << "Esta Jodido en Indices" << endl;
		exit(1);
	}
//shared memory ID historialComandos
	if ((shmidcomandos = shmget(IPC_PRIVATE, sizeof(indices),
	IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) {
		perror("shmget");
		cout << "Esta Jodido en buffer comandos" << endl;
		exit(1);
	}
//Instancias de Estructuras
	sensorLuz luz1;
	sensorTemp temp1;
	infoUsuario usuario1;
	luz1.IDmemory = shmidluz;
	luz1.pinLuz = a_pin_luz;
	temp1.IDmemory = shmidtemp;
	temp1.pinTemp = a_pin_tmp;
	usuario1.IDmemoryLuz = shmidluz;
	usuario1.IDmemoryTemp = shmidtemp;
	usuario1.IDmemoryConso = shmidconso;
//Creacion Hilos
	pthread_t HiloLuz, HiloTmp, HiloUsr, HiloConso;
	pthread_create(&HiloUsr, NULL, usuario, (void *) &usuario1);
	pthread_create(&HiloLuz, NULL, luz, (void *) &luz1);
	pthread_create(&HiloTmp, NULL, temperatura, (void *) &temp1);
	pthread_create(&HiloConso, NULL, consolidacion, (void *) &usuario1);
	pthread_join(HiloUsr, NULL);
	pthread_join(HiloLuz, NULL);
	pthread_join(HiloTmp, NULL);
	pthread_join(HiloConso, NULL);
	printf("Fin\n");
}
