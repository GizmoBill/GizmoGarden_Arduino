/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "GizmoGardenMotors.h"

// ************************************
// *                                  *
// *  GizmoGardenRotatingMotor Class  *
// *                                  *
// ************************************

#if TABLE_ACCESS == 2
uint8_t GizmoGardenRotatingMotor::speedTable[sizeof(speedTable)];

void GizmoGardenRotatingMotor::setSpeedTable(double amplitude, double sigma)
{
  for (int i = 0; i < sizeof(speedTable); ++i)
  {
    double x = i / sigma;
    int v = (int)(amplitude * log((1 + x) / (1 - x)));
    speedTable[i] = (uint8_t)::min(v, 200);
  }
}
#else

#if TABLE_ACCESS == 0
const uint8_t GizmoGardenRotatingMotor::speedTable[sizeof(speedTable)] =
#else
const uint8_t GizmoGardenRotatingMotor::speedTable[sizeof(speedTable)] PROGMEM =
#endif
{
  // 35.0, 157.0
    0,   0,   1,   1,   2,   2,   3,   3,   4,   4, 
    4,   5,   5,   6,   6,   7,   7,   8,   8,   9, 
    9,   9,  10,  10,  11,  11,  12,  12,  13,  13, 
   14,  14,  14,  15,  15,  16,  16,  17,  17,  18, 
   18,  19,  19,  20,  20,  21,  21,  22,  22,  23, 
   23,  24,  24,  25,  25,  26,  26,  27,  27,  28, 
   28,  29,  29,  30,  30,  31,  31,  32,  32,  33, 
   34,  34,  35,  35,  36,  36,  37,  38,  38,  39, 
   39,  40,  41,  41,  42,  42,  43,  44,  44,  45, 
   46,  46,  47,  48,  48,  49,  50,  51,  51,  52, 
   53,  53,  54,  55,  56,  57,  57,  58,  59,  60, 
   61,  62,  63,  63,  64,  65,  66,  67,  68,  69, 
   70,  72,  73,  74,  75,  76,  77,  79,  80,  81, 
   83,  84,  86,  87,  89,  90,  92,  94,  96,  98, 
  100, 102, 105, 107, 110, 113, 116, 120, 123, 128, 
  132, 138, 144, 152, 162, 177, 200
};
#endif

GizmoGardenRotatingMotor::GizmoGardenRotatingMotor(int pin, int fs)
: GizmoGardenServo(pin, 1300, 1700)
{
  vel_ = 0;
  fullSpeed_ = (int8_t)constrain(fs, -100, 100);

#if TABLE_ACCESS == 2
  if (speedTable[sizeof(speedTable) - 1] == 0)
    setSpeedTable(35.0, 157.0);
#endif
}

void GizmoGardenRotatingMotor::setFullSpeed(int fs)
{
  fullSpeed_ = (int8_t)constrain(fs, -100, 100);
  setSpeed(getSpeed());
}

void GizmoGardenRotatingMotor::setZero(int z)
{
  zero_ = (int8_t)constrain(z, -25, 25);
  setSpeed(getSpeed());
}

void GizmoGardenRotatingMotor::setSpeed(int v)
{
  vel_ = (int8_t)constrain(v, -100, 100);
  int t = (getSpeed() * getFullSpeed() + (1 << 5)) >> 6;

#if TABLE_ACCESS != 1
  if (t >= 0)
    t = speedTable[t];
  else
    t = -speedTable[-t];
#else
  if (t >= 0)
    t = pgm_read_byte(&speedTable[t]);
  else
    t = -pgm_read_byte(&speedTable[-t]);
#endif

  writeMicroseconds(1500 + t + zero_);
}

// ***************************************
// *                                     *
// *  GizmoGardenPositioningMotor Class  *
// *                                     *
// ***************************************

GizmoGardenPositioningMotor::GizmoGardenPositioningMotor(int pin, bool flipDirection, int offset)
: GizmoGardenServo(pin)
{
  this->flipDirection = flipDirection;
  this->offset = (int8_t)offset;
  position = 0;
}

void GizmoGardenPositioningMotor::setOffset(int x)
{
  offset = (int8_t)constrain(x, -90, 90);
  setPosition(getPosition());
}

void GizmoGardenPositioningMotor::setPosition(int pos)
{
  position = (int8_t)constrain(pos, -128, 127);
  pos = constrain(position - offset, -90, 90);
  write((flipDirection ? -pos : pos) + 90);
}

// ************
// *          *
// *  Dancer  *
// *          *
// ************

GizmoGardenDancer::GizmoGardenDancer
  (GizmoGardenRotatingMotor& leftWheel, GizmoGardenRotatingMotor& rightWheel,
   uint16_t baseTime)
  : leftWheel(leftWheel), rightWheel(rightWheel), baseTime(baseTime),
    GizmoGardenTask(false)
{
}

#ifdef TASK_MONITOR
GizmoGardenText GizmoGardenDancer::name() const
{
  return F("Dancer"); 
}
#endif

void GizmoGardenDancer::dance(GizmoGardenDanceMove* dm, GizmoGardenText durations)
{
  stop();
  this->dm = dm;
  this->durations = durations;
  start();
}

void GizmoGardenDancer::myTurn()
{
  leftWheel .setSpeed(dm->leftSpeed );
  rightWheel.setSpeed(dm->rightSpeed);
  uint16_t time = getMusicTime(durations, baseTime);
  if (time > 0)
  {
    ++dm;
    callMe(time);
  }
}

// **************
// *            *
// *  Gestures  *
// *            *
// **************

GizmoGardenGestures::GizmoGardenGestures
  (GizmoGardenPositioningMotor& motor, uint16_t baseTime)
  : motor(motor), baseTime(baseTime), GizmoGardenTask(false)
{}

#ifdef TASK_MONITOR
GizmoGardenText GizmoGardenGestures::name() const
{
  return F("Gestures"); 
}
#endif

void GizmoGardenGestures::play(const GizmoGardenGestureAngle* angles,
                               GizmoGardenText times)
{
  stop();
  this->angles = angles;
  this->times = times;
  start();
}

void GizmoGardenGestures::myTurn()
{
  int angle = *angles++;
  if (angle != GestureKeep)
    motor.setPosition(angle);
  uint16_t time = getMusicTime(times, baseTime);
  if (time != 0)
    callMe(time);
}
