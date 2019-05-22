/*
 * spi.h
 *
 *  Created on: Dec 8, 2018
 *      Author: root
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "DSP28x_Project.h"

void spi_init( void );
void spi_read_n_bytes(uint32_t n, unsigned char *buf);
void spi_write_n_bytes(uint32_t n, unsigned char *buf);
unsigned char spi_read_byte(void);
void spi_write_byte(unsigned char e);
#endif /* INC_SPI_H_ */
