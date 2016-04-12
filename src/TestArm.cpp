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
#include "lm74_drv.h"
#include "encoder_pru_drv.h"
using namespace std;

#define BATTERY_MAX_TEMP 140

void *primary_thread(void *incoming_args);
double apply_mod(double value,curve_args *mod);
bool battery_is_in_overtemperature(double current_temperature);
struct args_struct
{
	void* dumss;
};

struct shared_data
{
	float distance_in_cm;
	float speed;

};
uint32_t vitesse = 100;


int main()
{
	cout << "We are going to control the CamCar !!!" << endl;
	pthread_t thread;
	void* dummy;
	struct args_struct args;

#ifdef comm
	pthread_t thread_comm;
	int rc;
	/* disable stdout buffering */
	setvbuf(stdout, NULL, _IONBF, 0);

	if ((rc = pthread_create(&thread_comm, NULL, handler_comm, NULL)))
	{
	      fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
	      return EXIT_FAILURE;
	}
#endif
	if(pthread_create(&thread,NULL,&primary_thread,(void*)&args))
	{
		cout << "Thread creation failed !!!" << endl;
				return 1;
	}
	usleep(500);
	pthread_join(thread,&dummy);
#ifdef comm
	pthread_join(thread_comm, NULL);

#endif
	return 0;
}

void *primary_thread(void *incoming_args)
{
	args_struct *args = (args_struct *)incoming_args;

	curve_args curve_param;
	curve_args incomming_speed_modifier;

	shared_data shared;

	curve_param.slope =0;
	curve_param.origin =0;

	incomming_speed_modifier.slope = 0;
	incomming_speed_modifier.origin= 0;


	double vitesse_locale=0;
	double vitesse_mesure=0;
	double period_ms = MOTOR_PULSE_PERIOD_IN_MS;
	double duty_ms = MOTOR_STARTING_PULSE_IN_MS;
	double filter[NUMBER_OF_FILTER_ELEMENTS];

	float distance_read_in_cm=0;

	int value=0;

	bool push_button_state=0;
	bool direction =0;
	bool error_overtemperature =0;

	init_peripherals();

	update_pwm(period_ms,duty_ms);

	slope_maker(Y1,Y2,X1,X2,&curve_param);
	slope_maker(V1,V2,incomming_v1,incomming_v2,&incomming_speed_modifier);

	//Initialisation du module hardware pour compter les tours de l'encodeur
#ifdef encoder
	int32_t lecture_encoder=0;
	eQEP eqep(eQEP0,eQEP::eQEP_Mode_Relative);

	//Configuration de la vitesse d'échantillonage
	eqep.set_period(POLLING_SPEED_IN_MS * MS_TO_NS_FACTOR);
#endif




#ifdef test_adc_speed
	double test_adc_speed_table[NUMBER_OF_FILTER_ELEMENTS];
	curve_args test_adc_speed_modifier;
	test_adc_speed_modifier.slope=0;
	test_adc_speed_modifier.origin=0;
	slope_maker(V1,V2,A1,A2,&test_adc_speed_modifier);
	first_time_fill_filter(test_adc_speed_table,&test_adc_speed_modifier,read_adc,NUMBER_OF_FILTER_ELEMENTS);
#endif




#ifdef sensor_distance
	double distance_sensor_filter[NUMBER_OF_DISTANCE_ELEMENTS];
	curve_args distance_modifier;
	distance_modifier.slope=1;
	distance_modifier.origin=0;
	first_time_fill_filter(distance_sensor_filter,&distance_modifier,read_distance,NUMBER_OF_DISTANCE_ELEMENTS);
#endif

#ifdef encoder_pru
	double linear_speed_filter[NUMBER_OF_DISTANCE_ELEMENTS];
	curve_args linear_speed_mod;
	linear_speed_mod.slope = 1;
	linear_speed_mod.origin=0;
	first_time_fill_filter(linear_speed_filter,&linear_speed_mod,read_linear_speed,NUMBER_OF_DISTANCE_ELEMENTS);
#endif

	while(1)
	{
#ifdef temperature_sens
		if(battery_is_in_overtemperature(read_temperature()))
		{
			error_overtemperature = 1;
		}
#endif
#ifdef sensor_distance
		distance_sensor_filter[0]=read_distance();
		distance_read_in_cm = filter_shifter(distance_sensor_filter,NUMBER_OF_DISTANCE_ELEMENTS);

#endif
		printf("Prochain objet = %f cm    ",distance_read_in_cm);
#ifdef encoder_pru
		linear_speed_filter[0] = read_linear_speed();
		vitesse_mesure = filter_shifter(linear_speed_filter,NUMBER_OF_DISTANCE_ELEMENTS);

#endif
		printf("Vitesse mesurée = %f \n",vitesse_mesure);

#ifdef encoder
		lecture_encoder = eqep.get_position(false);
		printf("Vitesse mesurée = %d ",lecture_encoder);
#endif

		if(!error_overtemperature)
		{
			vitesse_locale = vitesse;
			vitesse_locale = apply_mod(vitesse_locale,&incomming_speed_modifier);
		}
		else
		{
			//Permettra de fixé la vitesse peut importe la vitesse regler par l'utilisateur
			//Plus le chariot sera proche du mur, plus la vitesse sera réguler par la fonction
			//speed_regulator
			vitesse_locale = 200;
			vitesse_locale = apply_mod(vitesse_locale,&incomming_speed_modifier);
		}






#ifdef test_adc_speed
		vitesse_locale = read_adc();
		vitesse_locale = apply_mod(vitesse_locale,&test_adc_speed_modifier);

#endif

		//Regule la vitesse en fonction de la distance par rapport au mur/obstacle
		vitesse_locale = speed_regulator(vitesse_locale,distance_read_in_cm);
		direction = 0;
		if(vitesse_locale < 0)
		{
			vitesse_locale = -vitesse_locale;
			direction = 1;

		}
		duty_ms = vitesse_locale;
		duty_ms = apply_mod(duty_ms,&curve_param);



		filter[0]=duty_ms;


		duty_ms = filter_shifter(filter,NUMBER_OF_FILTER_ELEMENTS);
		/*printf("Direction = %d  ",direction);
		printf("Duty cycle : %f\n",duty_ms);*/
		update_pwm(period_ms,duty_ms);

		push_button_state = read_gpio60_P9_12();
#ifdef change_direction_with_push_button
		if(push_button_state)
		{
			direction = !direction;
			set_motor_direction(direction);
		}
#endif

	}

	stop_pwm();
	return 0;
}
double apply_mod(double value,curve_args *mod)
{
	value *= mod->slope;
	value += mod->origin;
	return value;

}
bool battery_is_in_overtemperature(double current_temperature)
{
	if(current_temperature > BATTERY_MAX_TEMP)
		return 1;
	else
		return 0;
}
