#define F_CPU 20000000UL

#include <avr/io.h>
#include <avr/xmega.h>
#include <util/delay.h>

#include <avr/pgmspace.h>


PROGMEM const char pgmString[] = "xyz";

const char constString[] = "abcd";


int main(void) {
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKOUT_bm | CLKCTRL_CLKSEL_OSC20M_gc);
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0);
    //
    PORTB.DIRSET = PIN5_bm;

    while (1) {
        _delay_ms(500);
        
        volatile uint16_t sum = (uint16_t)&pgmString;
        sum = (uint16_t)&constString;
        (void) sum;
        //VPORTB.IN = PIN5_bm; // toggle
    }
}

