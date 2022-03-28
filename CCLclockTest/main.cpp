/*
 * CCLclockTest.cpp
 *
 * Created: 28.03.2022 19:39:30
 * Author : mikus
 */ 

#include <avr/io.h>


int main(void) {
    // EVOUTF == PF2 (output), PF5 output, PF6 input pullup:
    PORTF.DIRSET = PIN2_bm | PIN5_bm;
    PORTF.PIN6CTRL = PORT_PULLUPEN_bm;

    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
    //RTC.DBGCTRL = RTC_DBGRUN_bm;
    //RTC.CTRLA = RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm | RTC_RUNSTDBY_bm;
    RTC.PITCTRLA = RTC_PERIOD_CYC1024_gc | RTC_PITEN_bm;
    RTC.PITDBGCTRL = RTC_DBGRUN_bm;

    PORTMUX.EVSYSROUTEA = PORTMUX_EVOUTF_bm;
    EVSYS.CHANNEL0      = EVSYS_GENERATOR_RTC_PIT3_gc;   // PIT / 8192
    EVSYS.CHANNEL4      = EVSYS_GENERATOR_PORT1_PIN6_gc; // PF6 -> F=PORT1
    //EVSYS.USEREVOUTF    = EVSYS_CHANNEL_CHANNEL4_gc;
    EVSYS.USEREVOUTF    = EVSYS_CHANNEL_CHANNEL0_gc;

    while (1) {
        if (RTC.PITINTFLAGS & RTC_PI_bm) {
            RTC.PITINTFLAGS = RTC_PI_bm; // clear the flag
            PORTF.OUTTGL = PIN5_bm;
        }
        /*
        if (VPORTF_IN & PIN6_bm) {
            PORTF.OUTSET = PIN5_bm;
        } else {
            PORTF.OUTCLR = PIN5_bm;
        }*/
    }
}

