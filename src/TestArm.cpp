//============================================================================
// Name        : TestArm.cpp
// Author      : Mike
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "bsp.h"
#include "baborwire.h"
#include "TestArm.h"

using namespace std;
void *primary_thread(void *incoming_args);

struct args_struct
{
	void* dumss;
};

struct shared_data
{
	float distance_in_cm;
	float speed;

};
uint32_t vitesse;

int main()
{
	cout << "We are going to control the CamCar !!!" << endl;
	pthread_t thread;
	void* dummy;
	struct args_struct args;
	pthread_t thread_comm;
	int rc;

	/* disable stdout buffering */
	setvbuf(stdout, NULL, _IONBF, 0);

	if ((rc = pthread_create(&thread_comm, NULL, handler_comm, NULL)))
	{
	      fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
	      return EXIT_FAILURE;
	}


	if(pthread_create(&thread,NULL,&primary_thread,(void*)&args))
	{
		cout << "Thread creation failed !!!" << endl;
				return 1;
	}
	usleep(500);
	pthread_join(thread,&dummy);
	pthread_join(thread_comm, NULL);
	return 0;
}
void *primary_thread(void *incoming_args)
{
	args_struct *args = (args_struct *)incoming_args;
	curve_args curve_param;
	shared_data shared;
	curve_param.slope =0;
	curve_param.origin =0;
	double period_ms = MOTOR_PULSE_PERIOD_IN_MS;
	double duty_ms = MOTOR_STARTING_PULSE_IN_MS;
	double filter[NUMBER_OF_FILTER_ELEMENTS];
	int value=0;
	bool push_button_state=0;
	float distance_read_in_meter=0;
	bool direction =0;

	init_peripherals();
	//Initialisation du module hardware pour compter les tours de l'encodeur
#ifdef encoder
	eQEP eqep(eQEP0,eQEP::eQEP_Mode_Relative);

	//Configuration de la vitesse d'échantillonage
	eqep.set_period(POLLING_SPEED_IN_MS * MS_TO_NS_FACTOR);
#endif


	update_pwm(period_ms,duty_ms);

	slope_maker(Y1,Y2,X1,X2,&curve_param);
//	args->distance_read_in_meter = read_distance();

	/*while(!args->push_button_state)
	{
		args->push_button_state = read_gpio60_P9_12();
	}*/
	first_time_fill_fillter(filter,&curve_param);

	//args->distance_read_in_meter = read_distance();
	while(1)
	{

		//value = read_adc();

		duty_ms = vitesse;

		duty_ms *= curve_param.slope;
		duty_ms += curve_param.origin;

		filter[0]=duty_ms;

		duty_ms = filter_shifter(filter);
		printf("vit : %f\n",duty_ms);
		update_pwm(period_ms,duty_ms);

		push_button_state = read_gpio60_P9_12();
		if(push_button_state)
		{
			direction = !direction;
			set_motor_direction(direction);
		}
#ifdef sensor_distance
		distance_read_in_meter = read_distance();
#endif
	}

	stop_pwm();
	return 0;
}

