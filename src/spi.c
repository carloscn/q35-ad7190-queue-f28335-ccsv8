/*
 * spi.c
 *
 *  Created on: Dec 8, 2018
 *      Author: root
 */

#include "spi.h"


// GPIO57   - CS
// GPIO56   - sclk
// GPIO55   - OUT
// GPIO54   - IN

#define             SET_CS()                GpioDataRegs.GPBDAT.bit.GPIO57 = 1
#define             CLR_CS()                GpioDataRegs.GPBDAT.bit.GPIO57 = 0
#define             SET_SCL()               GpioDataRegs.GPBDAT.bit.GPIO56 = 1
#define             CLR_SCL()               GpioDataRegs.GPBDAT.bit.GPIO56 = 0
#define             SET_SDO()               GpioDataRegs.GPBDAT.bit.GPIO54 = 1
#define             CLR_SDO()               GpioDataRegs.GPBDAT.bit.GPIO54 = 0
#define             READ_SDI()              GpioDataRegs.GPBDAT.bit.GPIO55
#define             D_TIME                  20
#define    SPI_P
unsigned char spi_tx_ready(void)
{
    if (SpiaRegs.SPISTS.bit.BUFFULL_FLAG == 1)
        return 0;
    else
        return 1;
}

unsigned char spi_rx_ready(void)
{
    if (SpiaRegs.SPISTS.bit.INT_FLAG == 1)
        return 1;
    else
        return 0;
}
void spi_pin_init(void)
{
#ifdef SPI_P
    EALLOW;
    GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0;
    GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;
    GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0;
    GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;


    GpioCtrlRegs.GPBDIR.bit.GPIO54 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO55 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;


    GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;
    EDIS;
    GpioDataRegs.GPBDAT.bit.GPIO54 = 0;
    GpioDataRegs.GPBDAT.bit.GPIO55 = 0;
    GpioDataRegs.GPBDAT.bit.GPIO56 = 0;
    GpioDataRegs.GPBDAT.bit.GPIO57 = 0;
#else

#endif
}

void spi_read_n_bytes(uint32_t n, unsigned char *buf)
{
    uint32_t i = 0, j = 0;
    unsigned char rotate_data = 0;
    unsigned char temp = 0;

    SET_SCL();
    DELAY_US(D_TIME);
    SET_CS();
    DELAY_US(D_TIME);
    CLR_CS();
    DELAY_US(D_TIME);
    for (j = n; j > 0; j-- ) {
        for (i = 0; i < 8; i ++) {
            CLR_SCL();
            rotate_data <<= 1;
            DELAY_US(D_TIME);
            temp = READ_SDI();
            SET_SCL();
            //if (0x20 == (temp & 0x20))
            rotate_data |= temp;
            DELAY_US(D_TIME);
        }
        *(buf + j - 1) = rotate_data;
    }
    SET_CS();


}

void spi_write_n_bytes(uint32_t n, unsigned char *buf)
{
    uint32_t i = 0, j = 0;
    unsigned char value_to_write = 0;

    for (i = 0; i < n; i ++) {
        spi_write_byte(*(buf + i));
    }

    SET_SCL();
    DELAY_US(D_TIME);
    SET_CS();
    DELAY_US(D_TIME);
    CLR_CS();
    DELAY_US(D_TIME);

    for (i = n; i > 0; i --) {
        value_to_write = *(buf + i - 1);
        for (j = 0; j < 8; j ++) {
            CLR_SCL();
            if (0x80 == (value_to_write & 0x80))
                SET_SDO();
            else
                CLR_SDO();
            DELAY_US(D_TIME);
            SET_SCL();
            DELAY_US(D_TIME);
            value_to_write <<= 1;
        }
    }
    SET_CS();

}

unsigned char spi_wirte_read(unsigned char e)
{
    uint32_t i = 0;
    unsigned char rotate_data = 0;
#ifdef SPI_P
    SET_SCL();
    DELAY_US(D_TIME);
    SET_CS();
    DELAY_US(D_TIME);
    CLR_CS();
    DELAY_US(D_TIME);
    for (i = 0; i < 8; i ++) {
        CLR_SCL();
        if (0x80 == (e & 0x80))
            SET_SDO();
        else
            CLR_SDO();
        rotate_data <<= 1;
        DELAY_US(10);
        rotate_data |= READ_SDI()?1:0;
        e <<= 1;
        SET_SCL();
        DELAY_US(10);
    }
    SET_CS();
#else
    spi_write_byte(0x5C);
    while(spi_rx_ready() != 1);
    rotate_data = SpiaRegs.SPIRXBUF;

#endif
    return rotate_data;

}

void spi_write_byte(unsigned char e)
{
    uint16_t j = 0;
    unsigned char value_to_write;
#ifdef SPI_P
    value_to_write = e;

    SET_SCL();
    DELAY_US(D_TIME);
    SET_CS();
    DELAY_US(D_TIME);
    CLR_CS();
    DELAY_US(D_TIME);
    for (j = 0; j < 8; j ++) {
        CLR_SCL();
        if (0x80 == (value_to_write & 0x80))
            SET_SDO();
        else
            CLR_SDO();
        DELAY_US(D_TIME);
        SET_SCL();
        DELAY_US(D_TIME);
        value_to_write <<= 1;
    }
    DELAY_US(D_TIME);
    SET_CS();
#else
    SpiaRegs.SPITXBUF = e;
    while(spi_tx_ready() != 1);


#endif
}

unsigned char spi_read_byte(void)
{
    uint32_t i = 0;
    unsigned char rotate_data = 0;
#ifdef SPI_P
    SET_SCL();
    DELAY_US(D_TIME);
    SET_CS();
    DELAY_US(D_TIME);
    CLR_CS();
    DELAY_US(D_TIME);
    for (i = 0; i < 8; i ++) {
        CLR_SCL();
        rotate_data <<= 1;
        DELAY_US(10);
        rotate_data |= READ_SDI()?1:0;
        SET_SCL();
    }
    SET_CS();
#else
    spi_write_byte(0x5C);
    while(spi_rx_ready() != 1);
    rotate_data = SpiaRegs.SPIRXBUF;

#endif
    return rotate_data;
}

void spi_init( void )
{
#ifdef SPIP
    InitSpi();
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;
    SpiaRegs.SPICCR.bit.SPILBK = 0;
    SpiaRegs.SPICCR.bit.SPICHAR = 0x0F;
    SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0;
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpiaRegs.SPICTL.bit.CLK_PHASE = 1;
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;
    SpiaRegs.SPICTL.bit.TALK = 1;
    SpiaRegs.SPIBRR = 100;  // 1M
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;
#else

    spi_pin_init();
#endif
    //spi_pin_init();

}



