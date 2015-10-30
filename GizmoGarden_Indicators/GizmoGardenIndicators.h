#ifndef _GizmoGardenIndicators_
#define _GizmoGardenIndicators_

/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "../GizmoGarden_Common/GizmoGardenCommon.h"
#include "../GizmoGarden_Multitasking/GizmoGardenMultitasking.h"

// ********************
// *                  *
// *  LED Indicators  *
// *                  *
// ********************
/*
GizmoGardenIndicator is a task that conttrols a digital output pin,
presumably wired to an LED, so as to indicate to a human observer an
integer value in the range -10 to +10. The value n is indicated by
continuously repearing n short flashes and a longer pause. If n is
positive, the flashes are HIGH and the pause is LOW. If n is negative,
the flashes are LOW and the pause is HIGH. If n is 0, the pin is
held LOW. If n is -128, the pin is held HIGH.

Since this is a task, it runs cooperatively with other tasks.
*/

class GizmoGardenIndicator : public GizmoGardenTask
{
public:
  // Construct and set pin mode to OUTPUT
  GizmoGardenIndicator(int pin);

  // Set value to be indicated. Will be constrained to [-10 .. 10] or
  // -128. The indicated value changes from the current value to the
  // new value without indicating any incorrect intermediate values. 
  void setValue(int value);

  // Get the value being indicated.
  int getValue() { return value; }

  // 
  void setOnOff(bool on);

private:
  enum Times
  {
    OnTime        = 199,
    ShortOffTime  = 397,
    LongOffTime   = 797
  };

  uint8_t pin;
  int8_t value;
  int8_t nextValue;
  volatile uint8_t phase;

  virtual void myTurn();

  DECLARE_TASK_NAME
};

#endif
