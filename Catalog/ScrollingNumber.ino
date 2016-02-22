/***************************************************************************
Copyright (c) 2016 Bill Silver, Member of Gizmo Garden LLC. This source code
is distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The license
can be found at <http://www.gnu.org/licenses/>. There is no express or
implied warranty, including merchantability or fitness for a particular
purpose.
***************************************************************************/

// **********************
// *                    *
// *  Scrolling Number  *
// *                    *
// **********************

// This code uses an Adafruit 8x16 matrix of LEDs to display a number that
// scrolls by in Times Square fashion. You choose the number and the rate
// at which it moves. You can also set the brightness of the LEDs, but keep
// in mind that brighter means less battery life. You can make the message
// right side up or upside down, so that you can mount the display on your
// gizmo in whatever orientation is convenient. 

// **************
// *            *
// *  Settings  *
// *            *
// **************

// 1) The message moves in steps that take this amount of time in
//    milliseconds. Make the time longer to move slower, shorter to move
//    faster, but not less than 20. One character of the message moves by
//    in six steps.
#define StepTime 80

// 2) Set this to the brightness of the LEDs that you want. The values
//    range from 0 to 15, where 0 is off and 15 is max brightness (and
//    max drain on the batteries).
#define Bright 8

// 3) Set this to false if the LED's connector is on the left, and true if
//    the connector is on the right.
#define UpsideDown false

// 4) Set these to the total number of characters to use to show the
//    number, and the number of digits to the right of the decimal point.
#define NumCharacters 6
#define NumPlaces 4

// To set the number x to display, do this:
//    setScrollingNumber(x);
//
// The number will scroll across the display once. If you set a new number
// before the previous one is finished, the new number begins scrolling
// immediatlly. The total time needed to scroll a number, in milliseconds,
// is
//    (6 * NumCharacters + 14) * StepTime

// ==========================================================================

// *****************
// *               *
// *  Source Code  *
// *               *
// *****************

// This line of code prevents files from the code catalog from
// interfeering with each other due to name conflicts.
namespace ScrollingNumber
{

// Create an Adafruit_8x16matrix called led8x16 that can be used to
// control the display.
Adafruit_8x16matrix led8x16;

// Make names for some constants to make the code clearer and so that the
// values can be changed easily for different displays and fonts.
#define CharWidth 6   // width of one character in pixels
#define LedWidth 16   // width of the display in pixels

// Make an integer variable that holds the current position of the
// left-most visible character of the message, relative to the left side
// of the display. Note that this number will be negative when the left-
// most visible character has scrolled partly off the display.
int scrollX = 0;

// Make an integer variable that holds the position in the message of the
// left-most visible character. Note that the first character of the
// message is at position 0.
int charPosition = 0;

// Derive a class from Print that contains an array into which the characters
// of a number can be help.
class MessagePrinter : public Print
{
  virtual size_t write(uint8_t);
  char message[NumCharacters + 1];  // One more for 0 termination
  byte nextCharIndex;
  
public:
  char operator[](int index) const { return message[index]; }

  void setNumber(float x);
}
message;

size_t MessagePrinter::write(byte c)
{
  if (nextCharIndex < NumCharacters)
    message[nextCharIndex++] = c;
}

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
    if (message[charPosition] != 0)
      // Not at end of message
      scrollX = 1;
    else
      // At end, task stops by returning without doing the callMe
      return;
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

void MessagePrinter::setNumber(float x)
{
  nextCharIndex = 0;
  print(x, NumPlaces);
  write(0);     // 0 termination if number uses fewer than NumCharacters
  scrollX = LedWidth - 1;
  charPosition = 0;
  ScrollTask.start();
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
      message.setNumber(1.2345);
      break;
  }
}

GizmoGardenEventRegistry event(eventFunction);

};

void setScrollingNumber(float x)
{
  ScrollingNumber::message.setNumber(x);
}

#undef StepTime
#undef CharWidth
#undef LedWidth
#undef Bright
#undef NumCharacters 6
#undef NumPlaces 4

