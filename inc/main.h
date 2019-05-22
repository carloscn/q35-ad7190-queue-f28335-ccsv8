
#ifndef INC_MAIN_H_
#define INC_MAIN_H_

// ADC start parameters
#define CPU_FRQ_150MHZ   1
#if (CPU_FRQ_150MHZ)     // Default - 150 MHz SYSCLKOUT
  #define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz
#endif
#if (CPU_FRQ_100MHZ)
  #define ADC_MODCLK 0x2 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 100/(2*2)   = 25.0 MHz
#endif
#define ADC_CKPS   0x3   // ADC module clock = HSPCLK/2*ADC_CKPS   = 25.0MHz/(1*2) = 12.5MHz
#define ADC_SHCLK  0xf   // S/H width in ADC module periods                        = 16 ADC clocks
#define AVG        1000  // Average sample limit
#define ZOFFSET    0x00  // Average Zero offset
#define BUF_SIZE   128  // Sample buffer size
extern Uint16 Key;
interrupt void adc_isr(void);
extern void lcd_api_put(unsigned char x, unsigned char y);
__interrupt void tint0_isr(void);
#endif /* INC_MAIN_H_ */
