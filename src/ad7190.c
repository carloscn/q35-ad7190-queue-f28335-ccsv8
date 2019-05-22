/*
 * ad7190.c
 *
 *  Created on: Dec 8, 2018
 *      Author: root
 */

#include "ad7190.h"

inline void ad7190_write_to_chip(uint32_t n, unsigned char *buf)
{
    spi_write_n_bytes(n, buf);
}

inline void ad7190_read_from_chip(uint32_t n, unsigned char *buf)
{
    spi_read_n_bytes(n, buf);
}

void ad7190_init(void)
{
    unsigned char write_buffer[4];
    unsigned char read_buffer[4];

#if 0
    write_buffer[0] = 0x10;
    ad7190_write_to_chip(1, write_buffer);

    // write com register 0x10 to control the progress to write configuration register.
    write_buffer[2] = 0x00;
    write_buffer[1] = 0x01;
    write_buffer[0] = 0x08;
    ad7190_write_to_chip(3, write_buffer);

    // write communication register.
    write_buffer[0] = 0x08;
    ad7190_write_to_chip(1, write_buffer);

    // write mode register internal 4.92MHz clock ,output data rate = 4.7Hz
    write_buffer[2] = 0x08;
    write_buffer[1] = 0x03;
    write_buffer[0] = 0xFF;
    ad7190_write_to_chip(3, write_buffer);

    // read config reg value.
    write_buffer[0] = 0x50;
    ad7190_write_to_chip(1, write_buffer);
    ad7190_read_from_chip(3, read_buffer);
    scia_msg(read_buffer);

    // read adc data value
    write_buffer[0] = 0x58;
    ad7190_write_to_chip(1, write_buffer);
    ad7190_read_from_chip(3, read_buffer);
    scia_msg(read_buffer);
#endif
    ad7190_reset();

    read_buffer[0] = AD7190_GetRegisterValue( AD7190_REG_ID, 1 );
    if ( (read_buffer[0] & AD7190_ID_MASK) != ID_AD7190 ) {
        weight_ad7190_conf();
    }
}

void ad7190_reset(void)
{
    unsigned char registerWord[7];
    uint8_t i;

    registerWord[0] = 0x01;
    registerWord[1] = 0xFF;
    registerWord[2] = 0xFF;
    registerWord[3] = 0xFF;
    registerWord[4] = 0xFF;
    registerWord[5] = 0xFF;
    registerWord[6] = 0xFF;
    for(i=0;i<7;i++)
        spi_write_byte(registerWord[i]);
    DELAY_US(1000);
}

void ad7190_read_data(uint32_t n, uint32_t *buffer)
{
    unsigned char read_buffer[4];
    uint32_t i = 0;
    uint16_t j = 0;
    uint32_t t = 0;

    for (i = 0; i > n; i ++) {
        read_buffer[0] = 0x58;
        ad7190_write_to_chip(1, read_buffer);
        ad7190_read_from_chip(3, read_buffer);
        for (j = 0; j < 3; j ++) {
            t <<= 8;
            t |= read_buffer[j];
        }
        *(buffer + i)  = t;
    }
}

uint32_t ad7190_read_a_data(void)
{
    unsigned char read_buffer[4];
    uint16_t j = 0;
    uint32_t t = 0;

    read_buffer[0] = 0x58;
    ad7190_write_to_chip(1, read_buffer);
    ad7190_read_from_chip(3, read_buffer);
    for (j = 0; j < 3; j ++) {
        t <<= 8;
        t |= read_buffer[j];
    }
    return t;
}

void AD7190_SetPower(unsigned char pwrMode)
{
    unsigned int oldPwrMode = 0x0;
    unsigned int newPwrMode = 0x0;

    oldPwrMode = AD7190_GetRegisterValue(AD7190_REG_MODE, 3);
    oldPwrMode &= ~(AD7190_MODE_SEL(0x7));
    newPwrMode = oldPwrMode | AD7190_MODE_SEL((pwrMode * (AD7190_MODE_IDLE)) | (!pwrMode * (AD7190_MODE_PWRDN)));
    AD7190_SetRegisterValue(AD7190_REG_MODE, newPwrMode, 3);
}

void AD7190_ChannelSelect(unsigned short channel)
{
    unsigned int oldRegValue = 0x0;
    unsigned int newRegValue = 0x0;

    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF, 3);
    oldRegValue &= ~(AD7190_CONF_CHAN(0xFF));
    newRegValue = oldRegValue | AD7190_CONF_CHAN(1 << channel);
    AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3);
}

void AD7190_Calibrate(unsigned char mode, unsigned char channel)
{
    unsigned int oldRegValue = 0x0;
    unsigned int newRegValue = 0x0;

    AD7190_ChannelSelect(channel);
    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_MODE, 3);
    oldRegValue &= ~AD7190_MODE_SEL(0x7);
    newRegValue = oldRegValue | AD7190_MODE_SEL(mode);
    //    WEIGHT_CS_ENABLE();
    AD7190_SetRegisterValue(AD7190_REG_MODE, newRegValue, 3); // CS is not modified.
    //    WEIGHT_CS_DISABLE();
}

void AD7190_RangeSetup(unsigned char polarity, unsigned char range)
{
    unsigned int oldRegValue = 0x0;
    unsigned int newRegValue = 0x0;

    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF,3);
    oldRegValue &= ~(AD7190_CONF_UNIPOLAR | AD7190_CONF_GAIN(0x7));
    newRegValue = oldRegValue | (polarity * AD7190_CONF_UNIPOLAR) | AD7190_CONF_GAIN(range) | AD7190_CONF_BUF;
    AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3);
}

unsigned int AD7190_SingleConversion(void)
{
    unsigned int command = 0x0;
    unsigned int regData = 0x0;

    command = AD7190_MODE_SEL(AD7190_MODE_SINGLE) | AD7190_MODE_CLKSRC(AD7190_CLK_INT) | AD7190_MODE_RATE(0x060);
    //    WEIGHT_CS_ENABLE();
    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3); // CS is not modified.
    regData = AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
    //    WEIGHT_CS_DISABLE();

    return regData;
}

unsigned int AD7190_ContinuousReadAvg(unsigned char sampleNumber)
{
    unsigned int samplesAverage = 0x0;
    unsigned char count = 0x0;
    unsigned int command = 0x0;

    command = AD7190_MODE_SEL(AD7190_MODE_CONT) | AD7190_MODE_CLKSRC(AD7190_CLK_INT) | AD7190_MODE_RATE(0x060);
    //    WEIGHT_CS_ENABLE();
    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3);
    for(count = 0;count < sampleNumber;count ++)
    {
        samplesAverage += AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
    }
    //    WEIGHT_CS_DISABLE();
    samplesAverage = samplesAverage / sampleNumber;

    return samplesAverage ;
}

unsigned int AD7190_TemperatureRead(void)
{
    unsigned char temperature = 0x0;
    unsigned int dataReg = 0x0;

    AD7190_RangeSetup(0, AD7190_CONF_GAIN_1);
    AD7190_ChannelSelect(AD7190_CH_TEMP_SENSOR);
    dataReg = AD7190_SingleConversion();
    dataReg -= 0x800000;
    dataReg /= 2815;   // Kelvin Temperature
    dataReg -= 273;    //Celsius Temperature
    temperature = (unsigned int) dataReg;

    return temperature;
}

void weight_ad7190_conf(void)
{
    unsigned int command = 0x0;
#ifndef AIN1P_AIN2N
    /* Calibrates channel AIN1(+) - AIN2(-). */
    AD7190_Calibrate(AD7190_MODE_CAL_INT_ZERO, AD7190_CH_AIN1P_AIN2M);
    /* Selects unipolar operation and ADC's input range to +-Vref/1. */
    AD7190_RangeSetup(0, AD7190_CONF_GAIN_128);
    AD7190_Calibrate(AD7190_MODE_CAL_INT_FULL,AD7190_CH_AIN1P_AIN2M);
    /* Performs a single conversion. */
    AD7190_ChannelSelect(AD7190_CH_AIN1P_AIN2M);
    command = AD7190_MODE_SEL(AD7190_MODE_CONT) | AD7190_MODE_CLKSRC(AD7190_CLK_INT) |\
            AD7190_MODE_RATE(102)|AD7190_MODE_SINC3;
    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3);
    AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
    AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
#else
    /* Calibrates channel AIN3(+) - AIN4(-). */
    AD7190_Calibrate(AD7190_MODE_CAL_INT_ZERO, AD7190_CH_AIN3P_AIN4M);
    /* Selects unipolar operation and ADC's input range to +-Vref/1. */
    AD7190_RangeSetup(0, AD7190_CONF_GAIN_128);
    AD7190_Calibrate(AD7190_MODE_CAL_INT_FULL,AD7190_CH_AIN3P_AIN4M);
    /* Performs a single conversion. */
    AD7190_ChannelSelect(AD7190_CH_AIN3P_AIN4M);
    command = AD7190_MODE_SEL(AD7190_MODE_CONT) | AD7190_MODE_CLKSRC(AD7190_CLK_INT) |\
            AD7190_MODE_RATE(1023)|AD7190_MODE_SINC3;
    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3);
    DELAY_US(10);
    AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
    DELAY_US(10);
    AD7190_GetRegisterValue(AD7190_REG_DATA, 3);
#endif
}

void AD7190_SetRegisterValue(unsigned char registerAddress,
                             unsigned int  registerValue,
                             unsigned char bytesNumber)
{
    unsigned char writeCommand[5] = {0, 0, 0, 0, 0};
    unsigned char* dataPointer    = (unsigned char*)&registerValue;
    unsigned char bytesNr         = bytesNumber;
    uint8_t i;

    writeCommand[0] = AD7190_COMM_WRITE | AD7190_COMM_ADDR(registerAddress);
    while(bytesNr > 0) {

        writeCommand[bytesNr] = *dataPointer;
        dataPointer ++;
        bytesNr --;
    }
    for(i=0;i<(bytesNumber+1);i++) {
        spi_write_byte(writeCommand[i]);
    }
}

unsigned int AD7190_GetRegisterValue(unsigned char registerAddress,
                                     unsigned char bytesNumber)
{
    unsigned char registerWord;
    unsigned char address         = 0;
    unsigned int  buffer          = 0x0;
    unsigned char i               = 0;

    address = AD7190_COMM_READ | AD7190_COMM_ADDR(registerAddress);
    spi_write_byte(address);

    for(i = 0; i < bytesNumber; i++) {
        registerWord = 0;
        registerWord = spi_read_byte();
        buffer = (buffer << 8) + registerWord;
    }
    return buffer;
}
void AD7190_WaitRdyGoLow(void)
{
    DELAY_US(50);
}
unsigned int weight_ad7190_ReadAvg(unsigned char sampleNumber)
{
    unsigned int samplesAverage = 0x0;

    samplesAverage = (AD7190_GetRegisterValue(AD7190_REG_DATA, 3)>>3) - 1510;

    return samplesAverage ;
}
