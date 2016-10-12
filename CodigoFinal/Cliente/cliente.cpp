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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>

//Librerias para WebSockets

#include <signal.h>
#include <libwebsockets.h>

#define KGRN "\033[0;32;32m"
#define KCYN "\033[0;36m"
#define KRED "\033[0;32;31m"
#define KYEL "\033[1;33m"
#define KMAG "\033[0;35m"
#define KBLU "\033[0;32;34m"
#define KCYN_L "\033[1;36m"
#define RESET "\033[0m"
static int destroy_flag = 0;

int f_temp = 10;
int f_consol = 12;
int f_luz = 10;

//#include "serial_data.h"

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

#ifndef _STRUCTURES_P2_
#define _STRUCTURES_P2_
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
#endif

struct per_session_data {
	int fd;
};
int shmidtempdatos;
int shmidluzdatos;
int shmidindices;
int shmidcomandos;
float luzInitDato = 2.2;
float tempInitDato = 22.2;

infoUsuario usuario1;

pthread_mutex_t mutex_shidtempdatos = PTHREAD_MUTEX_INITIALIZER; //Mutex Buffer Datos Temperatura
pthread_mutex_t mutex_shidluzdatos = PTHREAD_MUTEX_INITIALIZER; // Mutex buffer datos luz
pthread_mutex_t mutex_shidindices = PTHREAD_MUTEX_INITIALIZER; //Mutex indices
pthread_mutex_t mutex_shidluz = PTHREAD_MUTEX_INITIALIZER; //Mutex frecuencia luz
pthread_mutex_t mutex_shidtemp = PTHREAD_MUTEX_INITIALIZER; // Mutex frecuencia temperatura
pthread_mutex_t mutex_shidconso = PTHREAD_MUTEX_INITIALIZER; //Mutex Frecuencia Consolidacion
pthread_mutex_t mutex_shidcomandos = PTHREAD_MUTEX_INITIALIZER; //Mutex Buffer Comandos

void error(const char *msg) {
	perror(msg);
	exit(0);
}

int serialize(char * buffer, datosTemp * d_temp, datosLuz * d_luz,
		comandos * d_comandos, indices index) {
	int i;
	sprintf(buffer, "{\"indeces\":[%d,%d,%d],\"temp\":[", index.indTemp,
			index.indLuz, index.indCom);
	for (i = 0; i < index.indTemp - 1; i++) {
		sprintf(buffer, "%s{\"v\":%4.2f,\"ts\":%li},", buffer,
				d_temp[i].datoTemperatura, (long int) d_temp[i].tiempo);
	}
	sprintf(buffer, "%s{\"v\":%4.2f,\"ts\":%li}],\"lux\":[", buffer,
			d_temp[i].datoTemperatura, (long int) d_temp[i].tiempo);

	for (i = 0; i < index.indLuz - 1; i++) {
		sprintf(buffer, "%s{\"v\":%4.2f,\"ts\":%li},", buffer, d_luz[i].datoLuz,
				(long int) d_luz[i].tiempo);
	}
	sprintf(buffer, "%s{\"v\":%4.2f,\"ts\":%li}],\"com\":[", buffer,
			d_luz[i].datoLuz, (long int) d_luz[i].tiempo);

	for (i = 0; i < index.indCom - 1; i++) {
		sprintf(buffer, "%s{\"t\":%d,\"l\":%d,\"c\":%d,\"ts\":%li},", buffer,
				d_comandos[i].freqTemp, d_comandos[i].freqLuz,
				d_comandos[i].freqconsol, (long int) d_comandos[i].tiempo);
	}
	sprintf(buffer, "%s{\"t\":%d,\"l\":%d,\"c\":%d,\"ts\":%li}]}", buffer,
			d_comandos[i].freqTemp, d_comandos[i].freqLuz,
			d_comandos[i].freqconsol, (long int) d_comandos[i].tiempo);

	printf("%s", buffer);
}
//Para WebSockets
static void INT_HANDLER(int signo) {
	destroy_flag = 1;
}

int websocket_write(struct lws *wsi_in, char *str, int str_size_in) {
	if (str == NULL || wsi_in == NULL)
		return -1;
	int n;
	int len;
	unsigned char *out = NULL;
	if (str_size_in < 1)
		len = strlen(str);
	else
		len = str_size_in;
	out = (unsigned char *) malloc(
			sizeof(char)
					* (LWS_SEND_BUFFER_PRE_PADDING + len
							+ LWS_SEND_BUFFER_POST_PADDING));
//* setup the buffer*/
	memcpy(out + LWS_SEND_BUFFER_PRE_PADDING, str, len);
//* write out*/
	n = lws_write(wsi_in, out + LWS_SEND_BUFFER_PRE_PADDING, len,
			LWS_WRITE_TEXT);
	printf(KBLU"[websocket_write] %s\n"RESET, str);
//* free the buffer*/
	free(out);
	return n;
}

int flag_websocket=0;

static int ws_service_callback(struct lws *wsi,
		enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	//int f_temp;
	//int f_consol;
	//int f_luz;

	char buffer[200];
	time_t tiempo;

//	infoUsuario luz = usuario1;
//	comandos* buffer_comandos;
//	comandos comandoActual;
//	buffer_comandos = (comandos*) shmat(shmidcomandos, 0, 0);
//	int tempfreqdefault;
//	int luzfreqdefault;
//	int consofreqdefault;
//	char* shared_memory_luz;
//	char* shared_memory_temp;
//	char* shared_memory_conso;
//
//	int identificadorLuz = luz.IDmemoryLuz;
//	shared_memory_luz = (char*) shmat(identificadorLuz, 0, 0);
//	int identificadorTemp = luz.IDmemoryTemp;
//	shared_memory_temp = (char*) shmat(identificadorTemp, 0, 0);
//	int identificadorConso = luz.IDmemoryConso;
//	shared_memory_conso = (char*) shmat(identificadorConso, 0, 0);
//	indices *indicecomando;
//	indicecomando = (indices*) shmat(shmidindices, 0, 0);
//	pthread_mutex_lock(&mutex_shidindices);
//	indicecomando->indCom = 0;
//	pthread_mutex_unlock(&mutex_shidindices);

	switch (reason) {
	case LWS_CALLBACK_ESTABLISHED:
		printf(KYEL"[Main Service] Connection established\n"RESET);
		sprintf(buffer,
				"{\"f_temp\":%d, \"f_luz\":%d, \"f_consol\":%d, \"temp\":%4.2f, \"luz\":%4.2f}",
				f_temp, f_luz, f_consol, tempInitDato, luzInitDato);
		websocket_write(wsi, buffer, -1);
		break;
//* If receive a data from client*/

	case LWS_CALLBACK_RECEIVE:

		printf(KCYN_L"[Main Service] Server recvived:%s\n"RESET, (char *) in);

		sscanf((char *) in, "{\"f_temp\":%d,\"f_luz\":%d,\"f_consol\":%d}",
				&f_temp, &f_luz, &f_consol);

		flag_websocket = 1;
//		tiempo = time(0);

//		pthread_mutex_lock(&mutex_shidluz);
//		sprintf(shared_memory_luz, "%d", luzfreqdefault);
//		printf("%s\n", shared_memory_luz);
//		pthread_mutex_unlock(&mutex_shidluz);
//
//		pthread_mutex_lock(&mutex_shidtemp);
//		sprintf(shared_memory_temp, "%d", tempfreqdefault);
//		printf("%s\n", shared_memory_temp);
//		pthread_mutex_unlock(&mutex_shidtemp);
//
//		pthread_mutex_lock(&mutex_shidconso);
//		sprintf(shared_memory_conso, "%d", consofreqdefault);
//		printf("%s\n", shared_memory_conso);
//		pthread_mutex_unlock(&mutex_shidconso);
//
//		comandoActual.freqLuz = luzfreqdefault;
//		comandoActual.freqTemp = tempfreqdefault;
//		comandoActual.freqconsol = consofreqdefault;
//		comandoActual.tiempo = tiempo;
//
//		pthread_mutex_lock(&mutex_shidindices);
//
//		pthread_mutex_lock(&mutex_shidcomandos);
//
//		buffer_comandos[indicecomando->indCom] = comandoActual;
//		//indicecomando->indLuz += 1;
//		//indicecomando->indTemp+=1;
//		indicecomando->indCom += 1;
//
//		pthread_mutex_unlock(&mutex_shidcomandos);
//
//		pthread_mutex_unlock(&mutex_shidindices);

		printf("t%d,l%d,c%d\n", f_temp, f_luz, f_consol);
		break;


	case LWS_CALLBACK_CLOSED:
		printf(KYEL"[Main Service] Client close.\n"RESET);
		break;
	default:
		break;
	}
	return 0;
}

void *consolidacion(void *arg) {

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[60000];

	portno = 51717;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

	if (sockfd < 0)
		error("ERROR opening socket");

	server = gethostbyname("nick.local");
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr)); // sets all values in a buffer to zero
	serv_addr.sin_family = AF_INET; // contains a code for the address family
	//serv_addr.sin_family = AF_UNIX;
	bcopy((char *) server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);

	serv_addr.sin_port = htons(portno); //contain the port number
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)

		error("ERROR connecting");

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

	datosLuz buffLuzActual[600];
	datosTemp buffTempActual[600];
	comandos bufferComActual[600];
	indices indicesActual;
	int indiceLuz, indiceTemp, indiceCom;

	for (;;) {
		//cout<<"Frecuencia Muestreo Actual: "<<freqMuestras<<endl;
		sleep(freqMuestras);
		cout << "CONSOLIDACION" << endl;
		pthread_mutex_lock(&mutex_shidindices);
		indiceLuz = indicestodos->indLuz;
		indiceTemp = indicestodos->indTemp;
		indiceCom = indicestodos->indCom;
		indicesActual.indCom = indicestodos->indCom;
		indicesActual.indLuz = indicestodos->indLuz;
		indicesActual.indTemp = indicestodos->indTemp;

		pthread_mutex_lock(&mutex_shidluzdatos);

		for (int i = 0; i < indiceLuz; i++) {

			buffLuzActual[i] = buffer_luz[i];

		}
		pthread_mutex_unlock(&mutex_shidluzdatos);

		pthread_mutex_lock(&mutex_shidtempdatos);

		for (int i = 0; i < indiceTemp; i++) {
			buffTempActual[i] = buffer_Temp[i];
		}

		//buffTempActual = buffer_Temp;
		pthread_mutex_unlock(&mutex_shidtempdatos);

		pthread_mutex_lock(&mutex_shidcomandos);

		for (int i = 0; i < indiceCom; i++) {
			bufferComActual[i] = buffer_comandos[i];
		}

		//bufferComActual = buffer_comandos;
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

		indicestodos->indLuz = 0;
		indicestodos->indCom = 0;
		indicestodos->indTemp = 0;
		pthread_mutex_unlock(&mutex_shidindices);

		pthread_mutex_lock(&mutex_shidconso); //Mutex shmidconso lock
		sscanf(shared_memory_conso, "%d", &freqMuestras);
		pthread_mutex_unlock(&mutex_shidconso); //Mutex shmidconso lock

		//serialize(char * buffer, datosTemp * d_temp, datosLuz * d_luz, comandos * d_comandos, indices index);
		serialize(buffer, buffTempActual, buffLuzActual, bufferComActual,
				indicesActual);

		// /*
		//printf("Please enter the message: ");
		cout << "Transmite el Socket" << endl;
		//bzero(buffer, 60000);
		//buffer="CAMM";

		//fgets(buffer, 255, stdin);
		n = write(sockfd, buffer, strlen(buffer));
		cout << "Transfirio" << endl;
		if (n < 0)
			error("ERROR writing to socket");
		bzero(buffer, 60000);
		//n = read(sockfd, buffer, 60000-1);
		//if (n < 0)
		//error("ERROR reading from socket");
		//printf("%s\n", buffer);

		// */
	}
	return NULL;
}
void *usuario(void *arg) {

	// server url will usd port 5000
	int port = 5000;
	const char *interface = NULL;
	struct lws_context_creation_info info;
	struct lws_protocols protocol;
	struct lws_context *context;
	// Not using ssl
	const char *cert_path = NULL;
	const char *key_path = NULL;
	// no special options
	int opts = 0;
	unsigned int ms, oldms = 0;
	//* register the signal SIGINT handler */
	struct sigaction act;
	act.sa_handler = INT_HANDLER;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction( SIGINT, &act, 0);
	//* setup websocket protocol */
	protocol.name = "my-echo-protocol";
	protocol.callback = ws_service_callback;
	protocol.per_session_data_size = sizeof(struct per_session_data);
	protocol.rx_buffer_size = 0;
	//* setup websocket context info*/
	memset(&info, 0, sizeof info);
	info.port = port;
	info.iface = interface;
	info.protocols = &protocol;
	info.extensions = lws_get_internal_extensions();
	info.ssl_cert_filepath = cert_path;
	info.ssl_private_key_filepath = key_path;
	info.gid = -1;
	info.uid = -1;
	info.options = opts;
	//* create libwebsocket context. */
	context = lws_create_context(&info);
	if (context == NULL) {
		printf(KRED"[Main] Websocket context create error.\n"RESET);
		//return -1;
	}
	printf(KGRN"[Main] Websocket context create success.\n"RESET);

	 infoUsuario *luz = (infoUsuario *) arg;
	 comandos* buffer_comandos;
	 comandos comandoActual;
	 time_t tiempo;
	 buffer_comandos = (comandos*) shmat(shmidcomandos, 0, 0);
//	 int tempfreqdefault;
//	 int luzfreqdefault;
//	 int consofreqdefault;
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

		lws_service(context, 0); //second parameter is timeout.
		if(flag_websocket)
		{
			tiempo = time(0);

			pthread_mutex_lock(&mutex_shidluz);
			sprintf(shared_memory_luz, "%d", f_luz);
			printf("%s\n", shared_memory_luz);
			pthread_mutex_unlock(&mutex_shidluz);

			pthread_mutex_lock(&mutex_shidtemp);
			sprintf(shared_memory_temp, "%d", f_temp);
			printf("%s\n", shared_memory_temp);
			pthread_mutex_unlock(&mutex_shidtemp);

			pthread_mutex_lock(&mutex_shidconso);
			sprintf(shared_memory_conso, "%d", f_consol);
			printf("%s\n", shared_memory_conso);
			pthread_mutex_unlock(&mutex_shidconso);

			comandoActual.freqLuz = f_luz;
			comandoActual.freqTemp = f_temp;
			comandoActual.freqconsol = f_consol;
			comandoActual.tiempo = tiempo;

			pthread_mutex_lock(&mutex_shidindices);

			pthread_mutex_lock(&mutex_shidcomandos);

			buffer_comandos[indicecomando->indCom] = comandoActual;
			//indicecomando->indLuz += 1;
			//indicecomando->indTemp+=1;
			indicecomando->indCom += 1;

			pthread_mutex_unlock(&mutex_shidcomandos);

			pthread_mutex_unlock(&mutex_shidindices);


			flag_websocket = 0;
		}
	}


	usleep(10);
	lws_context_destroy(context);
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
	pthread_mutex_lock(&mutex_shidindices);
	indiceluz->indLuz = 0;

	pthread_mutex_unlock(&mutex_shidindices);
	for (;;) {
		pthread_mutex_lock(&mutex_shidluz);
		sscanf(shared_memory_luz, "%d", &frecuenciaLuz);
		pthread_mutex_unlock(&mutex_shidluz);
		time_t tiempo = time(0);
		struct tm * actual = localtime(&tiempo);
		uint16_t pin_value = a_pin2->read();
		float Rsensor;
		Rsensor = (float) (1023 - pin_value) * 10 / pin_value;
		std::cout << "Luz: " << actual->tm_mon + 1 << "/" << actual->tm_mday

		<< "/" << actual->tm_hour << ":" << actual->tm_min << ": " << Rsensor
				<< std::endl;
		luzInitDato = Rsensor;
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
	buffer_Temp = (datosTemp*) shmat(shmidtempdatos, 0, 0);
	indicetemp = (indices*) shmat(shmidindices, 0, 0);
	pthread_mutex_lock(&mutex_shidindices);
	indicetemp->indTemp = 0;
	indicetemp->indCom = 0;
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
		std::cout << "Tem: " << actual->tm_mon + 1 << "/" << actual->tm_mday

		<< "/" << actual->tm_hour << ":" << actual->tm_min << ": "
				<< temperature << std::endl;

		tempInitDato = temperature;

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
	//infoUsuario usuario1;
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
