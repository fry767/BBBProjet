#ifndef __HCSR04_H_INCLUDED__
#define __HCSR04_H_INCLUDED__



#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

void init_hcsr04(void);
double read_distance(void);



#endif
