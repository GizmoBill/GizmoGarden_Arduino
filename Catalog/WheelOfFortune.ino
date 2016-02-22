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
// *  Wheel of Fortune  *
// *                    *
// **********************

// This code can be used to create a "Wheel of Fortune" using a NeoPixel
// assembly, for example a rigid ring or a flexible strip. The code lights
// up one NeoPixel at a time, in sequence around the ring, starting fast
// and then slowing down to a stop. Clicking sounds are made with each
// advance. The code uses a sequence of colors and brightnesses that you
// choose.
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

// 3) After the wheel comes to a stop, wait this amount of time in
//    milliseconds and then spin it again
#define RepeatAfterPause 5000

// 4) Wheel friction setting. Bigger values make the wheel slow dowm
//    faster. If you make this 0, the wheel has no friction and will
//    never stop, so maybe you don't want to do that.
#define Friction 0.05

// 5) The pitch and time for the clicks. Make either one 0 for no clicks.
#define ClickPitch 3000
#define ClickTime 5

// 6) Set this to true to choose the color for each step from the color
//    table (below) randomly instead of sequentially cycling around the
//    table.
#define RandomColor false

// This line of code prevents files from the code catalog from
// interfeering with each other due to name conflicts.
namespace WheelOfFortune {

// 7) Create a table of colors that is used to specify the color (and
//    brightness) of each NeoPixel when it is lit. The code is written to
//    cycle through the colors in sequence, and then repeat, or to choose
//    the colors from ths table randomly. When the pixels and colors are
//    chosen sequentially, there need be no relation between the number of
//    NeoPixels and the number of colors. If the number of NeoPixels is a
//    multiple of the number of colors, each NeoPixel will use only one
//    color. If not, more complex patterns will be generated. The pattern
//    will repeat every N steps, where N is the least common multiple of the
//    number of NeoPixels and the number of colors.
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
  {  32,  96,   0},   // lime
  {   0, 128,   0},   // green
  {   0,  96,  32},   // 
  {   0,  64,  64},   // cyan
  {   0,  32,  96},   //
  {   0,   0, 128},   // blue
  {  32,   0,  96},   // purple
  {  64,   0,  64},   // magenta
  {  96,   0,  32}    //
};

// 8) Once you are done setting everything up, you can set this to false
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

// Create an integer variable that holds the number of the NeoPixel
// currently lit.
int lightNumber = 0;

// Create an integer variable that holds the index in the color table of
// the color mixture of the current NeoPixel.
int colorIndex = 0;

float stepTime = 20;

#define NumberOfColors (sizeof(colors) / sizeof(NeoPixelColor))

// Create a task called WheelTask to run the light show, simultaneously
// with all other tasks that may be running. This code tells what happens
// when it's WheelTask's turn to run. For ech turn to run that WheelTask
// gets, it turns off the current NeoPixel and lights up the necxt one. The
// speed of the sequence is determined by how much time elpases between each
// turn, which is determined by the StepTime setting.
MakeGizmoGardenTaskWithStop(WheelTask)
{
  // Turn off the NeoPixel that is currently lit.
  lights.clear();

  // Update the light number by adding 1 and then using the %
  // operation to create a cycle. The % operation does "clock arithmetic".
  // Think about 24-hour time, where 1 hour past 23:00 is 0:00. Written
  // in C++, this would be (23 + 1) % 24 is 0. This is also like the
  // odometer on a car that reads 999999 miles and then you drive 1 more
  // mile: (999999 + 1) % 1000000 is 0. Mathematicians and programmers
  // call the % operation "modulo".
  lightNumber = (lightNumber + 1) % NumberOfPixels;

  // Update color, randomly or sequentially
  if (RandomColor)
    colorIndex = random(NumberOfColors);
  else
    colorIndex = (colorIndex + 1) % NumberOfColors;

  // Now that we have a new NeoPixel and a new color index, light it up.
  lights.setPixelColor(lightNumber, colors[colorIndex]);

  // The lights don't actually change until show is called.
  lights.show();

  // Make clicks
  GizmoGardenTone(ClickPitch, ClickTime);

  if (stepTime < 300)
  {
    callMe(stepTime);
    stepTime += stepTime * Friction;
  }
  else
  {
    stepTime = 20;
    lightNumber = random(NumberOfPixels);
    callMe(RepeatAfterPause);
  }
}

// When WheelTask is stopped, turn off any NeoPixel that may be lit.
CustomStop(WheelTask)
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
      WheelTask.start();
      break;
      
    case DriveStop:
      WheelTask.stop();
      break;
  }
}

GizmoGardenEventRegistry event(eventFunction);

#if IncludeMenus
// Make the color mixer menu
GizmoGardenColorMixer mixer(lights, &WheelTask);
#endif

};

#undef DigitalPinNumber
#undef NumberOfPixels
#undef RandomColor
#undef NumberOfColors
#undef RepeatAfterPause
#undef Friction
#undef ClickPitch
#undef ClickTime
#undef IncludeMenus

