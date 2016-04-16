#include "encoder_pru_drv.h"

#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <math.h>

#define ENCODER_DEBUG

#define RADIUS_OF_ENCODER_IN_MM	26

/*
 * Fonction permettant de convertir la vitesse angulaire lu
 * par l'encodeur en vitesse linéaire
 *
 * Paramètre :
 * radius_in_millimeter : rayon de la roue de l'encodeur en millimètres
 * time_in_us : Temps requis pour faire un tour de roue
 *
 * Valeur retournée : la vitesse linéaire
 *
 */
double convert_angular_to_linear_speed(double radius_in_millimeter,double time_in_us)
{
	double linear_speed = 0;
	linear_speed = (2*M_PI)/(time_in_us*0.000001);
	linear_speed*= (radius_in_millimeter*0.001);
	return linear_speed;

}

/*
 * Fonction qui initialise l'encodeur
 */
void init_encoder(void)
{
	/* Initialize the PRU */
#ifdef ENCODER_DEBUG
	printf("Initializing PRU\n");
#endif
	tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
	prussdrv_init();
	/* Open PRU Interrupt */
	if(prussdrv_open(PRU_EVTOUT_1))
	{
		// Handle failure
#ifdef ENCODER_DEBUG
		fprintf(stderr, "prussdrv_open open failed\n");
#endif
		exit(2);
	}
	/* Get the interrupt initialized */
	prussdrv_pruintc_init(&pruss_intc_initdata);
#ifdef ENCODER_DEBUG
	printf("Finishing initializing PRU ... \n");
#endif

}
/*
 * Fonction qui retourne une vitesse linéaire selon
 * la lecture de l'encodeur
 *
 * Paramètre : aucun
 *
 * Valeur retournée : la vitesse linéaire
 *
 */
double read_linear_speed(void)
{
	init_encoder();
	/* Get pointers to PRU local memory */
	void *pruDataMem;
	double vitesse = 0;
	prussdrv_map_prumem (PRUSS0_PRU1_DATARAM, &pruDataMem);
	unsigned int *pruData = (unsigned int *) pruDataMem;
#ifdef ENCODER_DEBUG
	/* Execute example on PRU */
	printf("Executing encoder pru code\n");
#endif
	prussdrv_exec_program(PRU1, "encoder.bin");
#ifdef ENCODER_DEBUG

	printf("Waiting to get the interrupt\n");
#endif

	// Wait for the PRU interrupt to occur
	prussdrv_pru_wait_event (PRU_EVTOUT_1);
	prussdrv_pru_clear_event (PRU_EVTOUT_1,PRU1_ARM_INTERRUPT);

	vitesse = (double) pruData[1];
	vitesse = convert_angular_to_linear_speed(RADIUS_OF_ENCODER_IN_MM,(double)pruData[1]);

#ifdef ENCODER_DEBUG

	/* Check if example passed */
	printf("Executed succesfully.\r\n");

	printf("Vitesse = %f \n",vitesse);
#endif


	/* Disable PRU and close memory mapping*/
	prussdrv_pru_disable (1);
	//prussdrv_exit ();
	return ((double) vitesse);

}

