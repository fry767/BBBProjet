#ifndef __BSP_H_INCLUDED__
#define __BSP_H_INCLUDED__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <string>


void start_pwm(void);
void update_pwm(double period_in_ms,double duty_in_ms);
void update_pwm_duty_cycle(double duty_in_ms);
void update_pwm_period_cycle(double period_in_ms);
void stop_pwm(void);
void set_adc(void);
int read_adc(void);

#endif
