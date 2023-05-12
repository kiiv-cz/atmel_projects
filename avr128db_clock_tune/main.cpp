/*
 * avr128db_clock_tune.cpp
 *
 * Board: Curiosity NANO AVR128DB48
 *
 * Created: 12.05.2023 18:34:10
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/xmega.h>
#include <util/delay.h>


int main(void) {
    // setup clock so delays are correct
    _PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA, CLKCTRL_FREQSEL_16M_gc | CLKCTRL_AUTOTUNE_bm); 
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKOUT_bm);

    // start Xtal oscillators
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL_ENABLE_bm | CLKCTRL_RUNSTBY_bm | CLKCTRL_LPMODE_bm | CLKCTRL_CSUT_1K_gc);
    _PROTECTED_WRITE(CLKCTRL.XOSCHFCTRLA, CLKCTRL_ENABLE_bm | CLKCTRL_RUNSTBY_bm| CLKCTRL_FRQRANGE_16M_gc | CLKCTRL_CSUTHF_4K_gc);

    _delay_ms(10);
    if (CLKCTRL.MCLKSTATUS & CLKCTRL_EXTS_bm) { // 16MHz xtal is running
        _PROTECTED_WRITE(CLKCTRL.MCLKCTRLC, CLKCTRL_CFD_bm | CLKCTRL_CFDSRC_CLKMAIN_gc);
        _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKOUT_bm | CLKCTRL_CLKSEL_EXTCLK_gc);
    }

    
    PORTB.DIRSET = PIN3_bm; // PB3 onboard led 
   
    
    while (1) {
        VPORTB.IN = PIN3_bm;
        _delay_ms(500);
    }
}

