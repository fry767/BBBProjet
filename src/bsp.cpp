/*
 * bsp.cpp

 *
 *  Created on: Feb 11, 2016
 *      Author: root
 */
#include "bsp.h"

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
	double duty_ns = duty_in_ms * 100000;
	double dummy = duty_ns;
	char str[12];
	char *pwmDuty_ns   = &(std::string(getenv("MOTORPATH"))+std::string("/pwm0/duty_cycle"))[0u];

	pwmHandle = fopen(pwmDuty_ns,"r+");
	sprintf(str,"%lf",dummy);
	fwrite(str,sizeof(char),8,pwmHandle);
	fclose(pwmHandle);


}
void update_pwm_period_cycle(double period_in_ms)
{
	FILE *pwmHandle = NULL;
	double period_ns = period_in_ms * 100000;
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
		fputs("ADC0 not found get rekt",stderr);
		exit(1);
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


