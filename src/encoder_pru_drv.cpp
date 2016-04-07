#include "encoder_pru_drv.h"

#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <math.h>

//#define ENCODER_DEBUG
#define RADIUS_OF_ENCODER_IN_MM	7
double convert_angular_to_linear_speed(double radius_in_millimeter,double time_in_us)
{
	double linear_speed = 0;
	linear_speed = (2*M_PI)/(time_in_us*0.000001);
	linear_speed*= (radius_in_millimeter*0.001);
	return linear_speed;

}
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

double read_linear_speed(void)
{
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

	int incrementation =0;
	//while(incrementation++<10)
	//{
		// Wait for the PRU interrupt to occur
		prussdrv_pru_wait_event (PRU_EVTOUT_1);
		prussdrv_pru_clear_event (PRU_EVTOUT_1,PRU1_ARM_INTERRUPT);
	//for(double i =0;i<60001;i++){}
		//Print out the distance received from the sonar (sound takes 58.77 microseconds to travel 1 cm at sea level in dry air)
	//	printf("Distance = %f cm\n", (float) pruData[1] / 58.77);
	//}

		vitesse = convert_angular_to_linear_speed(RADIUS_OF_ENCODER_IN_MM,pruData[1]);
		//A confirmer pour voir si le fix du capteur de distance est pareil pour l'encoder
		vitesse *= 0.79;
#ifdef ENCODER_DEBUG

	/* Check if example passed */
	printf("Executed succesfully.\r\n");

	printf("Vitesse = %f \n",vitesse);
#endif


	/* Disable PRU and close memory mapping*/
	//prussdrv_pru_disable (0);
	//prussdrv_exit ();
	return ((double) vitesse);

}

