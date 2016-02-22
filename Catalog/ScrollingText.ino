/***************************************************************************
Copyright (c) 2016 Bill Silver, Member of Gizmo Garden LLC. This source code
is distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The license
can be found at <http://www.gnu.org/licenses/>. There is no express or
implied warranty, including merchantability or fitness for a particular
purpose.
***************************************************************************/

// ********************
// *                  *
// *  Scrolling Text  *
// *                  *
// ********************

// This code uses an Adafruit 8x16 matrix of LEDs to display a message that
// scrolls by in Times Square fashion. You choose the message and the rate
// at which it moves. You can also set the brightness of the LEDs, but keep
// in mind that brighter means less battery life. You can make the message
// right side up or upside down, so that you can mount the display on your
// gizmo in whatever orientation is convenient. 

// **************
// *            *
// *  Settings  *
// *            *
// **************

// 1) Change the text between the quotes to whatever message you want.
#define Message "Welcome to Gizmo Garden!"

// 2) The message moves in steps that take this amount of time in
//    milliseconds. Make the time longer to move slower, shorter to move
//    faster, but not less than 20. One character of the message moves by
//    in six steps.
#define StepTime 40

// 3) Set this to the brightness of the LEDs that you want. The values
//    range from 0 to 15, where 0 is off and 15 is max brightness (and
//    max drain on the batteries).
#define Bright 8

// 4) Set this to false if the LED's connector is on the left, and true if
//    the connector is on the right.
#define UpsideDown false

// ==========================================================================

// *****************
// *               *
// *  Source Code  *
// *               *
// *****************

// This line of code prevents files from the code catalog from
// interfeering with each other due to name conflicts.
namespace ScrollingText
{

// Create an Adafruit_8x16matrix called led8x16 that can be used to
// control the display.
Adafruit_8x16matrix led8x16;

// Make names for some constants to make the code clearer and so that the
// values can be changed easily for different displays and fonts.
#define CharWidth 6   // width of one character in pixels
#define LedWidth 16   // width of the display in pixels

// Make a text string to hold the message that the code can use.
// GizmoGardenText behaves like a normal C++ string, but saves memory by not
// coping the string to dynamic memory.
MakeGizmoGardenText(message, Message)

// Make an integer variable that holds the current position of the
// left-most visible character of the message, relative to the left side
// of the display. Note that this number will be negative when the left-
// most visible character has scrolled partly off the display.
int scrollX = LedWidth - 1;

// Make an integer variable that holds the position in the message of the
// left-most visible character. Note that the first character of the
// message is at position 0.
int charPosition = 0;

// Create a task called ScrollTask to run the display, simultaneously
// with all other tasks that may be running. This code tells what happens
// when it's ScrollTask's turn to run. For ech turn to run that ScrollTask
// gets, it erases the entire display and rewrites the visible portion of
// the message at the new position. The speed of scrolling is determined by
// how much time elpases between each turn, which is determined by the
// StepTime setting.
MakeGizmoGardenTaskWithStop(ScrollTask)
{
  // Erase the entire display
  led8x16.clear();

  // Set the display's cursor to the current position at which to show
  // the visible portion of the message, relative to the left side of the
  // display.
  led8x16.setCursor(scrollX, 0);

  // Loop over the visible portion of the message and write each character.
  int charIndex = charPosition;
  for (int column = scrollX; column < LedWidth; column += CharWidth)
  {
    // Get the current character. It will be 0 if we've reached the end of
    // the message.
    char c = message[charIndex];

    // End of message?
    if (c != 0)
    {
      // No, write the character and step to the next one
      led8x16.print(c);
      ++charIndex;
    }
    else
      // Yes, we're done with the loop
      break;
  }

  // The display doesn't actually change until we do this
  led8x16.writeDisplay();

  // Move the message left one pixel for next time
  --scrollX;

  // Is the first visible character of the message no longer visible?
  if (scrollX <= -(CharWidth - 1))
  {
    // Yes, no longer visible, advance to the next character
    ++charPosition;

    // At end of message?
    if (message[charPosition] == 0)
    {
      // Yes, go back to the beginning of the message, all the way to
      // the right side of the display
      scrollX = LedWidth - 1;
      charPosition = 0;
    }
    else
      // Not at end of message
      scrollX = 1;
  }

  // Ask for another turn to run.
  callMe(StepTime);
}

// When ScrollTask is stopped, erase the display.
CustomStop(ScrollTask)
{
  led8x16.clear();
  led8x16.writeDisplay();
}

void eventFunction(byte eventCode)
{
  switch (eventCode)
  {
    case GizmoSetup:
      led8x16.begin(0x70);
      led8x16.setTextWrap(false);
      led8x16.setRotation(UpsideDown ? 3 : 1);
      led8x16.setBrightness(Bright);
      
    case DriveStart:
      ScrollTask.start();
      break;
      
    case DriveStop:
      ScrollTask.stop();
      break;
  }
}

GizmoGardenEventRegistry event(eventFunction);

};

#undef Message
#undef StepTime
#undef CharWidth
#undef LedWidth
#undef Bright
