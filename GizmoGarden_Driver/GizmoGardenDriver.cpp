/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "GizmoGardenDriver.h"

// ***********************************
// *                                 *
// *  GizmoGardenDriver Constructor  *
// *                                 *
// ***********************************

GizmoGardenDriver::GizmoGardenDriver
  (GizmoGardenRotatingMotor& leftWheel, GizmoGardenRotatingMotor& rightWheel,
   int leftSensorPin, int rightSensorPin)
{
  wheels[0] = &leftWheel;
  wheels[1] = &rightWheel;
  sensorPins[0] = (uint8_t)leftSensorPin;
  sensorPins[1] = (uint8_t)rightSensorPin;
  for (int i = 0; i < 2; ++i)
    scale[i] = 0;
  kp = 60;
}

void GizmoGardenDriver::setFullSpeed(int v)
{
  wheels[0]->setFullSpeed( v);
  wheels[1]->setFullSpeed(-v);
}

#ifdef TASK_MONITOR
GizmoGardenText GizmoGardenDriver::name() const
{
  return F("Driver"); 
}
#endif

float GizmoGardenDriver::getStiffness()
{
  return kp / 50.0f;
}

void GizmoGardenDriver::setStiffness(float k)
{
  kp = (uint8_t)constrain(k * 50 + 0.5f, 5.0f, 250.0f);
}

// ***********************************
// *                                 *
// *  GizmoGardenDriver Calibration  *
// *                                 *
// ***********************************

bool GizmoGardenDriver::calibrate()
{
  int16_t bright[2];
  for (int s = 0; s < 2; ++s)
  {
    int sum = 0;
    for (int i = 0; i < 32; ++i)
      sum += analogRead(sensorPins[s]);
    bright[s] = sum >> 5;
  }
  
  setFullSpeed(100);
  for (int s = 0; s < 2; ++s)
  {
    wheels[s]->setSpeed(100);
    dark[s] = 0;
  }
  delay(1000);
  
  for (int i = 0; i < 32; ++i)
  {
    for (int s = 0; s < 2; ++s)
      dark[s] += analogRead(sensorPins[s]);
    delay(60);
  }
  
  bool ok = true;
  int16_t range[2];
  for (int s = 0; s < 2; ++s)
  {
    wheels[s]->setSpeed(0);
    dark[s] >>= 5;
    range[s] = bright[s] - dark[s];
    if (ggAbs(range[s]) > 128)
      scale[s] = (int16_t)((1l << 22) / range[s]);
    else
    {
      scale[s] = 0;
      ok = false;
    }
  }

  return ok;
}

// ***************************************
// *                                     *
// *  GizmoGardenDriver Data for EEPROM  *
// *                                     *
// ***************************************

void GizmoGardenDriver::getData(GizmoGardenDriverData& dd)
{
  dd.validKey = GizmoGardenDriverData::ValidKey;
  dd.propK  = (uint8_t)getStiffness();
  for (int s = 0; s < 2; ++s)
  {
    dd.dark [s] = dark [s];
    dd.scale[s] = scale[s];
  }
}

void GizmoGardenDriver::setData(const GizmoGardenDriverData& dd)
{
  if (dd.isValid())
  {
    setStiffness(dd.propK );
    for (int s = 0; s < 2; ++s)
    {
      dark [s] = dd.dark [s];
      scale[s] = dd.scale[s];
    }
  }
}

bool GizmoGardenDriverData::operator==(const GizmoGardenDriverData& dd)
{
  return objectsEqual(*this, dd);
}

int GizmoGardenDriver::getRange(int s)
{
  return scale[s] > 0 ? (int)((1l << 22) / scale[s]) : 0;
}

// *******************************
// *                             *
// *  GizmoGardenDriver Startup  *
// *                             *
// *******************************

void GizmoGardenDriver::customStart()
{
  for (int s = 0; s < 2; ++s)
    smoothedOnRoad[s] = 1 << (7 + 4);
  
  spinMode = 0;
}

void GizmoGardenDriver::customStop()
{
  for (int s = 0; s < 2; ++s)
    wheels[s]->setSpeed(0);
}

void GizmoGardenDriver::spin()
{
  setFullSpeed(100);
  wheels[0]->setSpeed( 100);
  wheels[1]->setSpeed(-100);
  spinMode = 1;
}

// ************************************
// *                                  *
// *  GizmoGardenDriver Control Loop  *
// *                                  *
// ************************************

void GizmoGardenDriver::myTurn()
{
  int16_t onRoad[2];  // s16.7
  for (int s = 0; s < 2; ++s)
  {
    int16_t sense = analogRead(sensorPins[s]);
    onRoad[s] = (int16_t)((int32_t)(sense - dark[s]) * (int32_t)scale[s] >> 15);
  }
  int16_t diff = onRoad[0] - onRoad[1]; // s16.7

  if (spinMode == 0)
  {
    for (int s = 0; s < 2; ++s)
    {
      int16_t sot = smoothedOnRoad[s];    // s16.11
      smoothedOnRoad[s] = sot - (sot >> 4) + onRoad[s];
    }
      
    // If the largest sensor reading is below 0.5 of on-road, assume
    // we've run off the road and stop. customStop will be called to
    // set speed to 0.
    if (max(smoothedOnRoad[0], smoothedOnRoad[1]) < (1 << 10))
      return;
    
    int16_t speed = 100, turn = 0;
    if (getFullSpeed() >= 0)
    {
      speed = 84 + (min(onRoad[0], onRoad[1]) >> (7 - 4));
      turn = (kp * diff) >> 6;
    }
   
    wheels[0]->setSpeed(speed - max(turn, 0));
    wheels[1]->setSpeed(speed + min(turn, 0));

    callMe(20);
  }
  else
  {
    bool nearRoad = onRoad[1] >= 64;    // 0.5 of on-road
    switch (spinMode)
    {
    case 1:
      spinMode = 2;
      callMe(250);
      break;

    case 2:
      if (nearRoad)
      {
        spinMode = 3;
        callMe(250);
      }
      else
        callMe(50);
      break;

    case 3:
      if (!nearRoad)
      {
        spinMode = 4;
        callMe(250);
      }
      else
        callMe(50);
      break;

    case 4:
      if (nearRoad)
        spinMode = 5;
      callMe(50);
      break;

    case 5:
      if (diff >= 0)
      {
        spinMode = 0;
        callMe(0);
      }
      else
        callMe(20);
      break;
    }
  }
}
