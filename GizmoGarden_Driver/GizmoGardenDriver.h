#ifndef _GizmoGardenDriver_
#define _GizmoGardenDriver_

/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "../GizmoGarden_Common/GizmoGardenCommon.h"
#include "../GizmoGarden_Motors/GizmoGardenMotors.h"
#include "../GizmoGarden_Multitasking/GizmoGardenMultitasking.h"

// ***********************
// *                     *
// *  GizmoGardenDriver  *
// *                     *
// ***********************

// This struct is intended for collecting that portion of the
// GizmoGardenDriver class that is saved/restored from EEPROM
struct GizmoGardenDriverData
{
  enum { ValidKey = 0x37 };

  // These are copies of fields from GizmoGardenDriver. See below
  // for definitions.
  uint8_t    validKey;
  uint8_t    propK;
  int16_t    dark[2];
  uint16_t   scale[2];

  bool operator==(const GizmoGardenDriverData&);
  bool operator!=(const GizmoGardenDriverData& dd) { return !(*this == dd); }

  bool isValid() const { return validKey == ValidKey; }
};

class GizmoGardenDriver : public GizmoGardenTask
{
public:
  // Construct with specified motors and sensor pins
  GizmoGardenDriver(GizmoGardenRotatingMotor& leftWheel,
                    GizmoGardenRotatingMotor& rightWheel,
                    int leftSensorPin, int rightSensorPin);
  
  // Start and stop driving
  virtual void customStart();
  virtual void customStop();

  // Get/set the proportional constant of the control loop.
  // [0.1 .. 5.0]
  float getStiffness();
  void setStiffness(float k);
  
  // Calibrate sensors
  bool calibrate();
  bool isCalibrated() { return scale[0] != 0 || scale[1] != 0; }

  // Get calibration data for display. The dark value is the off-road
  // raw sensor reading; range is the difference between on-road and
  // off-road, also raw sensor, positive for bright roads, negative for
  // dark roads.
  int16_t getDark(int s) { return dark[s]; }
  int16_t getRange(int s);

  // Get smoothed sensor readings. s16.7, where 0 is the off-road value,
  // 1 is the on-road value. The returned value can fall beyond that range.
  int16_t getSmoothedOnRoad(int s) { return smoothedOnRoad[s] >> 4; }

  // Get/set full speed value, controls overall speed of gizmo. Same
  // units as Motor class
  int getFullSpeed() { return wheels[0]->getFullSpeed(); }
  void setFullSpeed(int v);

  // Get/set EEPROM data. Set will do nothing on invalid data (e.g. uninitialized
  // EEPROM).
  void getData(GizmoGardenDriverData&);
  void setData(const GizmoGardenDriverData&);

  void spin();
  bool isSpinning() const { return spinMode != 0; }
  
  DECLARE_TASK_NAME

private:
  // index 0 is left wheel, 1 is right wheel

  uint8_t sensorPins[2];                // sensor pins
  GizmoGardenRotatingMotor* wheels[2];  // wheels

  // Proportional control constant (stiffness).
  uint8_t kp;
  
  // Sensor calibration. dark is the off-road reading in raw sensor units.
  // scale is 1/(on-road - off-road), s16.22. scale is positive for bright
  // roads and negative for dark roads. The largest scale magnitude is
  // 2^(15-22) = 1/128, so (on-road - off-road) must be > 128. We use
  // the reciprocal to avoid doing a divide in the control loop.
  int16_t dark[2];
  int16_t scale[2];

  // smoothed sensor readings, s16.11, 0 is the off-road value, 1 is the
  // on-road value, value can fall beyond that range.
  int16_t smoothedOnRoad[2];
  
  // For spinning in place and then recovering the road. spinMode is 0
  // when not spinning, assumes small positive state values when spinning.
  uint8_t spinMode;
  
  virtual void myTurn();
};

#endif
