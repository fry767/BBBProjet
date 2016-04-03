#include "lm74_drv.h"
#include <stdio.h>
#include <stdlib.h>
/*
* Written by Derek Molloy for the book "Exploring BeagleBone: Tools and
* Techniques for Building with Embedded Linux" by John Wiley & Sons, 2014
* ISBN 9781118935125. Please see the file README.md in the repository root
* directory for copyright and GNU GPLv3 license information. */

#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

static const char *device = "/dev/spidev1.0";
unsigned char tx[2],rx[2];

void lm74_init(void)
{
	uint8_t mode = 3, bits = 8;
	uint32_t speed = 500000;
	int file;

	if((file = open(device,O_RDWR))<0)
	{
		perror("SPI : CANT OPEN DEVICE");
		exit(1);
	}

	if(ioctl(file,SPI_IOC_WR_MODE,&mode)==-1)
	{
		perror("SPI : CANT SET MODE");
		exit(1);
	}

	if (ioctl(file, SPI_IOC_RD_MODE, &mode)==-1)
	{
		perror("SPI: Can't get SPI mode.");
		exit(1);
	}
	if (ioctl(file, SPI_IOC_WR_BITS_PER_WORD, &bits)==-1)
	{
		perror("SPI: Can't set bits per word.");
		exit(1);
	}
	if (ioctl(file, SPI_IOC_RD_BITS_PER_WORD, &bits)==-1)
	{
		perror("SPI: Can't get bits per word.");
		exit(1);
	}
	if (ioctl(file, SPI_IOC_WR_MAX_SPEED_HZ, &speed)==-1)
	{
		perror("SPI: Can't set max speed HZ");
		exit(1);
	}
	if (ioctl(file, SPI_IOC_RD_MAX_SPEED_HZ, &speed)==-1)
	{
		perror("SPI: Can't get max speed HZ.");
		exit(1);
	}
	printf("spi mode: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
	close(file);
}
double read_temperature(void)
{
	int file;
	tx[0] = (0x00|0x00);
	double temperature = 0;
	double flag_lsb=0;
	 struct spi_ioc_transfer transfer = {
			 .tx_buf = (unsigned long) tx,
			 .rx_buf = (unsigned long) rx,
			 .len = 2,
	};

	if((file = open(device,O_RDWR))<0)
	{
		perror("SPI : CANT OPEN DEVICE");
		exit(1);
	}


	if (ioctl(file,SPI_IOC_MESSAGE(1),&transfer)<0)
	{
		perror("Failed to send SPI message");
		exit(1);
	}
	uint8_t MSB = rx[0],LSB = rx[1];
	LSB = LSB >>3;

	if(LSB & 0b00000001 == 1)
		flag_lsb = 1;

	LSB = LSB >> 1;
	if(MSB & 0b10000000 == 1)
	{
		MSB = ~MSB;
		MSB += 1;
		LSB = ~LSB;
		LSB += 1;
	}
	LSB = LSB >>3;
	temperature = (MSB << 1)|LSB;
	if(flag_lsb)
		temperature +=0.0625;
	printf("Temperature = %f \n",temperature);
	close(file);
	return temperature;



}
