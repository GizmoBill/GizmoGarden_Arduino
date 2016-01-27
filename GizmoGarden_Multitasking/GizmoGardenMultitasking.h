#ifndef _GizmoGardenMultitasking_
#define _GizmoGardenMultitasking_

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
// *  Gizmo Garden Multitasking  *
// *                             *
// *******************************

#include <Arduino.h>
#include "../GizmoGarden_Common/GizmoGardenCommon.h"

// Define this macro to include the task monitor menu item. Comment
// out if not using menus or to save memory.
//#define TASK_MONITOR

#ifdef TASK_MONITOR
#define DECLARE_TASK_NAME virtual GizmoGardenText name() const;
#define DEFINE_TASK_NAME(taskId) GizmoGardenText Class##taskId::name() const { return F(#taskId); }

class GizmoGardenTask : public RingBase
{
  friend class TaskMonitor;

  // Set on construction to allow or prevent the task monitor menu item
  // from starting or stopping the task. 
  const bool menuStartStopAllowed;

#else
#define DECLARE_TASK_NAME
#define DEFINE_TASK_NAME(name)

class GizmoGardenTask
{
#endif

  // Link pointer for list of GizmoTask objects, sorted in order of increasing myTime
  // If running, this task will be on the runList.
  GizmoGardenTask* next;     

  volatile bool running;    // Am I currently running?
  uint32_t myTime;          // If so, when am I scheduled to run next (absolute time)?

  // Measured execution time of myTurn in microseconds*16. Max is about 1 second.
  // Value is a crude recent maximum, reset every 24 calls to myTurn
  uint16_t runTime;
  int8_t runTimeReset;

  static GizmoGardenTask* runList;

  // Add this task to the runList in proper order. Argument is
  // absolute time in milliseconds.
  void scheduleMe(uint32_t ms);

  // Remove this task from the runList; do nothing if not on the list
  void unscheduleMe();

protected:
  // Only derived classes can make a GizmoTask
#ifdef TASK_MONITOR
  GizmoGardenTask(bool allowMenuStartStop = true);
#else
  // Argument ignored, for compatibility with TASK_MONITOR version
  GizmoGardenTask(bool = true);
#endif

  // Typically a class with virtual functions declares a virtual
  // destructor, but we're not going to use operator delete on
  // a task and memory for virtual function table entries is tight.
  ~GizmoGardenTask();    // Can be running when destroyed.

  // Override this and you will be called when it's your turn. Be
  // considerate of CPU time. When called, you are no longer
  // running (isRunning() returns false). To keep running you
  // must call callMe, callMeFromNow, or fitMeIn.
  virtual void myTurn() = 0;

  // Schedule me for a call back in the specified number of milliseconds
  // after the start of current callback. Use only in myTurn().
  void callMe(uint16_t ms);

  // Schedule me for a call back in the specified number of milliseconds
  // after now. Use only in myTurn().
  void callMeFromNow(uint16_t ms);

  // Schedule me for a call back at a time when the there is nothing to
  // do for at least the specified number of milliseconds.
  void fitMeIn(uint16_t ms);

public:
  // Call this in setup() to initialize GizmoGarden multitasking. Currently
  // this does nothing; hook for evolution.
  static void begin() {}

  // Call run() in loop(), preferably at 1KHz or more.
  static void run();

  // Call this after setting up your state variables to get on the GizmoTask
  // schedule, at the specified time after the present.
  void start(uint16_t ms = 0);

  // See if the task is still going
  bool isRunning() const { return running; }

  // Stop running
  void stop();

  // Override these to do some extra stuff when the task is started
  // or stopped.
  virtual void customStart();
  virtual void customStop();

  // Wait for task to complete
  // Note: This makes a recursive call to run. Only one task in the
  // program should use wait or delay, and only when normal
  // miltitasking is too awkward.
  void wait() const;

  // delay the specified number of milliseconds.
  // Note: This makes a recursive call to run. Only one task in the
  // program should use wait or delay, and only when normal
  // miltitasking is too awkward.. 
  // static void delay(uint16_t ms);

  // Return the maximum recent execution time of myTurn in
  // microseconds
  int32_t getRunTime() const { return runTime; }

  DECLARE_TASK_NAME
};

// Create a task by specifying just the body of myTurn,
// so that 
#define CUSTOM_START virtual void customStart();
#define CUSTOM_STOP  virtual void customStop();

#define MAKE_TASK(taskId, custom)                     \
class Class##taskId : public GizmoGardenTask          \
{                                                     \
protected:                                            \
  virtual void myTurn();                              \
public:                                               \
  DECLARE_TASK_NAME                                   \
  custom                                              \
} taskId;                                             \
DEFINE_TASK_NAME(taskId)                              \
void Class##taskId::myTurn()

#define MakeGizmoGardenTask(taskId)              MAKE_TASK(taskId,                         )
#define MakeGizmoGardenTaskWithStart(taskId)     MAKE_TASK(taskId, CUSTOM_START            )
#define MakeGizmoGardenTaskWithStop(taskId)      MAKE_TASK(taskId, CUSTOM_STOP             )
#define MakeGizmoGardenTaskWithStartStop(taskId) MAKE_TASK(taskId, CUSTOM_START CUSTOM_STOP)

#define CustomStart(taskId) void Class##taskId::customStart()
#define CustomStop(taskId)  void Class##taskId::customStop ()

#endif
