#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/xmega.h>


int main(void) {
    
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm);

    PORTF.DIRSET = PIN5_bm;

    TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
    TCA0.SINGLE.PER = 25000-1;
    TCA0.SINGLE.CMP0 = 12500;

    sei();

    while(1);
}

ISR(TCA0_CMP0_vect) {
  PORTF.IN = PIN5_bm;
  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm;
}

