// Standard includes
// PRUSS interface library
#include <hcsr04.h>
void init_hcsr04(void)
{
	/* Initialize the PRU */
	printf("Initializing PRU\n");
	tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
	prussdrv_init();
	/* Open PRU Interrupt */
	if(prussdrv_open(PRU_EVTOUT_0))
	{
		// Handle failure
		fprintf(stderr, "prussdrv_open open failed\n");
		exit(2);
	}
	/* Get the interrupt initialized */
	prussdrv_pruintc_init(&pruss_intc_initdata);

	printf("Finishing initializing PRU ... \n");

}
float read_distance(void)
{
	/* Get pointers to PRU local memory */
	void *pruDataMem;
	prussdrv_map_prumem (PRUSS0_PRU0_DATARAM, &pruDataMem);
	unsigned int *pruData = (unsigned int *) pruDataMem;
	/* Execute example on PRU */
	printf("Executing sonar pru code\n");
	prussdrv_exec_program(0, "hcsr04_demo.bin");
	/* Get 1000 measurements */
	printf("Waiting to get the interrupt\n");
	// Wait for the PRU interrupt to occur
	int incrementation =0;
	while(incrementation++<100)
	{
		prussdrv_pru_wait_event (PRU_EVTOUT_0);
		prussdrv_pru_clear_event (PRU_EVTOUT_0,PRU0_ARM_INTERRUPT);
		// Print out the distance received from the sonar (sound takes 58.77 microseconds to travel 1 cm at sea level in dry air)
		printf("Distance = %f cm\n", (float) pruData[1] / 58.77);
	}



	/* Check if example passed */
	printf("Executed succesfully.\r\n");
	/* Disable PRU and close memory mapping*/
	//prussdrv_pru_disable (0);
	//prussdrv_exit ();
	return ((float) pruData[1] / 58.77);

}

