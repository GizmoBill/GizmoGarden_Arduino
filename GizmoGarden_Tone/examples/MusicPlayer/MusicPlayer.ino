// ***************************************
// *                                     *
// *  Gizmo Garden Music Player Example  *
// *                                     *
// ***************************************
//
// Also a good example of the GizmoGardenMultitasking and GizmoGardenIndicators libraries.

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
This example plays the song "Row Row Row Your Boat" on a piezo buzzer.
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
and stops when either the song ends, or the switch-checking task stops it early.

One task controls the flashing LED. It runs at times dependent on the value that is
being indicated.

*/
#include <GizmoGardenCommon.h>
#include <GizmoGardenMultitasking.h>
#include <GizmoGardenIndicators.h>
#include <GizmoGardenTone.h>
#include <GizmoGardenMusicPlayer.h>

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

// ******************
// *                *
// *  Music Player  *
// *                *
// ******************

// GizmoGardenMusicPlayer is a task that plays music using GizmoGardenTone. Notes are
// specified as two GizmoGardenText strings, one for pitch and one for duration.
// GizmoGardenText puts the strings in flash to save SRAM memory, and the "F" macro
// can also be used. GizmoGardenText can create a string by static initialization,
// while "F" can only be used in an expression.
//
// The pitch string is a sequence of <note><octave><sharp/flat> characters, where <note>
// is A-G (upper case), <octave> is 5-7, and <sharp/flat> is an optional # or b. Following
// conventional music notation, each octive is CDEFGAB, so that for example C6 follows B5.
// Spaces are ignored, except that they cannot separate the characters of one note.
// Octaves lower than 5 are not implemented because the pitches are too low for piezo buzzers.
//
// A - (dash) can be used is place of <note><octave> characters to signify a rest.
//
// The duration string uses these characters:
//    S   sixteenth
//    E   eighth
//    Q   quarter
//    H   half
//    W   whole
//    T   triplet (beatlength / 3)
//
// All notes except T can be followed by a "." to extend the time by half. Spaces are
// ignored, except that they cannot appear between a note and the ".". The length in
// milliseconds of a quarter note is called the beatLength, as is given as an argument
// to the constructor.
//
//
// One can derive a class from GizmoGardenMusicPlayer that makes use of currentNote and
// currentWhiteNote to do some related action, synchronized to the music.
GizmoGardenMusicPlayer player(400);   // quarter note is 400 ms

MakeGizmoGardenText(rowPitches, "C6C6C6D6E6 E6D6E6F6G6 C7C7C7G6G6G6E6E6E6C6C6C6 G6F6E6D6C6");
MakeGizmoGardenText(rowTimes  , "Q Q E.S Q  E.S E.S H  T T T T T T T T T T T T  E.S E.S H");


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
    if (player.isRunning())
      player.stop();
    else
      player.play(rowPitches, rowTimes);
  }
  previousStopGoSwitch = sw;

  // Call me back in 50 milliseconds to check the switchs again.
  callMe(50);
}

void play()
{
  player.play(rowPitches, rowTimes);
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
