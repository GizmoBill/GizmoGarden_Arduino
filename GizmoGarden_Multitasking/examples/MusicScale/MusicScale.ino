
// ***************************************
// *                                     *
// *  Gizmo Garden Multitasking Example  *
// *                                     *
// ***************************************
//
// Also a good example of the GizmoGardenTone and GizmoGardenIndicators libraries.

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
examples below.

Example Code
------------
This example plays a musical scale (the white notes of octive 7) on a piezo buzzer.
It uses two switches and one LED, connected as indicated below. One switch changes
the volume of the music (0 is off, 1 is half, 2 is full), while the LED flashes to
indicate the volume to the user. The other switch starts or stops the music.

To indicate volume n, the LED is flashed n times, followed by a longer pause. The
cycle of n flashes followed by a puase repeats until a new volume is selected.

Three tasks are created and managed by the multitasking system. One task checks the
switches 20 times a second to see if any have changed from not clicked to clicked, and
if so to cycle the volume or start/stop the music. Checking 20 times a second
eliminates false clicks due to contact bounce.

One task plays the music. It starts when told to do so by the switch-checking task,
and stops when either all 8 notes have been played, or the switch-checking task
stops it early.

One task controls the flashing LED. It runs at times dependent on the value that is
being indicated.
*/

#include <GizmoGardenCommon.h>
#include <GizmoGardenMultitasking.h>
#include <GizmoGardenIndicators.h>
#include <GizmoGardenTone.h>

// Define the digital pins we'll use for the switches, LEDs, and buzzer.
enum DigitalPins
{
  TonePin1        =  4,   // One terminal of the piezo buzzer
  TonePin2        =  5,   // Other terminal
  VolumeLedPin    = 12,   // LED wired to ground through a resistor to display volume
  VolumeSwitchPin = 11,   // Switch wired to ground to control volume
  StopGoSwitchPin = 10,   // Switch wired to ground to start or stop the music
};

// Create an indicator task to show the tone volume (0, 1, or 2) by flashing an
// LED that is wired to VolumeLedPin. The pin is set to OUTPUT, so you don't have to do
// it in setup().
GizmoGardenIndicator volumeIndicator(VolumeLedPin);

// ********************************
// *                              *
// *  Task to Play Musical Scale  *
// *                              *
// ********************************

int pitches[] = { 2093, 2349, 2637, 2794, 3136, 3520, 3951, 4186 };
int currentPitch;   // will be initialized when the task starts

// Make a task with custom start and stop
MakeGizmoGardenTaskWithStartStop(PlayScale)
{
  // Play each note in sequence and then stop.
  if (currentPitch < 8)
  {
    GizmoGardenTone(pitches[currentPitch], 600);
    ++currentPitch;

    // Call me back in 750 milliseconds to play the next note. If there is no next note,
    // callMe is skipped and so the task stops running. We might consider stopping the
    // task after starting the last note, but when the task stops for any reason the
    // CustomStop function is called, which would kill the last note immediatly. So we
    // let it go one more time.
    callMe(750);
  }
}

// When the task starts, initialize the counter.
CustomStart(PlayScale)
{
  currentPitch = 0;
}

// When the task stops, shut off the music if any is playing. The music needs to be
// shut off when the start/stop switch is clicked to stop the music.
CustomStop(PlayScale)
{
  GizmoGardenTone(0);
}

// ***************************
// *                         *
// *  Task to Check Switchs  *
// *                         *
// ***************************

int previousVolumeSwitch = HIGH;
int previousStopGoSwitch = HIGH;

MakeGizmoGardenTask(CheckSwitches)
{
  // Read the current state of the volume switch
  int sw = digitalRead(VolumeSwitchPin);

  // If it has changed from not clicked (HIGH) to clicked (LOW), advance the
  // volume
  if (sw == LOW && previousVolumeSwitch == HIGH)
  {
    GizmoGardenSetVolume((GizmoGardenGetVolume() + 1) % 3);
    volumeIndicator.setValue(GizmoGardenGetVolume());
  }
  previousVolumeSwitch = sw;

  // Read the current state of the stop/go switch
  sw = digitalRead(StopGoSwitchPin);

  // If it has changed from not clicked (HIGH) to clicked (LOW), start or stop the
  // music playing task
  if (sw == LOW && previousStopGoSwitch == HIGH)
  {
    if (PlayScale.isRunning())
      PlayScale.stop();
    else
      PlayScale.start();
  }
  previousStopGoSwitch = sw;

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
  GizmoGardenToneBegin(TonePin1, TonePin2);
  
  pinMode(VolumeSwitchPin, INPUT_PULLUP);
  pinMode(StopGoSwitchPin, INPUT_PULLUP);

  // Start the task to check the switches. The other tasks start as needed.
  CheckSwitches.start();
  volumeIndicator.setValue(GizmoGardenGetVolume());
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
