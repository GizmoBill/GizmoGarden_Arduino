/***************************************************************************
Copyright (c) 2016 Bill Silver, Member of Gizmo Garden LLC. This source code
is distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The license
can be found at <http://www.gnu.org/licenses/>. There is no express or
implied warranty, including merchantability or fitness for a particular
purpose.
***************************************************************************/

// *********************
// *                   *
// *  Bouncing Lights  *
// *                   *
// *********************

// This code can be used to create a bouncing ball effect on a NeoPixel
// assembly, for example a flexible strip. The code lights up one NeoPixel
// at a time to represent the "ball", which bounces up and down along the
// assembly under the influence of "gravity". The  ball makes a sound when
// it bounces off of the "floor". The motion is mathematically correct for
// constant Newtonian gravity, no friction, and an elastic bounce at the
// bottom.
//
// You can set the strength of gravity, the colors and brightnesses to use
// as the ball moves, and the sound made when the ball bounces.
//
// A dashboard menu called Mix Color lets you experiment with colors by
// mixing the primary colors red, green, and blue, and the secondary colors
// yellow, magenta, and cyan. Colors and the Mix Color menu are described
// in the document ComputerColors.pdf.


// **************
// *            *
// *  Settings  *
// *            *
// **************

// 1) Digital pin number to which the NeoPixel control signal is wired
#define DigitalPinNumber 3

// 2) Number of NeoPixels in your assembly.
#define NumberOfPixels 24

// 3) Set this to true to choose the color for each step from the color
//    table (below) randomly instead of sequentially cycling around the
//    table.
#define RandomColor false

// 4) This is the strength of gravity, in somewhat arbitrary units.
#define Gravity 10.0

// 5) Set these to the pitch and duration of the beep you want when the
//    ball bounces off the "floor". Set either one to 0 for no beep.
#define BounceTonePitch 2400
#define BounceToneDuration 100

// This line of code prevents files from the code catalog from
// interfeering with each other due to name conflicts.
namespace BouncingLights {

// 6) Create a table of colors that is used to specify the color (and
//    brightness) of each NeoPixel when it is lit. If RandomColor is false,
//    the code will step through the table once as the ball moves from top
//    to bottom or bottom to top. So for example, if you have 24 NeoPixels
//    and 8 colors, each color will be used three times in a row before
//    stepping to the next color. The example colors are chosen to
//    approximate the colors of the rainbow, with the red end used when the
//    ball is moving slowly at the top and the violet end used when the ball
//    is moving rapidly at the bottom.
//
//    The following is an example table that you can use as is or modify.
//    You can change the colors, brightnesses, and number of different
//    colors. You can create colors you like using the Mix Color menu that
//    is described in ComputerColors.pdf.
//
//    The red, green, and blue components of each color in this table sum to
//    128, and since only one NeoPixel is on at a time, the power used is
//    about 0.05W.
NeoPixelColor colors[] =
{
//  Red Green Blue
  { 128,   0,   0},   // red
  {  96,  32,   0},   // orange
  {  64,  64,   0},   // yellow
  {   0, 128,   0},   // green
  {   0,  64,  64},   // cyan
  {   0,   0, 128},   // blue
  {  32,   0,  96},   // purple
  {  43,   0,  85},   // purple
};

// 7) Once you are done setting everything up, you can set this to false
//    to remove the Mix Colors menu from the dashboard to make it easier to
//    find what you really want. You can always get the menu back later by
//    setting it back to true.
#define IncludeMenus true

// ==========================================================================

// *****************
// *               *
// *  Source Code  *
// *               *
// *****************

// Create a GizmoGardenPixels object called lights that can be used to
// control a NeoPixel assembly.
GizmoGardenPixels lights(NumberOfPixels, DigitalPinNumber);

#define NumberOfColors (sizeof(colors) / sizeof(NeoPixelColor))

// Create an integer variable that holds the number of the NeoPixel
// currently lit.
int lightNumber = 0;

// Create an integer variable that holds the current direction of
// motion: 1 is down and -1 is up
int direction = 1;

// Create a float variable that holds the time in milliseconds from when
// the ball was at the top to when the ball is halfway between the current
// NeoPixel and the next one.
float stepTime  = 0;

// Create a task called BounceTask to run the light show, simultaneously
// with all other tasks that may be running. This code tells what happens
// when it's BounceTask's turn to run. For ech turn to run that BounceTask
// gets, it moves the ball one position. The speed of the ball is determined
// by how much time elpases between each turn. The time is adjusted after
// each turn to create the appearance of constant acceleration.
MakeGizmoGardenTaskWithStop(BounceTask)
{
  // Turn off the NeoPixel that is currently lit.
  lights.setPixelColor(lightNumber, 0);

  // Update the number of the lit NeoPixel in the direction the ball is
  // moving.
  lightNumber += direction;

  // Apply the laws of physics. Each step of the motion is a constant
  // distance and a variable amount of time. Calculate the time t from
  // when the ball was at the top to when it is halfway between the
  // current NeoPixel and the next one. By subtracting this time from
  // the previous time, we get how long the current NeoPixel should be
  // lit. We then use callMe to ask that BounceTask get its next turn
  // to run in that amount of time.
  if (lightNumber > 0)
  {
    // Since the variable t is created inside the {} of this if statement,
    // it is called a "local" variable. It is invisible to any code outside
    // the {}, so you don't have to worry about other code using the
    // same name, and it only uses computer memory when the {} is
    // running.
    float t = 1000 * sqrt((lightNumber + 0.5 * direction) / Gravity);
    callMe(direction * (t - stepTime));
    stepTime = t;
  }
  else
    // The case where the ball is at the top and reverses direction is
    // different.
    callMe(2 * stepTime);

  switch (lightNumber)
  {
    // When the ball reaches the top, reverse direction
    case 0:
      direction = 1;
      break;

    // When the ball reaches the bottom, reverse direction and make a sound
    case NumberOfPixels - 1:
      direction = -1;
      GizmoGardenTone(BounceTonePitch, BounceToneDuration);
      break;
  }

  // Choose color from the table, either randomly or sequentially
  int colorIndex;
  if (RandomColor)
    colorIndex = random(NumberOfColors);
  else
    colorIndex = lightNumber * NumberOfColors / NumberOfPixels;
  lights.setPixelColor(lightNumber, colors[colorIndex]);

  // The lights don't actually change until show is called.
  lights.show();
}

// When BounceTask is stopped, turn off any NeoPixel that may be lit.
CustomStop(BounceTask)
{
  lights.clear();
  lights.show();
}

void eventFunction(byte eventCode)
{
  switch (eventCode)
  {
    case GizmoSetup:
      lights.begin();
      
    case DriveStart:
      BounceTask.start();
      break;
      
    case DriveStop:
      BounceTask.stop();
      break;
  }
}

GizmoGardenEventRegistry event(eventFunction);

// Make the color mixer menu
#if IncludeMenus
GizmoGardenColorMixer mixer(lights, &BounceTask);
#endif

};

#undef DigitalPinNumber
#undef NumberOfPixels
#undef RandomColor
#undef Gravity
#undef BounceTonePitch
#undef BounceToneDuration
#undef NumberOfColors
#undef IncludeMenus

