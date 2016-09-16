//============================================================================
// Name        : TestSensores.cpp
// Author      : CAMM NEVS
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include "mraa.hpp"
#include <iostream>
#include <unistd.h>
#include <pthread.h>


void *luz(void *arg) {

	mraa::Aio* a_pin2 = (mraa::Aio *) arg;
	uint16_t pin_value = a_pin2->read();
	std::cout << "Valor de Luz " << pin_value << std::endl;
	sleep(1);

	return NULL;
}

void *temperatura(void *arg) {

	mraa::Aio* a_pin_tempe = (mraa::Aio *) arg;
	uint16_t pin_value = a_pin_tempe->read();
	std::cout << "Valor de temperatura " << pin_value << std::endl;
	sleep(1);

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

	mraa::Aio* a_pin_luz = new mraa::Aio(0);
	if (a_pin_luz == NULL) {
		std::cerr << "Can't create mraa::Aio object, exiting" << std::endl;
		return MRAA_ERROR_UNSPECIFIED;
	}

	mraa::Aio* a_pin_tmp = new mraa::Aio(1);
	if (a_pin_tmp == NULL) {
		std::cerr << "Can't create mraa::Aio object, exiting" << std::endl;
		return MRAA_ERROR_UNSPECIFIED;
	}

	pthread_t h1;
	pthread_t h2;
	pthread_create(&h1, NULL, luz, (void *) a_pin_luz);
	sleep(5);
	pthread_create(&h2, NULL, temperatura, (void *) a_pin_tmp);

	pthread_join(h1, NULL);
	pthread_join(h2, NULL);

	printf("Fin\n");

	/*
	 mraa_platform_t platform = mraa_get_platform_type();
	 if ((platform != MRAA_INTEL_GALILEO_GEN1)
	 && (platform != MRAA_INTEL_GALILEO_GEN2)
	 && (platform != MRAA_INTEL_EDISON_FAB_C)) {
	 std::cerr << "Unsupported platform, exiting" << std::endl;
	 return MRAA_ERROR_INVALID_PLATFORM;
	 }


	 mraa::Aio* a_pin = new mraa::Aio(0);
	 if (a_pin == NULL) {
	 std::cerr << "Can't create mraa::Aio object, exiting" << std::endl;
	 return MRAA_ERROR_UNSPECIFIED;
	 }


	 for (;;) {
	 uint16_t pin_value = a_pin->read();
	 std::cout << "analog input value " << pin_value << std::endl;
	 sleep(1);
	 }

	 return MRAA_SUCCESS;

	 */

}
