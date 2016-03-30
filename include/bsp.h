#ifndef __BSP_H_INCLUDED__
#define __BSP_H_INCLUDED__

#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <pthread.h>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include "eqep.h"
#include "hcsr04.h"

#define MS_TO_NS_FACTOR					1000000
#define MOTOR_STARTING_PULSE_IN_MS		0.05
#define MOTOR_PULSE_PERIOD_IN_MS		0.2
#define POLLING_SPEED_IN_MS				10
#define NUMBER_OF_FILTER_ELEMENTS		10
#define NUMBER_OF_DISTANCE_ELEMENTS		5

#define X1								0
#define X2								100

#define Y1								0.1/1000
#define Y2								0.2

#define V1								-100
#define V2								100

#define A1								0
#define A2								2585

//#define sensor_distance
//#define test_adc_speed
#define change_direction_with_push_button
//#define encoder
//#define comm

struct curve_args
{
	double slope;
	double origin;
};
typedef double (*fill_filter_fct)(void);

void init_peripherals(void);
double speed_regulator(double speed,double distance);
void start_pwm(void);
void setup_motor_direction_pin(void);
void set_motor_direction(bool sens);
void update_pwm(double period_in_ms,double duty_in_ms);
void update_pwm_duty_cycle(double duty_in_ms);
void update_pwm_period_cycle(double period_in_ms);
void slope_maker(double y1 , double y2 , double x1, double x2,curve_args* parameter);
double filter_shifter(double table[],double nb_element);
void stop_pwm(void);
void set_adc(void);
double read_adc(void);
bool read_gpio60_P9_12(void);
void first_time_fill_filter(double *table,curve_args* parameter,fill_filter_fct fill,double nb_element);

#endif
