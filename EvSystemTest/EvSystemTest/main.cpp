/*
 * EvSystemTest.cpp
 *
 * Created: 09.02.2022 18:49:18
 * Author : mikus
 */ 
#define F_CPU (16000000UL / 2)

#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h>
#include <avr/xmega.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

#include <stdio.h>

static int uart_putchar(char c, FILE *);

static FILE mystdout; // uart_putchar, NULL,_FDEV_SETUP_WRITE);

void initStdout() {
    stdout = &mystdout;
    mystdout.buf = nullptr;
    mystdout.flags = _FDEV_SETUP_WRITE;
    mystdout.put = &uart_putchar;
    mystdout.udata = nullptr;
}

ISR(RTC_PIT_vect) {
    PORTB.OUTTGL = PIN5_bm;
    RTC.PITINTFLAGS = RTC_PI_bm;
}

ISR(TCB0_INT_vect) {
    //PORTB.OUTTGL = PIN5_bm;
    TCB0.INTFLAGS = TCB_CAPTEI_bm;
}

inline void USART0_init(void) {
    PORTA.DIRCLR = PIN2_bm;
    PORTA.DIRSET = PIN1_bm;
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(19200);
    USART0.CTRLB |= USART_TXEN_bm;
}


inline void USART0_sendChar(char c) {
    while (!(USART0.STATUS & USART_DREIF_bm)) {;}
    USART0.TXDATAL = c;
}

static int uart_putchar(char c, FILE *) {
    USART0_sendChar(c);
    return 0;
}


int main(void) {
    stdout = &mystdout;
    
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm ); // 16MHz/2    
    _PROTECTED_WRITE(CLKCTRL.OSC20MCALIBA, 0x1B);
    _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL_RUNSTDBY_bm | CLKCTRL_PEN_bm);
    
    _delay_ms(100);

    RTC.PITCTRLA   = RTC_PITEN_bm | RTC_PERIOD_CYC32768_gc;
    //RTC.PITINTCTRL = RTC_PI_bm;
    RTC.PITDBGCTRL = RTC_DBGRUN_bm;
    RTC.CTRLA      = RTC_RTCEN_bm | RTC_PRESCALER_DIV32768_gc | RTC_RUNSTDBY_bm;
    RTC.DBGCTRL    = RTC_DBGRUN_bm;

    if (bit_is_set(CLKCTRL.MCLKSTATUS, CLKCTRL_XOSC32KS_bm)) {
        RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;
    } else {
        RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
    }

    //PORTMUX.CTRLA = /*PORTMUX_EVOUT0_bm | PORTMUX_EVOUT1_bm |*/ PORTMUX_EVOUT2_bm; // #PA2 | PB2 | PC2
    PORTMUX.CTRLB = PORTMUX_USART0_ALTERNATE_gc;
    //PORTA.OUTSET  = PIN1_bm;
    //PORTA.DIRSET  = PIN1_bm;                     // usart0 TxD output

    PORTB.DIRSET  = PIN5_bm;
    PORTB.PIN4CTRL= PORT_PULLUPEN_bm;   // USER Button

    //PORTC.DIRSET  = PIN2_bm;            // EVOUT2

    // EVENT SYSTEM generators:
    //EVSYS.ASYNCCH1 = EVSYS_ASYNCCH1_PORTB_PIN4_gc;    // <- PB4 + pullup
    //EVSYS.ASYNCCH3 = EVSYS_ASYNCCH3_PIT_DIV256_gc;
    
    //EVSYS.ASYNCUSER0  = EVSYS_ASYNCUSER0_ASYNCCH3_gc;
    //EVSYS.ASYNCUSER8  = EVSYS_ASYNCUSER0_ASYNCCH1_gc; //n=8 EVOUT0 Event OUT 0      ->  PA2
    //EVSYS.ASYNCUSER9  = EVSYS_ASYNCUSER0_ASYNCCH3_gc; //n=9 EVOUT1 Event OUT 1      ->  PB2
    //EVSYS.ASYNCUSER10 = EVSYS_ASYNCUSER0_ASYNCCH3_gc; //n=10 EVOUT2 Event OUT 2     ->  PC2
    
    //TCB0_CTRLB  = TCB_CNTMODE_FRQ_gc;
    //TCB0.EVCTRL = TCB_CAPTEI_bm;
    //TCB0.CTRLA  = TCB_ENABLE_bm | TCB_CLKSEL_CLKDIV2_gc;
    //TCB0.INTCTRL= TCB_CAPTEI_bm;

    USART0_init();
    initStdout();

    sei();

    /* Replace with your application code */
    while (1) {
        //if (TCB0.INTFLAGS & TCB_CAPTEI_bm) {
            //TCB0.INTFLAGS = TCB_CAPTEI_bm;
        
            //printf("%05u\r\n", TCB0.CCMP);
        //}        
        
        _delay_ms(500);
        printf("%u\r\n", RTC.CNT);
    }
}

