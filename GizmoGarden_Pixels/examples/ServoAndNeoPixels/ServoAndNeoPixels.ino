// ********************************************
// *                                          *
// *  Gizmo Garden Servo + NeoPixels Example  *
// *                                          *
// ********************************************
//
// Also a good example of Gizmo Garden multitasking.

/*
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
The code below controls a NeoPixel ring and a servo simultaneously and without any
interference. The servo controller is software interrupt driven and can handle up to
a dozen servos on any pins, and several hundred NeoPixels. For more info and a
theory of operation, see GizmoGardenServo.h.

The example creates three tasks running in the Gizmo Garden multitasking system. One
task cycles the NeoPixels, one waves the servo back and forth, and the third monitors
two switches that start and stop the first two tasks.
*/

#include <Adafruit_NeoPixel_GizmoGardenModified.h>

#include <GizmoGardenCommon.h>
#include <GizmoGardenMultitasking.h>
#include <GizmoGardenPixels.h>
#include <GizmoGardenServo.h>

// Define the digital pins we'll use.
enum DigitalPins
{
  NeoPin          =  6,
  ServoPin        =  8,
  NeoSwitchPin    = 10,   // Switch wired to ground to start/stop NeoPixels
  ServoSwitchPin  = 11,   // Switch wired to ground to start/stop servo
};

// ********************
// *                  *
// *  NeoPixels Task  *
// *                  *
// ********************

Adafruit_NeoPixel neoPixels(30, NeoPin);

int pixelIndex = 0;

MakeGizmoGardenTask(neoTask)
{
  neoPixels.clear();
  neoPixels.setPixelColor(pixelIndex, (pixelIndex & 1) << 6, (pixelIndex & 2) << 5, (pixelIndex & 4) << 4);
  neoPixels.show();
  pixelIndex = (pixelIndex + 1) % neoPixels.numPixels();
  callMe(40);
}

// ****************
// *              *
// *  Servo Task  *
// *              *
// ****************

GizmoGardenServo servo(ServoPin);

int servoAngle = 45;
int step = 1;

MakeGizmoGardenTask(servoTask)
{
  servo.write(servoAngle);
  servoAngle += step;
  if (servoAngle >= 135 || servoAngle <= 45)
    step = -step;
  callMe(25);
}


// ***************************
// *                         *
// *  Task to Check Switchs  *
// *                         *
// ***************************

int previousNeoSwitch = HIGH;
int previousServoSwitch = HIGH;

MakeGizmoGardenTask(CheckSwitches)
{
  // Read the current state of the neo switch
  int sw = digitalRead(NeoSwitchPin);

  // If it has changed from not clicked (HIGH) to clicked (LOW), start or
  // stop the neoPixels task
  if (sw == LOW && previousNeoSwitch == HIGH)
  {
    if (neoTask.isRunning())
      neoTask.stop();
    else
      neoTask.start();
  }
  previousNeoSwitch = sw;

  // Read the current state of the servo switch
  sw = digitalRead(ServoSwitchPin);

  // If it has changed from not clicked (HIGH) to clicked (LOW), start or stop the
  // servo task
  if (sw == LOW && previousServoSwitch == HIGH)
  {
    if (servoTask.isRunning())
      servoTask.stop();
    else
      servoTask.start();
  }
  previousServoSwitch = sw;

  // Call me back in 50 milliseconds to check the switchs again.
  callMe(50);
}


// ***********
// *         *
// *  Setup  *
// *         *
// ***********

void setup()
{
  GizmoGardenTask::begin();
  GizmoGardenServo::begin();
  neoPixels.begin();
  
  pinMode(NeoSwitchPin, INPUT_PULLUP);
  pinMode(ServoSwitchPin, INPUT_PULLUP);

  // Start the task to check the switches. The other tasks start as needed.
  CheckSwitches.start();
}

// **********
// *        *
// *  Loop  *
// *        *
// **********

void loop()
{
  GizmoGardenTask::run();
}
