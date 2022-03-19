/*
 * rtc_atmega4809.cpp
 *
 * Created: 13.03.2022 10:01:57
 * Author : mikus
 */ 

#include <avr/io.h>
#include <avr/xmega.h>

int8_t RTC_0_init();


int main(void) {
    //_PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, 0);
    //while(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm) { ; /* Wait until XOSC32KS becomes 0 */ }
    //_PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL_CSUT_1K_gc | CLKCTRL_RUNSTDBY_bm );
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL_CSUT_1K_gc | CLKCTRL_RUNSTDBY_bm | CLKCTRL_ENABLE_bm);

    RTC_0_init();
    
    
    EVSYS.CHANNEL1 = EVSYS_GENERATOR_RTC_PIT3_gc; // RTC clock/64
    EVSYS.USEREVOUTF = EVSYS_CHANNEL_CHANNEL1_gc;

    PORTF.DIRSET = PIN5_bm | PIN4_bm | PIN2_bm;

    while (1) {
        if (RTC.PITINTFLAGS & RTC_PI_bm) {
            RTC.PITINTFLAGS = RTC_PI_bm; // clear the flag
            PORTF.OUTTGL = PIN4_bm;
        }
        if (RTC.INTFLAGS & RTC_OVF_bm) {
            RTC.INTFLAGS = RTC_OVF_bm; // clear the flag
            PORTF.OUTTGL = PIN5_bm;
        }
    }
}


int8_t RTC_0_init(){

    while (RTC.STATUS > 0) { /* Wait for all register to be synchronized */ }

    RTC.CTRLA = RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm | RTC_RUNSTDBY_bm;
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;
    //RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
    RTC.DBGCTRL = RTC_DBGRUN_bm;

    while (RTC.PITSTATUS > 0) { /* Wait for all register to be synchronized */ }

    RTC.PITCTRLA = RTC_PERIOD_CYC16384_gc | RTC_PITEN_bm;
    RTC.PITDBGCTRL = RTC_DBGRUN_bm;


    return 0;
}