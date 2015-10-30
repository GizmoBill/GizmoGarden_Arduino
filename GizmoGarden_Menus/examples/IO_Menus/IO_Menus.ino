// **************************************
// *                                    *
// *  Gizmo Garden Menu System Example  *
// *                                    *
// **************************************

/*

How the Menu System Works
-------------------------
The Gizmo Garden menu system uses an Adafruit LCD shield (16x2 or 16x4) to allow a user
to monitor changing values, examine and set parameters, and take actions. It runs with
the Gizmo Garden multitasking system so that other tasks can be active while the menus
are being used.

A menu is created in a sketch by the programmer, and consists of one or more menu items.
Each item has a name that usually appears on the top line of the LCD when that item is
selected. I say "usually" because you can program a menu item to behave differently if
you want. The name is also an identifier that can be used in code to select that item.
Creating menu items is very simple and described below.

The user interacts with the menus by clicking the five buttoms on the LCD shield. These
buttons are called LEFT, RIGHT, UP, DOWN, and SELECT. The LEFT and RIGHT buttons move
forward and backward among the menu items, which form a ring. There really is no first
and last menu item, you just cycle around in either direction.

The UP and DOWN buttons are used to change a parameter setting and show the result on
the LCD. A parameter is typically a number, but can also be one of a small set of
options, like YES/NO, LOW/MEDIUM/HIGH, etc. For numeric values, the amount to change
for each click is determined by the programmer when the menu item is created. If the
user holds down an UP or DOWN button, the value changes continuously until the button
is released.

The SELECT button is used to take some action, as determined by the programmer.

Any given menu item can some or all of monitoring values, setting parameters, and taking
actions, as decided by the programmer.

To create a menu item, start with the following template, select a name for the menu item,
and then add in code to do what you want:

MakeGizmoGardenMenuItem(Name)
{
  switch(event)
  {
    case Enter:
      printName();
      break;

    case Leave:
      break;

    case ChangeAndShow:
      break;

    case Monitor:
      break;

    case Select:
      break
  }
}

Let's take a tour of the template to see how everything works. The symbol "Name" on the
first line is the name of the menu item, so change it to something appropriate. The name
is a C++ identifier, so it must be letters and numbers with no spaces or punctuation, and
must start with a letter.

As you can see, each menu item is programmed to respond to five events, as indicated
by the case statements. Generally a menu item does not need to respond to all five,
and the ones not needed can be deleted or left in place. The events are:

Enter: This happens when the menu item is selected (the user clicks LEFT or RIGHT and
       arrives at this item). Usually you just print the name on the LCD with the printName()
       call, but you can do something else or in addition. When the Enter event begins,
       the LCD is cleared and the cursor is at (0, 0).

Leave: This happens when the user clicks LEFT or RIGHT to move to a different item.
       Usually you do nothing and delete this case.

ChangeAndShow: This happens when the user clicks UP or DOWN. Typically you write code
       to change a value and then print the new value on the LCD. The varaible
       "direction" is set to 1 for UP and -1 for DOWN. You multiply this by the
       amount of change you want if it is different than 1. You can use constrain()
       to keep the value between limits, or modular arithmetic to cycle around.

       When the ChangeAndShow event begins, the cursor is at (2, 1). You can put it
       somewhere else if you want.
       
       Since you'll also want to show the value when the menu item is entered, the
       menu system also does the ChangeAndShow event just after Enter, but with direction
       set to 0 so the value doesn't change.

Monitor: This happens four times a second whenever the menu item is selected, so you can
       monitor a changing value. The cursor starts at (2, 1), and you can print the
       value on the LCD.

Select: This happens when the user clicks the SELECT button.

Note that no other code than described above is needed to make a menu item and have it
recognized by the system.

When printing numbers to the LCD, the Arduino-supplied print functions are generally
insufficient. We need to specify a specific number of columns to use and have the
number right-justified in those columns. The GizmoGardenCommon library has ggPrint
functions for integers, floats, and strings that allow the number of columns to be
specified. See examples below.

You can tell the menu system to select a specific menu item from code. See the example
in setup().

The menu system creates and uses two tasks, one that checks the switches and gets the
events to happen, and one that prints characters on the LCD. The LCD-printing task
is necessary because it takes a surprisingly long time to print each character.
Printing more than one or two characters at a time would bog down the cooperative
multitasking, so the task prints one character at a time, yielding control between
each one. This is all done behind the scenes so that using the GizmoGardenLCDPrint
object is the same as using an ordinary Adafruit LCD.

I strongly recommend speeding up LCD printing by increasing the speed of the Wire
interface. This is done by editing the line in twi.h that says

  #define TWI_FREQ 100000L

Change the "1" to a "4". Under Windows, you can edit using WordPad if you do
"Run As Administrator". For more info and help, search the Web for TWI_FREQ.

This reduces the character printing time on an Arduino Uno from about 5.5 ms to
about 2.1 ms per character. Either is too long for cooperative multitasking without
using the special LCD-printing task, but 2.1 is much better.

Example Code
------------
The following example code implements a menu that allows a user to monitor all
digital and analog pins on an Arduino Uno. The UP/DOWN buttons select which
pin to monitor.

For analog pins, the user can use a GizmoGardenSmoother to smooth out the reading.
Smoothing ranges from 0 to 12, where 0 is no smoothing and 12 is max. See
GizmoGardenCommon.h for details.

For digital pins, the user can monitor both inputs and outputs, and can use the
SELECT switch to flip the state of outputs.
*/

// We need these Adafruit libraries for the LCD shield
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

#include <GizmoGardenCommon.h>
#include <GizmoGardenMultitasking.h>
#include <GizmoGardenLCDPrintTask.h>
#include <GizmoGardenMenus.h>

// Create the LCD-printing task and the menu task.
GizmoGardenLCDPrint lcd;
GizmoGardenMenuTask menuTask(lcd);

// ******************************************************
// *                                                    *
// *  Menu Item to Set Smoothing of Analog Pin Monitor  *
// *                                                    *
// ******************************************************

GizmoGardenSmoother analogMonitor(0); // Start with no smoothing

MakeGizmoGardenMenuItem(Smoothness)
{
  switch(event)
  {
    case Enter:
      printName();
      break;

    case ChangeAndShow:
      // setSmoothness constrains to the 0 - 12 range
      analogMonitor.setSmoothness(analogMonitor.getSmoothness() + direction);
      ggPrint(lcd, analogMonitor.getSmoothness(), 2);
      break;
  }
}

// **************************************
// *                                    *
// *  Menu Item to Monitor Analog Pins  *
// *                                    *
// **************************************

int selectedAnalogPin = 0;

MakeGizmoGardenMenuItem(AnalogPin)
{
  switch(event)
  {
    case Enter:
      printName();
      break;

    case ChangeAndShow:
      // Resetting the GizmoGardenSmoother discards the signal history when the
      // pin changes.
      analogMonitor.reset();
      selectedAnalogPin = (selectedAnalogPin + direction) % 6;
      lcd.setCursor(14, 0);
      ggPrint(lcd, selectedAnalogPin, 1);
      break;

    case Monitor:
      analogMonitor.input(analogRead(selectedAnalogPin));
      ggPrint(lcd, analogMonitor.getOutput(), 4);
      break;
  }
}

// ***************************************
// *                                     *
// *  Menu Item to Monitor Digital Pins  *
// *                                     *
// ***************************************

int selectedDigitalPin = 0;

MakeGizmoGardenMenuItem(DigitalPin)
{
  switch(event)
  {
    case Enter:
      printName();
      break;

    case ChangeAndShow:
      selectedDigitalPin = (selectedDigitalPin + direction) % 14;
      lcd.setCursor(14, 0);
      ggPrint(lcd, selectedDigitalPin, 2);
      break;

    case Monitor:
      ggPrint(lcd, digitalRead(selectedDigitalPin) == LOW ? F("LOW") : F("HIGH"), 4);
      break;

    case Select:
      // The odd-numbered pins are set to OUTPUT.
      if ((selectedDigitalPin & 1) != 0)
        digitalWrite(selectedDigitalPin, digitalRead(selectedDigitalPin) == HIGH ? LOW : HIGH);
      break;
  }
}

// ***********
// *         *
// *  Setup  *
// *         *
// ***********

void setup()
{
  GizmoGardenTask::begin();
  lcd.begin();
  
  // For demonstration purposes, set even-numbered digital pins to INPUT_PULLUP
  // and odd-numbered pins to OUTPUT
  for (int i = 0; i < 14; i += 2)
  {
    pinMode(i, INPUT_PULLUP);
    pinMode(i + 1, OUTPUT);
  }

  // Start with the DigitalPin menu item.
  menuTask.choose(DigitalPin);
  menuTask.start();
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
}
