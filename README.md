# atmel_projects
Experiments with event system, CCL, timers/RTC, main clock

* EvSystemTest   - Attiny416 Xplained nano board, remapped serial, EVSYS, stdout + printf funcionality, PIT ISR, TCB0 ISR
* rtc_atmega4809 - Atmega4809 Curiosity nano board, RTC xtal oscillator (soldered 32k xtal + 2x 10pF), utilized PIT/RTC flags without ISR, PIT to PF2 output through EVSYS
* Tiny212_int32k_evout - Attiny212 (bare), Clock swithed to internal 32k oscillator -> 15.4uA active consumption (without LEDs), PIT, EVSYS


