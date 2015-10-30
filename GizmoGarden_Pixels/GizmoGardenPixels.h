#ifndef _GizmoGardenPixels_
#define _GizmoGardenPixels_

// ************************************************************************
// *                                                                      *
// *  Gizmo Garden Control of Adafruit NeoPixels for Servo Compatibility  *
// *                                                                      *
// ************************************************************************

#include "../Adafruit_NeoPixel_GizmoGardenModified/Adafruit_NeoPixel_GizmoGardenModified.h"
#include "../GizmoGarden_Servo/GizmoGardenServo.h"

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
};
#endif
