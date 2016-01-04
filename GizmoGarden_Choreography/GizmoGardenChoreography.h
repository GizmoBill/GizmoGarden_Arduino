#ifndef _GizmoGardenChoreography_
#define _GizmoGardenChoreography_

/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

// *******************************
// *                             *
// *  Gizmo Garden Choreography  *
// *                             *
// *******************************

#include <Arduino.h>
#include "../GizmoGarden_Multitasking/GizmoGardenMultitasking.h"

class GizmoGardenCycle : public GizmoGardenTask
{

protected:
  int step;
  int stepsInCycle;
  uint16_t periodMs;

  GizmoGardenCycle(int stepsInCycle, uint16_t periodMs);

  virtual void myTurn();

  virtual void nextStep() = 0;
};

#define MAKE_CYCLE(taskId, stepsInCycle, periodMs, custom)        \
class Class##taskId : public GizmoGardenCycle                     \
{                                                                 \
protected:                                                        \
  virtual void nextStep();                                        \
public:                                                           \
  Class##taskId() : GizmoGardenCycle(stepsInCycle, periodMs) {}   \
  DECLARE_TASK_NAME                                               \
  custom                                                          \
} taskId;                                                         \
DEFINE_TASK_NAME(taskId)                                          \
void Class##taskId::nextStep()

#define MakeGizmoGardenCycle(             taskId, stepsInCycle, periodMs) MAKE_CYCLE(taskId, stepsInCycle, periodMs,                         )
#define MakeGizmoGardenCycleWithStart(    taskId, stepsInCycle, periodMs) MAKE_CYCLE(taskId, stepsInCycle, periodMs, CUSTOM_START            )
#define MakeGizmoGardenCycleWithStop(     taskId, stepsInCycle, periodMs) MAKE_CYCLE(taskId, stepsInCycle, periodMs, CUSTOM_STOP             )
#define MakeGizmoGardenCycleWithStartStop(taskId, stepsInCycle, periodMs) MAKE_CYCLE(taskId, stepsInCycle, periodMs, CUSTOM_START CUSTOM_STOP)


class GizmoGardenWave : public GizmoGardenTask
{
protected:
  int step;
  int lowStep, highStep;
  int8_t direction;
  uint16_t periodMs;

  GizmoGardenWave(int lowStep, int HighStep, uint16_t periodMs);

  virtual void myTurn();

  virtual void nextStep() = 0;
};


#define MAKE_WAVE(taskId, lowStep, highStep, periodMs, custom)        \
class Class##taskId : public GizmoGardenWave                          \
{                                                                     \
protected:                                                            \
  virtual void nextStep();                                            \
public:                                                               \
  Class##taskId() : GizmoGardenWave(lowStep, highStep, periodMs) {}   \
  DECLARE_TASK_NAME                                                   \
  custom                                                              \
} taskId;                                                             \
DEFINE_TASK_NAME(taskId)                                              \
void Class##taskId::nextStep()

#define MakeGizmoGardenWave(             taskId, lowStep, highStep, periodMs) MAKE_WAVE(taskId, lowStep, highStep, periodMs,                         )
#define MakeGizmoGardenWaveWithStart(    taskId, lowStep, highStep, periodMs) MAKE_WAVE(taskId, lowStep, highStep, periodMs, CUSTOM_START            )
#define MakeGizmoGardenWaveWithStop(     taskId, lowStep, highStep, periodMs) MAKE_WAVE(taskId, lowStep, highStep, periodMs, CUSTOM_STOP             )
#define MakeGizmoGardenWaveWithStartStop(taskId, lowStep, highStep, periodMs) MAKE_WAVE(taskId, lowStep, highStep, periodMs, CUSTOM_START CUSTOM_STOP)

#endif
