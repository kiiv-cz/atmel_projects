/*
  wiring_digital.c - digital input and output functions
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  Modified 28 September 2010 by Mark Sproul
*/

#define ARDUINO_MAIN
#include "wiring_private.h"
#include "pins_arduino.h"

void pinMode(uint8_t pin, uint8_t mode)
{
  uint8_t bit_mask = digitalPinToBitMask(pin);

  if ((bit_mask == NOT_A_PIN) || (mode > INPUT_PULLUP))
    return;

  PORT_t *port = digitalPinToPortStruct(pin);
  if (port == NULL)
    return;

  if (mode == OUTPUT)
  {
    /* Configure direction as output */
    port->DIRSET = bit_mask;
  }
  else
  { /* mode == INPUT or INPUT_PULLUP */

    uint8_t bit_pos = digitalPinToBitPosition(pin);
    /* Calculate where pin control register is */
    volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(port, bit_pos);

    /* Save state */
    uint8_t status = SREG;
    cli();

    /* Configure direction as input */
    port->DIRCLR = bit_mask;

    /* Configure pull-up resistor */
    if (mode == INPUT_PULLUP)
    {
      /* Enable pull-up */
      *pin_ctrl_reg |= PORT_PULLUPEN_bm;
    }
    else
    { /* mode == INPUT (no pullup) */

      /* Disable pull-up */
      *pin_ctrl_reg &= ~(PORT_PULLUPEN_bm);
    }

    /* Restore state */
    SREG = status;
  }
}

// Forcing this inline keeps the callers from having to push their own stuff
// on the stack. It is a good performance win and only takes 1 more byte per
// user than calling. (It will take more bytes on the 168.)
//
// But shouldn't this be moved into pinMode? Seems silly to check and do on
// each digitalread or write. One issue that then needs to be fixed is that
// current implementation on analogWrite() depends on this behaviour.
//
// Mark Sproul:
// - Removed inline. Save 170 bytes on atmega1280
// - changed to a switch statment; added 32 bytes but much easier to read and maintain.
// - Added more #ifdefs, now compiles for atmega645
//
//static inline void turnOffPWM(uint8_t timer) __attribute__ ((always_inline));
//static inline void turnOffPWM(uint8_t timer)
static void turnOffPWM(uint8_t pin)
{
  /* Actually turn off compare channel, not the timer */

  /* Get pin's timer */
  uint8_t timer = digitalPinToTimer(pin);
  if (timer == NOT_ON_TIMER)
    return;

  uint8_t bit_pos;
  TCB_t *timerB;

  switch (timer)
  {
    /* TCA0 */
    case TIMERA0:
      /* Bit position will give output channel */
      bit_pos = digitalPinToBitPosition(pin);

      /* Disable corresponding channel */
      if (bit_pos >= 3) ++bit_pos; /* Upper 3 bits are shifted by 1 */
      TCA0.SPLIT.CTRLB &= ~(1 << (TCA_SPLIT_LCMP0EN_bp + bit_pos));

      break;

    /* TCB - only one output */
    case TIMERB0:
    case TIMERB1:
    case TIMERB2:
    case TIMERB3:

      timerB = (TCB_t *)&TCB0 + (timer - TIMERB0);

      /* Disable TCB compare channel */
      timerB->CTRLB &= ~(TCB_CCMPEN_bm);

      break;
    default:
      break;
  }
}

void digitalWrite(uint8_t pin, uint8_t val)
{
  /* Get bit mask for pin */
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN)
    return;

  /* Turn off PWM if applicable */

  // If the pin that support PWM output, we need to turn it off
  // before doing a digital write.
  turnOffPWM(pin);

  /* Assuming the direction is already output !! */

  /* Get port */
  PORT_t *port = digitalPinToPortStruct(pin);

  /* Output direction */
  if (port->DIR & bit_mask)
  {
    /* Set output to value */
    if (val == LOW)
    { /* If LOW */
      port->OUTCLR = bit_mask;
    }
    else if (val == CHANGE)
    { /* If TOGGLE */
      port->OUTTGL = bit_mask;
      /* If HIGH OR  > TOGGLE  */
    }
    else
    {
      port->OUTSET = bit_mask;
    }

    /* Input direction */
  }
  else
  {
    /* Old implementation has side effect when pin set as input -
    pull up is enabled if this function is called.
    Should we purposely implement this side effect?
    */

    /* Get bit position for getting pin ctrl reg */
    uint8_t bit_pos = digitalPinToBitPosition(pin);

    /* Calculate where pin control register is */
    volatile uint8_t *pin_ctrl_reg = getPINnCTRLregister(port, bit_pos);

    /* Save system status and disable interrupts */
    uint8_t status = SREG;
    cli();

    if (val == LOW)
    {
      /* Disable pullup */
      *pin_ctrl_reg &= ~PORT_PULLUPEN_bm;
    }
    else
    {
      /* Enable pull-up */
      *pin_ctrl_reg |= PORT_PULLUPEN_bm;
    }

    /* Restore system status */
    SREG = status;
  }
}

inline __attribute__((always_inline)) void digitalWriteFast(uint8_t pin, uint8_t val)
{
  // Make sure pin is constant and know at compile time
  //check_constant_pin(pin);

  // Mega-0, Tiny-1 style IOPORTs
  // Assumes VPORTs exist starting at 0 for each PORT structure
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;

  // Write pin value from VPORTx.OUT register
  vport = (VPORT_t *)(port * 4);

  if (val == HIGH)
    vport->OUT |= mask;
  else if (val == LOW)
    vport->OUT &= ~mask;
  else // CHANGE
    vport->IN = mask;
}

uint8_t digitalRead(uint8_t pin)
{
  /* Get bit mask and check valid pin */
  uint8_t bit_mask = digitalPinToBitMask(pin);
  if (bit_mask == NOT_A_PIN)
    return LOW;

  // If the pin that support PWM output, we need to turn it off
  // before getting a digital reading.
  turnOffPWM(pin);

  /* Get port and check valid port */
  PORT_t *port = digitalPinToPortStruct(pin);

  /* Read pin value from PORTx.IN register */
  if (port->IN & bit_mask)
    return HIGH;
  else
    return LOW;

  return LOW;
}

inline __attribute__((always_inline)) uint8_t digitalReadFast(uint8_t pin)
{
  // Make sure pin is constant and know at compile time
  //check_constant_pin(pin);

  // Mega-0, Tiny-1 style IOPORTs
  // Assumes VPORTs exist starting at 0 for each PORT structure
  uint8_t mask = 1 << digital_pin_to_bit_position[pin];
  uint8_t port = digital_pin_to_port[pin];
  VPORT_t *vport;

  // Old style port logic is a small integer 0 for PORTA, 1 for PORTB etc.
  vport = (VPORT_t *)(port * 4);

  // Read pin value from VPORTx.IN register
  return !!(vport->IN & mask);
}
