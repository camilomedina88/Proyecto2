#include "mraa.hpp"
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define size 1024

using namespace std;

void *usuario(void *arg) {

	int tempfreqdefault = 10;
	int luzfreqdefault = 7;
	char* shared_memory;
	//int shmid;

	int Identificador = (int) arg;

	cout << "ID: " << Identificador << endl;

	shared_memory = (char*) shmat(Identificador, 0, 0);
	printf("shared memory attached at address %p\n", shared_memory);
	sprintf(shared_memory, "HOLAAA A TODOS");


	/*cout<<"Frecuencia Temperatura: ";
	 cin>>tempfreqdefault;

	 cout<<"Frecuencia Luz: ";
	 cin>>luzfreqdefault;

	 std::cout<<"ingresado tmp: "<<tempfreqdefault <<std::endl;
	 std::cout<<"ingresado luz: "<<luzfreqdefault <<std::endl;
	 */

	return NULL;
}

void *luz(void *arg) {

	mraa::Aio* a_pin2 = (mraa::Aio *) arg;

	for (;;) {
		time_t tiempo = time(0);
		struct tm * actual = localtime(&tiempo);
		uint16_t pin_value = a_pin2->read();

		float Rsensor;
		Rsensor = (float) (1023 - pin_value) * 10 / pin_value;
		std::cout << "Luz: " << actual->tm_mon + 1 << "/" << actual->tm_mday
				<< "/" << actual->tm_hour << ":" << actual->tm_min << ": "
				<< Rsensor << std::endl;
		sleep(10);
	}
	return NULL;
}

void *temperatura(void *arg) {

	mraa::Aio* a_pin_tempe = (mraa::Aio *) arg;

	for (;;) {
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
		sleep(9);
	}
	return NULL;
}

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

	int shmid; //shared memory ID
	if ((shmid = shmget(IPC_PRIVATE, size,
			IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) {
		perror("shmget");
		cout << "Esta Jodido" << endl;
		exit(1);
	}

	//Creacion Hilos

	pthread_t HiloLuz, HiloTmp, HiloUsr;
	pthread_create(&HiloUsr, NULL, usuario, (void *) shmid);
	pthread_create(&HiloLuz, NULL, luz, (void *) a_pin_luz);
	pthread_create(&HiloTmp, NULL, temperatura, (void *) a_pin_tmp);

	pthread_join(HiloUsr, NULL);
	pthread_join(HiloLuz, NULL);
	pthread_join(HiloTmp, NULL);

	printf("Fin\n");

}

