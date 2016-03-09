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
	double period_ms = 200;
	double duty_ms = 0.9;
	int value =0;

	//SCRIPT d'initialisation du PWM ainsi que de l'adc
	system("bash init.sh");

	while(1)
	{
		value = read_adc();
		duty_ms = value;
		duty_ms *= (199/4096.0);
		update_pwm(period_ms,duty_ms);
	}




	stop_pwm();
	return 0;
}


