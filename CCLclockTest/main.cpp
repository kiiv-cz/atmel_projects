/*
 * CCLclockTest.cpp
 *
 * Created: 28.03.2022 19:39:30
 * Author : mikus
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t encoder = 0;

ISR(PORTF_PORT_vect) {
    if (PORTF.INTFLAGS & PIN2_bm) {
        PORTF.PIN2CTRL = PORT_PULLUPEN_bm;
        PORTF.PIN3CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
        PORTF.INTFLAGS = PIN2_bm;
        
        if (VPORTF.IN & PIN3_bm) {
            ++encoder;
        } else {
            --encoder;
        }
    }
    if (PORTF.INTFLAGS & PIN3_bm) {
        PORTF.PIN2CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
        PORTF.PIN3CTRL = PORT_PULLUPEN_bm;
        PORTF.INTFLAGS = PIN3_bm;
    }
}

int main() {
    PORTF.DIRSET   = PIN5_bm;
    PORTF.PIN2CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    PORTF.PIN3CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    PORTF.PIN6CTRL = PORT_PULLUPEN_bm;
    
    sei();

    uint8_t last_encoder = encoder;
    while (1) {
        if (encoder != last_encoder) {
            last_encoder = encoder;
            
            if (encoder & 1) {
                PORTF.OUTSET = PIN5_bm;
            } else {
                PORTF.OUTCLR = PIN5_bm;
            }
        }
        
    }
}


/*
int main(void) {
    // EVOUTF == PF2 (output), PF5 output, PF6 input pullup:
    //PORTF.OUTCLR = PIN2_bm ;
    
    PORTA.DIRSET = PIN3_bm;
    PORTA.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTF.DIRSET   = PIN2_bm | PIN5_bm;
    //PORTF.PIN2CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    //PORTF.PIN3CTRL = PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    PORTF.PIN6CTRL = PORT_PULLUPEN_bm;
    

    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    //RTC.CTRLA = RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm | RTC_RUNSTDBY_bm;
    RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc | RTC_PITEN_bm | RTC_RUNSTDBY_bm;
    RTC.PITDBGCTRL = RTC_DBGRUN_bm;

    
    PORTMUX.EVSYSROUTEA = PORTMUX_EVOUTF_bm;

    EVSYS.CHANNEL6      = EVSYS_GENERATOR_RTC_PIT0_gc;   // PIT / 64 -> 16Hz
    EVSYS.CHANNEL4      = EVSYS_GENERATOR_PORT1_PIN6_gc;
    
    EVSYS.CHANNEL7      = EVSYS_GENERATOR_CCL_LUT0_gc;
    EVSYS.USEREVOUTF    = EVSYS_CHANNEL_CHANNEL7_gc;

    EVSYS.USERCCLLUT0A  = EVSYS_CHANNEL_CHANNEL4_gc;  // PF6 -> EVENTA LUT0
    EVSYS.USERCCLLUT0B  = EVSYS_CHANNEL_CHANNEL6_gc;


    CCL.LUT0CTRLB       = CCL_INSEL0_EVENTA_gc | CCL_INSEL1_MASK_gc;
    CCL.LUT0CTRLC       = CCL_INSEL2_EVENTB_gc;
    CCL.TRUTH0          = 0b01010101;
    CCL.LUT0CTRLA       = CCL_FILTSEL_FILTER_gc | CCL_CLKSRC_IN2_gc | CCL_ENABLE_bm; // | CCL_OUTEN_bm;  
    
    CCL.CTRLA           = CCL_ENABLE_bm;
 

    while (1) {
        if (RTC.PITINTFLAGS & RTC_PI_bm) {
            RTC.PITINTFLAGS = RTC_PI_bm; // clear the flag
            PORTF.OUTTGL = PIN5_bm;
            PORTA.OUTTGL = PIN3_bm;
        }
        
        //if (VPORTF_IN & PIN6_bm) {
        //    PORTF.OUTSET = PIN5_bm;
        //} else {
        //    PORTF.OUTCLR = PIN5_bm;
        //}
    }
}

*/