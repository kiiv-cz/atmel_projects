#define F_CPU 33400UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h>
#include <avr/xmega.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

int main(void) {
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCULP32K_gc); // Switch the main clock to internal 32kHz oscillator
    //_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm); // Prescaler enable /4
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, 0); // disable prescaller

    // RTC config:
    RTC.CLKSEL       = RTC_CLKSEL_INT32K_gc;
    RTC.PITCTRLA     = RTC_PITEN_bm | RTC_PERIOD_CYC32768_gc;

    // PA3, PA6, PA7 input disabled
    PORTA.PIN3CTRL   = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN6CTRL   = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN7CTRL   = PORT_ISC_INPUT_DISABLE_gc;

    // PA1, PA2 as outputs:
    PORTA.DIRSET     = PIN1_bm | PIN2_bm;          // set output pins (for debugging)
    PORTA.OUTSET     = PIN1_bm;

    // EventSystem + output to EVOUT0
    PORTMUX.CTRLA    = PORTMUX_EVOUT0_bm;
    EVSYS.ASYNCCH3   = EVSYS_ASYNCCH3_PIT_DIV2048_gc;
    EVSYS.ASYNCUSER8 = EVSYS_ASYNCUSER8_ASYNCCH3_gc;

    /* Replace with your application code */
    while (1) {
        PORTA.OUTTGL = PIN1_bm;
        _delay_ms(50);
    }
}
