#ifndef __BSP_H_INCLUDED__
#define __BSP_H_INCLUDED__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include "eqep.h"
#include "hcsr04.h"

#define MS_TO_NS_FACTOR					1000000
#define MOTOR_STARTING_PULSE_IN_MS		1.5
#define MOTOR_PULSE_PERIOD_IN_MS		20
#define POLLING_SPEED_IN_MS				10
#define NUMBER_OF_FILTER_ELEMENTS		100

#define X1								0
#define X2								4096

#define Y1								1
#define Y2								2

struct curve_args
{
	double slope;
	double origin;
};

void init_peripherals(void);
void start_pwm(void);
void update_pwm(double period_in_ms,double duty_in_ms);
void update_pwm_duty_cycle(double duty_in_ms);
void update_pwm_period_cycle(double period_in_ms);
void slope_maker(double y1 , double y2 , double x1, double x2,curve_args* parameter);
double filter_shifter(double table[]);
void stop_pwm(void);
void set_adc(void);
int read_adc(void);
bool read_gpio60_P9_12(void);
void first_time_fill_fillter(double *table,curve_args* parameter);

#endif
