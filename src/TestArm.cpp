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

int main()
{
	cout << "We are going to control the PWM !!!" << endl;
	double period_ms = 20;
	double duty_ms = MOTOR_STARTING_PULSE;
	int value =0;
	bool pushButtonState =0;

	//SCRIPT d'initialisation du PWM ainsi que de l'adc
	system("bash init.sh");
	start_pwm();
	update_pwm(period_ms,duty_ms);
	while(!pushButtonState)
	{
		pushButtonState = read_gpio60_P9_12();
	}

	while(1)
	{

		value = read_adc();
		duty_ms = value;
		duty_ms *= (1/4096.0);
		duty_ms += 1;
		update_pwm(period_ms,duty_ms);

	}




	stop_pwm();
	return 0;
}


