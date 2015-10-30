/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include <Arduino.h>
#include "GizmoGardenTone.h"

// You can use a different timer by changing these macros
#define TCNT  TCNT2
#define OCRA  OCR2A
#define TCCRA TCCR2A
#define TCCRB TCCR2B
#define TIFR  TIFR2
#define TIMSK TIMSK2
#define TVEC  TIMER2_COMPA_vect
#define OCIEA OCIE2A

int8_t pins[2];
int8_t volume = 2;
volatile uint8_t* pinOutputs[2];
uint8_t pinMasks[2];
uint32_t count;

void GizmoGardenToneBegin(int pin1, int pin2)
{
  pins[0] = pin1;
  pins[1] = pin2;
  volume = 2;

  for (int i = 0; i < 2; ++i)
  {
    pinMode(pins[i], OUTPUT);
    pinOutputs[i] = portOutputRegister(digitalPinToPort(pins[i]));
    pinMasks[i] = digitalPinToBitMask(pins[i]);
  }

  // Two of the bits for Fast PWM, and set normal port operation with
  // OC2A/OC2B disconnected.
  TCCRA  = _BV(WGM20) | _BV(WGM21);
}

void stopTone()
{
  // Disable the timer interrupt.
  TIMSK &= ~_BV(OCIEA);

  // Set outputs low so no stress on piezo
  for (int i = 0; i < 2; ++i)
    *pinOutputs[i] &= ~pinMasks[i];
}

int GizmoGardenGetVolume()
{
  return volume;
}

void GizmoGardenSetVolume(int volume)
{
  ::volume = (int8_t)constrain(volume, 0, 2);
}

const uint8_t prescaleTable[] PROGMEM = { 0, 3, 5, 6, 7, 8, 10 };

void GizmoGardenTone(uint16_t frequency, uint16_t duration)
{
  stopTone();
  if (frequency == 0)
    return;

  // Find smallest prescale value that can generate the frequency
  uint8_t prescale, top;
  for (prescale = 0; prescale < 7; ++prescale)
  {
    // We need interrupts at twice the frequency. Get twice the number
    // of timer ticks, then round off.
    uint32_t ticks = (F_CPU >> pgm_read_byte(&prescaleTable[prescale])) / frequency;
    ticks = (ticks + 1) >> 1;
    if (ticks <= 256)
    {
      top = ticks - 1;
      goto frequencyOK;
    }
  }
  return;

frequencyOK:
  // Load timer registers
  OCRA   = top;
  TCNT = 0;
  TCCRB  = _BV(WGM22) | (prescale + 1); // Third bit for Fast PWM, prescale.

  // Compute count
  count = (uint32_t)duration * frequency / 500;

  // Enable timer interrupt.
  TIMSK |= _BV(OCIEA);                  
}

ISR(TVEC)
{
  if (--count == 0)
    stopTone();
  else
  {
    uint8_t state = count & 1;

    // Hopefully the compiler will unroll this loop
    for (int i = 0; i < 2; ++i)
    {
      uint8_t output = *pinOutputs[i] & ~pinMasks[i];
      if ((state ^ i ) != 0 && volume > i)
        output |= pinMasks[i];
      *pinOutputs[i] = output;
    }
  }
}
