/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "GizmoGardenMultitasking.h"

#ifdef TASK_MONITOR
#include "../GizmoGarden_Menus/GizmoGardenMenus.h"

class TaskMonitor : public GizmoGardenMenuItem
{
  friend class GizmoGardenTask;
  static Ring<GizmoGardenTask> taskRing;

protected:
  virtual void action(uint8_t event, int8_t direction, GizmoGardenLCDPrint&);
}
taskMonitor;

Ring<GizmoGardenTask> TaskMonitor::taskRing;

void TaskMonitor::action(uint8_t event, int8_t direction, GizmoGardenLCDPrint& lcd)
{
  switch (event)
  {
    case Enter:
      lcd.print(F("TaskMonitor"));
      break;

    case ChangeAndShow:
      if (direction > 0)
        taskRing.forward();
      else if (direction < 0)
        taskRing.backup();
      lcd.setCursor(0, 1);
      ggPrint(lcd, taskRing.current()->name(), 10);
      break;

    case Monitor:
      lcd.setCursor(15, 0);
      lcd.print(taskRing.current()->isRunning() ? '!' : ' ');
      lcd.setCursor(10, 1);
      ggPrint(lcd, taskRing.current()->getRunTime() * 0.016f, 6, 2);    
      break;

    case Select:
      if (taskRing.current()->menuStartStopAllowed)
        if (taskRing.current()->isRunning())
          taskRing.current()->stop();
        else
          taskRing.current()->start();
      break;
  }
}
#endif

GizmoGardenTask* GizmoGardenTask::runList = 0;

#ifdef TASK_MONITOR
GizmoGardenTask::GizmoGardenTask(bool allowMenuStartStop)
  : running(false), runTime(0), runTimeReset(0),
    RingBase(TaskMonitor::taskRing.ring), menuStartStopAllowed(allowMenuStartStop)
#else
GizmoGardenTask::GizmoGardenTask(bool)
  : running(false), runTime(0), runTimeReset(0)
#endif
{
}

GizmoGardenTask::~GizmoGardenTask()
{
  stop();
#ifdef TASK_MONITOR
  remove(TaskMonitor::taskRing.ring);
#endif
}

void GizmoGardenTask::scheduleMe(uint32_t ms)
{
  GizmoGardenTask* p;
  GizmoGardenTask* q = 0;
  for (p = runList; p != 0 && p->myTime <= ms; q = p, p = p->next);
  next = p;
  if (q == 0)
    runList = this;
  else
    q->next = this;

  myTime = ms;
}

void GizmoGardenTask::unscheduleMe()
{
  GizmoGardenTask* p;
  GizmoGardenTask* q = 0;
  for (p = runList; p != 0; q = p, p = p->next)
    if (p == this)
    {
      if (q == 0)
        runList = next;
      else
        q->next = next;
      break;
    }
}

void GizmoGardenTask::fitMeIn(uint16_t ms)
{
  GizmoGardenTask* p;
  GizmoGardenTask* q = 0;
  uint32_t t = millis();
  for (p = runList; p != 0 && p->myTime < t + ms; q = p, t = p->myTime, p = p->next);
  next = p;
  if (q == 0)
    runList = this;
  else
    q->next = this;

  myTime = t;
  running = true;
}

void GizmoGardenTask::wait() const
{
  while (isRunning())
    run();
}

//void GizmoGardenTask::delay(uint16_t ms)
//{
//  uint32_t t = millis();
//  while (millis() < t + ms)
//    run();
//}

void GizmoGardenTask::callMe(uint16_t ms)
{
  scheduleMe(myTime + ms);
  running = true;
}

void GizmoGardenTask::callMeFromNow(uint16_t ms)
{
  scheduleMe(millis() + ms);
  running = true;
}

void GizmoGardenTask::start(uint16_t ms)
{
  stop();
  callMeFromNow(ms);
  customStart();
}

void GizmoGardenTask::stop()
{
  if (isRunning())
  {
    customStop();
    unscheduleMe();
    running = false;
  }
}

void GizmoGardenTask::customStart() {}
void GizmoGardenTask::customStop() {}

#ifdef TASK_MONITOR
GizmoGardenText GizmoGardenTask::name() const
{
  return F("Untitled");
}
#endif

void GizmoGardenTask::run()
{
  while (true)
  {
    uint32_t ms = millis();
    GizmoGardenTask* p = runList;
    if (p != 0 && p->myTime <= ms)
    {
      runList = p->next;
      p->running = false;
      p->myTime = ms;
      uint32_t us = micros();
      p->myTurn();
      uint16_t t = (uint16_t)(micros() - us >> 4);
      if (++p->runTimeReset == 24)
      {
        p->runTimeReset = 0;
        p->runTime = t;
      }
      else
        p->runTime = max(p->runTime, t);

      if (!p->isRunning())
        p->customStop();
    }
    else
      break;
  }
}

extern "C"
{
  void yield() { GizmoGardenTask::run(); }
}
