#ifndef _GizmoGardenPixels_
#define _GizmoGardenPixels_

// ************************************************************************
// *                                                                      *
// *  Gizmo Garden Control of Adafruit NeoPixels for Servo Compatibility  *
// *                                                                      *
// ************************************************************************

#include "../Adafruit_NeoPixel_GizmoGardenModified/Adafruit_NeoPixel_GizmoGardenModified.h"
#include "../GizmoGarden_Servo/GizmoGardenServo.h"

struct NeoPixelColor
{
  uint8_t red, green, blue;

  NeoPixelColor();
  NeoPixelColor(uint8_t red, uint8_t green, uint8_t blue)
    : red(red), green(green), blue(blue)
  {}

  operator uint32_t() { return Adafruit_NeoPixel::Color(red, green, blue); }
};

class GizmoGardenPixels : public Adafruit_NeoPixel, private ServoCallback
{
public:
  GizmoGardenPixels(uint16_t numPixels, uint8_t pin);

  void show();
  void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
  void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
  void setPixelColor(uint16_t n, uint32_t c);
  void setBrightness(uint8_t);
  void clear();

protected:
  virtual void callback();

private:
  // This is used like endTime in the Adafruit version, to guarantee that at least 50 us
  // separates the end of one show() and the beginning of the next. But the theory of
  // operation is somewhat different, and explained in GizmoGardenPixels.cpp.
  uint8_t endMark;
};
#endif
