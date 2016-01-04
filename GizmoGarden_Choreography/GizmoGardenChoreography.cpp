/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "GizmoGardenChoreography.h"

GizmoGardenCycle::GizmoGardenCycle(int stepsInCycle, uint16_t periodMs)
  : stepsInCycle(step), periodMs(periodMs), step(0)
{
}

void GizmoGardenCycle::myTurn()
{
  nextStep();
  step = (step + 1) % stepsInCycle;
  callMe(periodMs);
}


GizmoGardenWave::GizmoGardenWave(int lowStep, int highStep, uint16_t periodMs)
  : lowStep(lowStep), highStep(highStep), periodMs(periodMs), step(lowStep), direction(-1)
{
}

void GizmoGardenWave::myTurn()
{
  nextStep();
  if (step == lowStep || step == highStep)
    direction = -direction;
  step += direction;
  callMe(periodMs);
}
