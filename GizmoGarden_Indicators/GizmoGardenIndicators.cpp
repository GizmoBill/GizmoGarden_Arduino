/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "GizmoGardenIndicators.h"

GizmoGardenIndicator::GizmoGardenIndicator(int pin)
: pin((uint8_t)pin)
{
  pinMode(pin, OUTPUT);
}

void GizmoGardenIndicator::setValue(int n)
{
  if ((n & 0x7F) == 0)
  {
    stop();
    value = (int8_t)n;
    digitalWrite(pin, n >= 0 ? LOW : HIGH);
  }
  else
  {
    nextValue = (int8_t)constrain(n, -10, 10);
    if (isRunning())
    {
      if (phase < 2 * ggAbs(nextValue))
        value = nextValue;
    }
    else
    {
      value = nextValue;
      phase = 0;
      start();
    }
  }
}

void GizmoGardenIndicator::setOnOff(bool on)
{
  setValue(on ? 0x80 : 0);
}

#ifdef TASK_MONITOR
GizmoGardenText GizmoGardenIndicator::name() const
{
  return F("Indicator"); 
}
#endif

void GizmoGardenIndicator::myTurn()
{
  bool on;
  int p = phase;
  if (p == 2 * ggAbs(value) - 1)
  {
    on = false;
    phase = 0;
    value = nextValue;
    callMe(LongOffTime);
  }
  else
  {
    on = (p & 1) == 0;
    phase = p + 1;
    callMe(on ? OnTime : ShortOffTime);
  }

  digitalWrite(pin, on ^ (value < 0) ? HIGH : LOW);
}
