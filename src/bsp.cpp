/*
 * bsp.cpp

 *
 *  Created on: Feb 11, 2016
 *      Author: root
 */
#include "bsp.h"
#include <math.h>

/*
 * Parametre pour une fonction exponentielle qui nous
 * permet de réguler la vitesse.
 */
#define a	-3.164
#define b	-0.0273
#define h 	-146.2
#define k	111

//Le nombre e en mathématique (2.71828)
#define exp	M_E

//Distance critique en cm où nous commençons à réguler la vitesse
#define critical_distance 100

/* Fonction qui nous permet de réguler la vitesse du chariot
 * en fonction de la distance d'un prochain obstacle lu par
 * le capteur de distance.
 *
 * Paramètre :
 * speed : vitesse du chariot commandé par l'utilisateur
 * distance : distance lu par le capteur de distance
 *
 * Valeur retournée : vitesse régulé en fonction de la distance
 * */
double speed_regulator(double speed,double distance)
{
	double return_speed =0;
	if(distance == 0)
	{
		return speed;
	}
	if(distance <= critical_distance)
	{
		if(speed > 0 )
		{
			return_speed = a*(pow(exp,b*(distance+h)))+k;

		}else
		{
			return_speed = -a*(pow(exp,b*(distance+h)))-k;
		}
	}else
	{
		return_speed = speed;
	}

	return return_speed;


}
/*
 * Fonction qui initialise le pwm, la direction du moteur
 * ainsi que le capteur de température.
 */
void init_peripherals(void)
{


	//Démarrage du module qui génère le pwm pour le moteur
	start_pwm();

	//Configuration de la pin qui controlera la direction du moteur
	setup_motor_direction_pin();

	//Initialisation du capteur de température
	lm74_init();



}
/*
 * Fonction qui configure le contrôle de la
 * direction du moteur
 */
void setup_motor_direction_pin(void)
{

	char* gpio48_loc = "/sys/class/gpio/gpio48/direction";
	FILE *motorDirectionHandler = NULL;
	motorDirectionHandler = fopen(gpio48_loc,"r+");
	fwrite("out",sizeof(char),3,motorDirectionHandler);
	fclose(motorDirectionHandler);

}
/* Fonction qui permet de contrôler la direction
 * du moteur
 */
void set_motor_direction(bool sens)
{
	char* gpio48_value = "/sys/class/gpio/gpio48/value";
	FILE *motorDirectionHandler = NULL;
	motorDirectionHandler = fopen(gpio48_value,"r+");
	if(sens)
		fwrite("1",sizeof(char),1,motorDirectionHandler);
	else
		fwrite("0",sizeof(char),1,motorDirectionHandler);
	fclose(motorDirectionHandler);
}

/*
 * Fonction qui démarre le module qui génère le pwm
 * à l'intérieur du Beaglebone
 *
 */
void start_pwm(void)
{
	FILE *pwmHandle = NULL;
	char *pwm_export  = &(std::string(getenv("MOTORPATH"))+std::string("/export"))[0u];
	pwmHandle = fopen(pwm_export,"w");
	fwrite("0",sizeof(char),1,pwmHandle);
	fclose(pwmHandle);

	char *pwm_enable  = &(std::string(getenv("MOTORPATH"))+std::string("/pwm0/enable"))[0u];
	pwmHandle = fopen(pwm_enable,"w");
	fwrite("1",sizeof(char),1,pwmHandle);
	fclose(pwmHandle);
}
/*
 * Fonction qui arrète le module qui génère le pwm
 * à l'intérieur du beaglebone
 */
void stop_pwm(void)
{
	FILE *pwmHandle = NULL;
	char *pwm_enable  = &(std::string(getenv("MOTORPATH"))+std::string("/pwm0/enable"))[0u];
	pwmHandle = fopen(pwm_enable,"w");
	fwrite("0",sizeof(char),1,pwmHandle);
	fclose(pwmHandle);

	prussdrv_pru_disable (0);
	prussdrv_exit ();

}
/*
 * Fonction qui permet de changer la valeur du rapport
 * cycle du pwm.
 *
 * Paramètre :
 * duty_in_ms : Le nouveau rapport cyclique du pwm en millisecondes.
 *
 * Valeur retournée : aucune
 */
void update_pwm_duty_cycle(double duty_in_ms)
{
	FILE *pwmHandle = NULL;
	double duty_ns = duty_in_ms * MS_TO_NS_FACTOR;
	double dummy = duty_ns;
	char str[12];
	int number_of_bytes_to_write = 0;

	char *pwmDuty_ns   = &(std::string(getenv("MOTORPATH"))+std::string("/pwm0/duty_cycle"))[0u];

	pwmHandle = fopen(pwmDuty_ns,"r+");

	number_of_bytes_to_write = sprintf(str,"%.0lf",dummy);

	fwrite(str,sizeof(char),number_of_bytes_to_write,pwmHandle);


	fclose(pwmHandle);


}
/*
 * Fonction qui permet de changer la valeur de la peéiode
 * du pwm.
 *
 * Paramètre :
 * period_in_ms : La nouvelle période du pwm en millisecondes.
 *
 * Valeur retournée : aucune
 */
void update_pwm_period_cycle(double period_in_ms)
{
	FILE *pwmHandle = NULL;
	double period_ns = period_in_ms * MS_TO_NS_FACTOR;
	double dummy = period_ns;
	char str[12];
	char *pwmPeriod_ns = &(std::string(getenv("MOTORPATH"))+std::string("/pwm0/period"))[0u];
	int number_of_bytes_to_write = 0;

	pwmHandle = fopen(pwmPeriod_ns,"r+");

	number_of_bytes_to_write = sprintf(str,"%.0lf",dummy);
	fwrite(str,sizeof(char),number_of_bytes_to_write,pwmHandle);
	fclose(pwmHandle);

}
/*
 * Fonction qui permet de changer la période ainsi
 * que le rapport cyclique du pwm.
 *
 * Paramètre :
 * period_in_ms : La nouvelle période du pwm en millisecondes.
 * duty_in_ms : Le nouveau rapport cyclique du pwm en millisecondes.
 *
 * Valeur retournée : aucune
 *
 */
void update_pwm(double period_in_ms,double duty_in_ms)
{

		//stop_pwm();
		update_pwm_period_cycle(period_in_ms);
		update_pwm_duty_cycle(duty_in_ms);
		//start_pwm();
}

/*
 * Fonction qui permet de lire la valeur 12 bits
 * lu sur le module ADC numéro 0 du Beaglebone
 *
 * Paramètre : aucun
 *
 * Valeur retournée : Valeur lu sur le ACD sur 12 bits
 *
 */
double read_adc(void)
{
	FILE *adcHandler;
	long lSize;
	char *buffer;
	//Pin P9.39
	char* adc0_loc = getenv("ADC_0_value");;
	size_t result;

	adcHandler = fopen(adc0_loc,"r");
	if(adcHandler == NULL)
	{
		//fputs("ADC0 not found ",stderr);
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
	double value = atoi(buffer);

	fclose(adcHandler);
	free(buffer);

	return value ;
}
/*
 * Fonction permettant de lire l'état de la pin
 * P9_12 sur le Beaglebone
 *
 * Paramètre : aucun
 *
 * Valeur retourné : État de la pin (1 ou 0)
 */
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
	fclose(gpioHandler);
	free(buffer);
	return value ;
}
/*
 * Fonction qui crée une fonction de premier degré à l'aide de
 * deux points (x1,y1) , (x2,y2)
 *
 * Paramètre :
 * y1 : Coordonée y du premier point
 * y2 : Coordonée y du deuxième point
 * x1 : Coordonée x du premier point
 * x2 : Coordonée x du deuxième point
 * parameter : structure dans lequel la pente ainsi que l'origine
 * de la fonction seront accessible.
 *
 * Valeur retourné : aucune
 */
void slope_maker(double y1 , double y2 , double x1, double x2,curve_args* parameter)
{

	parameter->slope = (y2-y1)/(x2-x1);
	parameter->origin = y1 - x1 * parameter->slope;


}
/*
 * Fonction qui permet de remplir un filtre numérique
 * pour la première fois
 *
 * Paramètre :
 * table : tableau contenant les valeurs échantillonées
 * parameter : structure qui contient les modifications à faire sur les valeurs
 * fill : fonction à utiliser pour remplir le tableau
 * nb_element : nb d'éléments à échantillonées
 *
 * Valeur retourné : aucune
 *
 */
void first_time_fill_filter(double *table,curve_args* parameter,fill_filter_fct fill,double nb_element)
{
	for(int i = 0;i < nb_element; i++)
	{

		table[i] = fill();
		table[i]*= parameter->slope;
		table[i] += parameter->origin;
	}
}

/*
 * Fonction qui permet de décaler tous les éléments d'un
 * filtre numérique et qui retourne la valeur filtrée
 *
 * Parametre :
 * table : tableau contenant les valeurs échantillonées
 * nb_element : nb d'éléments échantillonées
 *
 * Valeur retournée : valeur filtrée
 */
double filter_shifter(double table[],double nb_element)
{
	double sum = 0;
	for(int j =0;j<nb_element;j++)
		sum += table[j];

	for(int i = nb_element - 1 ; i > 0;i--)
		table[i] = table[i-1];
	return (sum/nb_element);
}
