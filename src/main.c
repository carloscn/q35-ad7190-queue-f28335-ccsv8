//###########################################################################
// Description:
//! \addtogroup f2833x_example_list
//!  <h1>SCI Digital Loop Back with Interrupts (scia_loopback_interrupts)</h1>
//!
//! This program uses the internal loop back test mode of the peripheral.
//! Other then boot mode pin configuration, no other hardware configuration
//! is required. Both interrupts and the SCI FIFOs are used.
//
//###########################################################################
// $TI Release: F2833x/F2823x Header Files and Peripheral Examples V141 $
// $Release Date: November  6, 2015 $
// $Copyright: Copyright (C) 2007-2015 Texas Instruments Incorporated -
//     http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "project.h"   // Device Headerfile and Examples Include File
#include "main.h"
#include "math.h"



#ifdef FLASH
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
#endif

float adc_result[100];
uint32_t adc_count = 0;
float weight_proportion = 31114;
float weight_base  = 0;
void main(void)

{
    Uint16 i = 0;

    //queue = new_queue(BUF_SIZE);
    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the DSP2833x_SysCtrl.c file.
    InitSysCtrl();
#ifdef  FLASH
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    // Call Flash Initialization to setup flash waitstates
    // This function must reside in RAM
    InitFlash();
#endif
    // Step 2. Initialize GPIO:
    // This example function is found in the DSP2833x_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    InitGpio();
    // Setup only the GP I/O only for SCI-A and SCI-B functionality
    // This function is found in DSP2833x_Sci.c

    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    DINT;
    // Initialize PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the DSP2833x_PieCtrl.c file.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in DSP2833x_DefaultIsr.c.
    // This function is found in DSP2833x_PieVect.c.
    InitPieVectTable();
    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.SCIRXINTA = &sciaRxFifoIsr;
    PieVectTable.SCIRXINTB = &scibRxFifoIsr;
    PieVectTable.SCIRXINTC = &scicRxFifoIsr;
    PieVectTable.TINT0 = &tint0_isr;
    EDIS; // This is needed to disable write to EALLOW protected registers
    // Enable ADCINT in PIE
    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0,150,100000);
    SCI_INIT();
    spi_init();
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1; // Enable the PIE block
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1;   // PIE Group 9, int1
    PieCtrlRegs.PIEIER9.bit.INTx3 = 1;   // PIE Group 9, INT3
    PieCtrlRegs.PIEIER9.bit.INTx5 = 1;   // PIE Group 9, INT3
    PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1; // timer 0

    IER |= 0x100 | M_INT1 ; // Enable CPU INT
    EINT;

    ad7190_init();
    memset(adc_result, 0, 100);


    CpuTimer0Regs.TCR.bit.TSS = 0;

    // Step 6. IDLE loop. Just sit and loop forever (optional):
    for(;;){

    }
}

__interrupt void tint0_isr(void)
{
    adc_result[adc_count++] = ((float)weight_ad7190_ReadAvg(1))/1117.0f;
    if (adc_count == 100) {
        adc_count = 0;
        //memset(adc_result, 0, 100);
    }
    CpuTimer0Regs.TCR.bit.TIF = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1 | PIEACK_GROUP7; // Acknowledge interrupt to PIE
    EINT;
}
