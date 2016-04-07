// Standard includes
// PRUSS interface library
#include <hcsr04.h>

//#define HCSR04_DEBUG
void init_hcsr04(void)
{
	/* Initialize the PRU */
#ifdef HCSR04_DEBUG
	printf("Initializing PRU\n");
#endif
	tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
	prussdrv_init();
	/* Open PRU Interrupt */
	if(prussdrv_open(PRU_EVTOUT_0))
	{
		// Handle failure
#ifdef HCSR04_DEBUG
		fprintf(stderr, "prussdrv_open open failed\n");
#endif
		exit(2);
	}
	/* Get the interrupt initialized */
	prussdrv_pruintc_init(&pruss_intc_initdata);
#ifdef HCSR04_DEBUG
	printf("Finishing initializing PRU ... \n");
#endif

}
double read_distance(void)
{
	/* Get pointers to PRU local memory */
	void *pruDataMem;
	prussdrv_map_prumem (PRUSS0_PRU0_DATARAM, &pruDataMem);
	unsigned int *pruData = (unsigned int *) pruDataMem;
	double reel_distance =0;
#ifdef HCSR04_DEBUG
	/* Execute example on PRU */
	printf("Executing sonar pru code\n");
#endif
	prussdrv_exec_program(0, "hcsr04_demo.bin");
#ifdef HCSR04_DEBUG

	printf("Waiting to get the interrupt\n");
#endif

	int incrementation =0;
	//while(incrementation++<10)
	//{
		// Wait for the PRU interrupt to occur
		prussdrv_pru_wait_event (PRU_EVTOUT_0);
		prussdrv_pru_clear_event (PRU_EVTOUT_0,PRU0_ARM_INTERRUPT);
	//for(double i =0;i<60001;i++){}
		//Print out the distance received from the sonar (sound takes 58.77 microseconds to travel 1 cm at sea level in dry air)
	//	printf("Distance = %f cm\n", (float) pruData[1] / 58.77);
	//}
#ifdef HCSR04_DEBUG

	/* Check if example passed */
	printf("Executed succesfully.\r\n");
#endif
	/* Disable PRU and close memory mapping*/
	//prussdrv_pru_disable (0);
	//prussdrv_exit ();
	reel_distance = (double) pruData[1];
	reel_distance *=0.79;
	return (reel_distance / 58.77);

}

