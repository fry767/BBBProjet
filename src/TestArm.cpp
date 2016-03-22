//============================================================================
// Name        : TestArm.cpp
// Author      : Mike
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "bsp.h"
#include "TestArm.h"

using namespace std;
void *primary_thread(void *incoming_args);

struct args_struct
{
	double period_ms;
	double duty_ms;
	double filter[NUMBER_OF_FILTER_ELEMENTS];
	int value;
	bool push_button_state;
	float distance_read_in_meter;
};

int main()
{
	cout << "We are going to control the CamCar !!!" << endl;
	pthread_t thread;
	void* dummy;
	struct args_struct args;
	args.period_ms = MOTOR_PULSE_PERIOD_IN_MS;
	args.duty_ms = MOTOR_STARTING_PULSE_IN_MS;
	args.value =0;
	args.push_button_state=0;


	if(pthread_create(&thread,NULL,&primary_thread,(void*)&args))
	{
		cout << "Thread creation failed !!!" << endl;
				return 1;
	}
	usleep(500);
	pthread_join(thread,&dummy);
	return 0;
}
void *primary_thread(void *incoming_args)
{
	args_struct *args = (args_struct *)incoming_args;
	curve_args curve_param;
	curve_param.slope =0;
	curve_param.origin =0;
	bool direction =0;

	init_peripherals();
	//Initialisation du module hardware pour compter les tours de l'encodeur
#ifdef encoder
	eQEP eqep(eQEP0,eQEP::eQEP_Mode_Relative);

	//Configuration de la vitesse d'échantillonage
	eqep.set_period(POLLING_SPEED_IN_MS * MS_TO_NS_FACTOR);
#endif


	update_pwm(args->period_ms,args->duty_ms);

	slope_maker(Y1,Y2,X1,X2,&curve_param);
//	args->distance_read_in_meter = read_distance();

	/*while(!args->push_button_state)
	{
		args->push_button_state = read_gpio60_P9_12();
	}*/
	first_time_fill_fillter(args->filter,&curve_param);

	//args->distance_read_in_meter = read_distance();
	while(1)
	{

		args->value = read_adc();
		args->duty_ms = args->value;
		args->duty_ms *= curve_param.slope;
		args->duty_ms += curve_param.origin;

		args->filter[0]=args->duty_ms;

		args->duty_ms = filter_shifter(args->filter);
		update_pwm(args->period_ms,args->duty_ms);

		args->push_button_state = read_gpio60_P9_12();
		if(args->push_button_state)
		{
			direction = !direction;
			set_motor_direction(direction);
		}

		//args->distance_read_in_meter = read_distance();
	}

	stop_pwm();
	return 0;
}

