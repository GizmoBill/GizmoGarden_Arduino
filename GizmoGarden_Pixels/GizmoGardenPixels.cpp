// ************************************************************************
// *                                                                      *
// *  Gizmo Garden Control of Adafruit NeoPixels for Servo Compatibility  *
// *                                                                      *
// ************************************************************************

#include "GizmoGardenPixels.h"

GizmoGardenPixels::GizmoGardenPixels(uint16_t numPixels, uint8_t pin)
: Adafruit_NeoPixel(numPixels, pin)
{
}

void GizmoGardenPixels::callback()
{
  showInterrupt();
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
