#ifndef _GizmoGardenMotors_
#define _GizmoGardenMotors_

/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "../GizmoGarden_Common/GizmoGardenCommon.h"
#include "../GizmoGarden_Servo/GizmoGardenServo.h"
#include "../GizmoGarden_Multitasking/GizmoGardenMultitasking.h"

// ************************************
// *                                  *
// *  GizmoGardenRotatingMotor Class  *
// *                                  *
// ************************************

// The GizmoGardenRotatingMotor class encapsulates Servo in a friendlier
// interface for use with continuous rotation servos.
//
// Speed is from -100 to +100. 0 is stop, 100 is full speed in one direction,
// -100 is full speed in the other direction.
//
// Full speed is also a value from -100 to 100. 100 is the fastest the motor can
// turn in one direction, -100 is the fastest the motor can turn in the other
// direction. Setting full speed scales all velocity settings, like a master
// volume control. 
//
// Setting full speed negative negates all velocity settings. If you have a
// two-wheel robot, you can make one wheel have a positive full speed and the
// other negative, to account for their opposite directions of rotation. 
//
// The relationship between time and speed is shown in the Parallax manual
// on page 71 and looks like a pretty good match for the hyperbolic tangent.
// To convert speed to time we use an inverse tanh lookup table, parameterized
// emperically. Desired speed is derived from speed * fullSpeed, where 10,000
// is max motor speed. To avoid doing a divide (slow), we shift the product
// right 6, which gives a range after rounding of [-156 .. 156], so the
// table is 157 bytes. Here are the options for getting and storing the table.
// The table initializer is computed by Excel.
//   0    Initialized in flash, copied to SRAM
//   1    Initialized & read in flash (smallest flash and SRAM, because the
//        computation code is longer than the table)
//   2    Computed in SRAM
#define TABLE_ACCESS 1

class GizmoGardenRotatingMotor : private GizmoGardenServo
{
  int8_t vel_;            // Use signed 8-bit values to save memory
  int8_t fullSpeed_;

#if TABLE_ACCESS == 0
  static const uint8_t speedTable[157];
#elif TABLE_ACCESS == 1
  static const uint8_t speedTable[157] PROGMEM;
#else
  static uint8_t speedTable[157];
#endif

public:
  // Construct a Motor attached to the specified pin and with the specified
  // full speed.
  GizmoGardenRotatingMotor(int pin, int fullSpeed);
  
  // Get and set the current velocity
  int getSpeed() const { return vel_; }
  void setSpeed(int v);
  
  // Get and set the current full speed
  int getFullSpeed() const { return fullSpeed_; }
  void setFullSpeed(int fs);

#if TABLE_ACCESS == 2
  static void setSpeedTable(double amplitude, double sigma);
#endif
};

// ***************************************
// *                                     *
// *  GizmoGardenPositioningMotor Class  *
// *                                     *
// ***************************************

// The GizmoGardenPositioningMotor class is used for standard servos.
// Unlike GizmoGardenServo it uses an angle range of -90 to +90, and
// allows the 0 degree position to be adjusted to calibrate your
// mechanical setup. It also allows positions to be flipped around 0
// to make mirror-image motions.

class GizmoGardenPositioningMotor : private GizmoGardenServo
{
  bool flipDirection;
  int8_t offset;
  int8_t position;

public:
  GizmoGardenPositioningMotor(int pin, bool flipDirection = false, int offset = 0);

  // Offset is in degrees and will be constrained to the range [-90 .. 90]
  int getOffset() const { return offset; }
  void setOffset(int x);

  // Position is in degrees and will be constrained to the range [-128 .. 127],
  // with the actual motor position constrained to [-90 .. 90]. The reason for
  // the extra range of position is because the offset might not be 0.
  int getPosition() const { return position; }
  void setPosition(int);
};

// ************
// *          *
// *  Dancer  *
// *          *
// ************

// Dancer is a GizmoGardenTask that uses two GizmoGardenRotatingMotors
// (presumably attached to wheels) to execute choreographed dance moves.
// A dance is an array of GizmoGardenDanceMove, each specifying a speed
// for the left and right motor. The duration of each move is specified
// using GizmoGardenText strings, formatted for getMusicTime and
// documented in GizmoGardenCommon.h.
//
// There should be one more GizmoGardenDanceMove in the dance than
// there are notes in the durations string. The final move is executed
// to conclude the dance, which can be RestMotion to stop or anything
// else to keep moving.

struct GizmoGardenDanceMove
{
  int8_t leftSpeed;
  int8_t rightSpeed;
};

// Macros for convenient initialization of a dance. For the turn moves,
// r is a radius parameter. 0 radius is a turn in place, with the
// wheels spinning in opposite directions.
#define RestMotion { 0, 0 }
#define MoveStraight(v) { v, v }
#define TurnRight(v, r) { v, v - 8 * v / (r + 4) }
#define TurnLeft(v, r) { v - 8 * v / (r + 4), v }
#define MoveCustom(left, right) { left, right }

class GizmoGardenDancer : public GizmoGardenTask
{
public:
  GizmoGardenDancer(GizmoGardenRotatingMotor& leftWheel,
                    GizmoGardenRotatingMotor& rightWheel,
                    uint16_t baseTime);

  void dance(GizmoGardenDanceMove*, GizmoGardenText durations);

private:
  GizmoGardenRotatingMotor& leftWheel;
  GizmoGardenRotatingMotor& rightWheel;

  uint16_t baseTime;

  GizmoGardenDanceMove* dm;
  GizmoGardenText durations;

  virtual void myTurn();

  DECLARE_TASK_NAME
};

// **************
// *            *
// *  Gestures  *
// *            *
// **************

// GizmoGardenGestures is a GizmoGardenTask that uses a
// GizmoGardenPositioningMotor to execute choreographed dance moves
// (e.g. for arms).
//
// A dance is an array of GizmoGardenGestureAngle, each specifying a
// position in degrees. The duration of each move is specified
// using GizmoGardenText strings, formatted for getMusicTime and
// documented in GizmoGardenCommon.h. If an angle is given the
// value GestureKeep, the previous position is kept for the current
// note time.

enum GizmoGardenDanceCodes
{
  GestureKeep = -128,
};

typedef int8_t GizmoGardenGestureAngle;

class GizmoGardenGestures : public GizmoGardenTask
{
public:
  GizmoGardenGestures(GizmoGardenPositioningMotor& motor, uint16_t baseTime);

  void play(const GizmoGardenGestureAngle* angles, GizmoGardenText times);

private:
  GizmoGardenPositioningMotor& motor;

  uint16_t baseTime;

  const GizmoGardenGestureAngle* angles;
  GizmoGardenText times;

  virtual void myTurn();

  DECLARE_TASK_NAME
};

#endif
