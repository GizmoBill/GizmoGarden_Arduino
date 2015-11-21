// ************************************************************************
// *                                                                      *
// *  Gizmo Garden Control of Adafruit NeoPixels for Servo Compatibility  *
// *                                                                      *
// ************************************************************************

#include "GizmoGardenPixels.h"

// This mirrors the code in micros(). We need access to the count register
// of timer 0, which the Arduino system uses for general timing and sets
// up with prescaler = 64.
#if defined(TCNT0)
  #define TCNT TCNT0
#elif defined(TCNT0L)
  #define TCNT TCNT0L
#else
	#error TIMER 0 not defined
#endif

GizmoGardenPixels::GizmoGardenPixels(uint16_t numPixels, uint8_t pin)
: Adafruit_NeoPixel(numPixels, pin)
{
}

// To satisfy NeoPixl timing requirements, we need to guarantee that the start
// of one showInterrupt() is at least 50 us later than the end of the previous.
// Adafruit uses calls to micros() to guarantee the timing without wasting time
// in the typical case when no delay is necessary. But here callback() must run
// entirely with interrupts off, and micros() can return values that are not
// monotonically creasing when called with interrupts off, when they have
// been off for longer than the timer 0 period of 1024 us. Using micros() here
// could under rare but possible cases cause a delay of around 1 ms when only
// 50 us is needed.
//
// Instead of micros(), we use the 8-bit hardware counter in timer 0. Since the
// counter only gives time modulo 256, we can delay by up to 50 us when in fact
// no delay is necessary. But 50 us is worst case; the expected value of the
// unnecessary delay is only about 1.25 us, which is negligable, and we avoid
// the possibility of a much longer unnecessary delay.
//
// Note that there is no need to initialize endMark; any random initial value
// will do.

void GizmoGardenPixels::callback()
{
  // This works because it is an unsigned comparison. Note that 50 us converted
  // to timer ticks with integer math truncates to 48 us, but surely there is at
  // least 2 us overhead between calls to show().
  while ((uint8_t)(TCNT - endMark) < (uint8_t)(50 * clockCyclesPerMicrosecond() / 64));
  showInterrupt();
  endMark = TCNT;
}

void GizmoGardenPixels::show()
{
  call();
}

void GizmoGardenPixels::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
  cancel();
  Adafruit_NeoPixel::setPixelColor(n, r, g, b);
}

void GizmoGardenPixels::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
  cancel();
  Adafruit_NeoPixel::setPixelColor(n, r, g, b, w);
}

void GizmoGardenPixels::setPixelColor(uint16_t n, uint32_t c)
{
  cancel();
  Adafruit_NeoPixel::setPixelColor(n, c);
}

void GizmoGardenPixels::setBrightness(uint8_t z)
{
  cancel();
  Adafruit_NeoPixel::setBrightness(z);
}

void GizmoGardenPixels::clear()
{
  cancel();
  Adafruit_NeoPixel::clear();
}
