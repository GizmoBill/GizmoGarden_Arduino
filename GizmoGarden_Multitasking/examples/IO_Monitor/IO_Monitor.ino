// ***************************************
// *                                     *
// *  Gizmo Garden Multitasking Example  *
// *                                     *
// ***************************************

/*
Also a good example of the GizmoGardenIndicators library.

Task Overview
-------------
A task is created like this:

MakeGizmoGardenTask(TaskName)
{
  <task body>
}

TaskName is a C++ identifier, so it must be letters and numbers with no spaces or
punctuation, and must start with a letter. Tasks share the computer by taking turns,
and the <task body> is code you write to say what happens when it's your turn (i.e.
this task's turn). The code should run quickly, because no other task can have a turn
while your <task body> is running. No more than a few milliseconds is a good rule of
thumb, depending on what else is going on in your program. If you need to run longer,
you can break up the work into short units. You can see how long your task's turns are
taking with TaskName.getRunTime(), which returns the recent maximum tine in microseconds.

Every task is either running or stopped. You can start and stop tasks anywhere in your
code with TaskName.start() and TaskName.stop(). You can see if a task is running
with TaskName.isRunning(). You can start a task some number of milliseconds in the
future with TaskName.start(ms). If you start a task that is already running, it stops
first and then is restarted.

Every task that is running is scheduled to have a turn at some specific time. When that
time comes, or as close to that time as other tasks allow, the <task body> executes.
Somewhere in <task body>, you can do callMe(ms) to schedule another turn in the
specified number of milliseconds after the start of the current turn. If you do not do
the callMe, the task becomes stopped.

You can write code to say what happens when a task is started or stopped. There are
examples in MusicScale.ino.

Example Code
------------
This example allows you to monitor the state of digital pins 1 - 10 using two LEDs
and a switch. One of the LEDs tells you which pin you are monitoring, and the other
shows the state of that pin. The switch allows you to cycle through the pins to select
one to monitor. The monitored pin can be an input or an output.

The pin number being monitored is indicated by flashing one of the LEDs. For pin n,
the LED is flashed n times, followed by a longer pause. The cycle of n flashes
followed by a puase repeats until a new pin is selected.

Clicking the switch advances to the next pin. After pin 10, it cycles back to pin 1.

Three tasks are created and managed by the multitasking system. One task checks the
switch 20 times a second to see if it has changed from not clicked to clicked, and
if so to advance the selected pin. Checking 20 times a second eliminates false
clicks due to contact bounce.

One task reads the selected pin and writes it to an LED to show the state of the pin.
This task runs 200 times a second.

One task controls the flashing LED. It runs at times dependent on the value that is
being indicated.
*/

#include <GizmoGardenCommon.h>
#include <GizmoGardenMultitasking.h>
#include <GizmoGardenIndicators.h>

// Define the digital pins we'll use for the switch and LEDs
enum DigitalPins
{
  SelectSwitvhPin = 11,   // Put a switch to ground on this pin
  SelectLedPin    = 12,   // Put an LED through a resistor to ground
  MonitorLedPin   = 13,   // Put an LED through a resistor to ground
};

// Create an indicator task to show which pin we've selected to monitor by flashing an
// LED that is wired to SelectLedPin. The pin is set to OUTPUT, so you don't have to do
// it in setup().
GizmoGardenIndicator selectIndicator(SelectLedPin);

// *********************************
// *                               *
// *  Task to Check Select Switch  *
// *                               *
// *********************************

int previousSelectSwitch = HIGH;
int selectedPin = 1;              // this is the currently selected pin number

MakeGizmoGardenTask(SelectPin)
{
  // Read the current state of the switch
  int selectSwitch = digitalRead(SelectSwitvhPin);

  // If it has changed from not clicked (HIGH) to clicked (LOW), advance the
  // selected pin
  if (selectSwitch == LOW && previousSelectSwitch == HIGH)
  {
    if (selectedPin < 10)
      ++selectedPin;
    else
      selectedPin = 1;

    // Show the new pin number to the user
    selectIndicator.setValue(selectedPin);
  }
  previousSelectSwitch = selectSwitch;

  // Call me back in 50 milliseconds to check the switch again.
  callMe(50);
}

// **********************************
// *                                *
// *  Task to Monitor Selected Pin  *
// *                                *
// **********************************

MakeGizmoGardenTask(MonitorPin)
{
  // Read the selected pin
  int pin = digitalRead(selectedPin);

  // Show it
  digitalWrite(MonitorLedPin, pin);

  // Call me back in 5 milliseconds
  callMe(5);
}

// ***********
// *         *
// *  Setup  *
// *         *
// ***********

void setup()
{
  GizmoGardenTask::begin();
  
  pinMode(SelectSwitvhPin, INPUT_PULLUP);
  pinMode(MonitorLedPin, OUTPUT);

  // For demonstration purposes, set pins 1 - 9 to INPUT_PULLUP and pin 10 to
  // OUTPUT. In loop, pin 10 is set HIGH if any of pins 1 - 9 are LOW. Any of 
  // these pins can then be monitored.
  for (int i = 1; i <= 9; ++i)
    pinMode(i, INPUT_PULLUP);
  pinMode(10, OUTPUT);

  // Start the two tasks created above. The indicator task starts automatically as needed.
  SelectPin.start();
  MonitorPin.start();
  selectIndicator.setValue(selectedPin);
}

// **********
// *        *
// *  Loop  *
// *        *
// **********
//
// You must call GizmoGardenTask::run() somewhere in loop(), and the rest of it, if any,
// should not run too long, just like any task (e.g. under a few milliseconds, shorter is
// better). Do not call delay(), because no task can run during a delay. But you can call
// GizmoGardenTask::delay(), which has the same effect and allows other tasks to run.
void loop()
{
  GizmoGardenTask::run();

  for (int i = 1; i <= 9; ++i)
    if (digitalRead(i) == LOW)
    {
      digitalWrite(10, HIGH);
      return;
    }
  digitalWrite(10, LOW);
}
