/*
 * bsp.cpp

 *
 *  Created on: Feb 11, 2016
 *      Author: root
 */
#include "bsp.h"

void init_peripherals(void)
{
	//SCRIPT d'initialisation du PWM ainsi que de l'adc
	system("bash init.sh");
	start_pwm();
	init_hcsr04();
}
void start_pwm(void)
{
	FILE *pwmHandle = NULL;
	char *pwm_enable  = &(std::string(getenv("MOTORPATH"))+std::string("/pwm0/enable"))[0u];
	pwmHandle = fopen(pwm_enable,"r+");
	fwrite("1",sizeof(char),1,pwmHandle);
	fclose(pwmHandle);
}
void stop_pwm(void)
{
	FILE *pwmHandle = NULL;
	char *pwm_enable  = &(std::string(getenv("MOTORPATH"))+std::string("/pwm0/enable"))[0u];
	pwmHandle = fopen(pwm_enable,"r+");
	fwrite("0",sizeof(char),1,pwmHandle);
	fclose(pwmHandle);

}
void update_pwm_duty_cycle(double duty_in_ms)
{
	FILE *pwmHandle = NULL;
	double duty_ns = duty_in_ms * MS_TO_NS_FACTOR;
	double dummy = duty_ns;
	char str[12];
	char *pwmDuty_ns   = &(std::string(getenv("MOTORPATH"))+std::string("/pwm0/duty_cycle"))[0u];

	pwmHandle = fopen(pwmDuty_ns,"r+");
	sprintf(str,"%lf",dummy);
	fwrite(str,sizeof(char),7,pwmHandle);
	fclose(pwmHandle);


}
void update_pwm_period_cycle(double period_in_ms)
{
	FILE *pwmHandle = NULL;
	double period_ns = period_in_ms * MS_TO_NS_FACTOR;
	double dummy = period_ns;
	char str[12];
	char *pwmPeriod_ns = &(std::string(getenv("MOTORPATH"))+std::string("/pwm0/period"))[0u];

	pwmHandle = fopen(pwmPeriod_ns,"r+");
	sprintf(str,"%lf",dummy);
	fwrite(str,sizeof(char),8,pwmHandle);
	fclose(pwmHandle);

}
void update_pwm(double period_in_ms,double duty_in_ms)
{

		//stop_pwm();
		update_pwm_period_cycle(period_in_ms);
		update_pwm_duty_cycle(duty_in_ms);
		//start_pwm();
}
int read_adc(void)
{
	FILE *adcHandler;
	long lSize;
	char *buffer;
	char* adc0_loc = getenv("ADC_0_value");;
	size_t result;

	adcHandler = fopen(adc0_loc,"r");
	if(adcHandler == NULL)
	{
		//fputs("ADC0 not found get rekt",stderr);
		//exit(1);
	}
	fseek(adcHandler,0,SEEK_END);
	lSize = ftell(adcHandler);
	rewind(adcHandler);

	buffer = (char*)malloc(sizeof(char)*lSize);
	if(buffer == NULL)
	{
		fputs("Memory have some problem, deal with it",stderr);
		exit(2);
	}
	result = fread(buffer,1,lSize,adcHandler);
	int value = atoi(buffer);
//	fputs(buffer,stdout);
	fclose(adcHandler);
	free(buffer);
	return value ;
}
void set_adc(void)
{

}
bool read_gpio60_P9_12(void)
{
	FILE *gpioHandler;
	long lSize;
	char *buffer;
	char* gpio60_loc = "/sys/class/gpio/gpio60/value";
	size_t result;
	gpioHandler = fopen(gpio60_loc,"r");


	fseek(gpioHandler,0,SEEK_END);
	lSize = ftell(gpioHandler);
	rewind(gpioHandler);

	buffer = (char*)malloc(sizeof(char)*lSize);
	if(buffer == NULL)
	{
		fputs("Memory have some problem, deal with it",stderr);
		exit(2);
	}
	result = fread(buffer,1,lSize,gpioHandler);
	int value = atoi(buffer);
	//fputs(buffer,stdout);
	fclose(gpioHandler);
	free(buffer);
	return value ;
}
void slope_maker(double y1 , double y2 , double x1, double x2,curve_args* parameter)
{

	parameter->slope = (y2-y1)/(x2-x1);
	parameter->origin = y1 - x1 * parameter->slope;


}
void first_time_fill_fillter(double *table,curve_args* parameter)
{
	for(int i = 0;i < NUMBER_OF_FILTER_ELEMENTS; i++)
	{

		table[i] = read_adc();
		table[i]*= parameter->slope;
		table[i] += parameter->origin;
	}
}
double filter_shifter(double table[])
{
	double sum = 0;
	for(int j =0;j<NUMBER_OF_FILTER_ELEMENTS;j++)
		sum += table[j];

	for(int i = NUMBER_OF_FILTER_ELEMENTS - 1 ; i > 0;i--)
		table[i] = table[i-1];
	return (sum/NUMBER_OF_FILTER_ELEMENTS);
}
