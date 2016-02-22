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
// *  Flowing Lights  *
// *                  *
// ********************

// This code can be used to create a flowing light show on a NeoPixel
// assembly, for example a rigid ring or a flexible strip. Colors flow
// from one NeoPixel to the next by brightening the leading pixel
// while dimming the trailing pixel. When the leading pixel is fully
// bright and the trailing pixel is off, the display steps to the
// next pixel, so that what was the leading pixel becomes the trailing
// pixel. You can choose the color and full brightnesse of each NeoPixel,
// and the rate at which the sequence advances.
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

// 3) The time (milliseconds) that each NeoPixel is lit before stepping
//    to the next one. Make this smaller to go faster, larger to go
//    slower, but not less than 20.
#define StepTime 20

// 4) Number of steps of brightening and dimming before moving to the
//    next pixel and color in the sequence. Make this larger for slower
//    and smoother transitions.
#define Smooth 12

// This line of code prevents files from the code catalog from
// interfeering with each other due to name conflicts.
namespace FlowingLights {

// 5) Create a table of colors that is used to specify the color (and
//    brightness) of each NeoPixel when it is fully lit. The code is written
//    to cycle through the colors in sequence, and then repeat. There need
//    be no relation between the number of NeoPixels and the number of
//    colors. If the number of NeoPixels is a multiple of the number of
//    colors, each NeoPixel will use only one color. If not, more complex
//    patterns will be generated. The pattern will repeat every N steps,
//    where N is the least common multiple of the number of NeoPixels and the
//    number of colors.
//
//    The following is an example table that you can use as is or modify.
//    You can change the colors, brightnesses, and number of different
//    colors. You can create colors you like using the Mix Color menu that
//    is described in ComputerColors.pdf.
//
//    The red, green, and blue components of each color in this table sum to
//    255 or 256, and the brightening and dimming is arranged so that the sum
//    of the red, green, and blue components of the two pixels that are lit at
//    any time is no more than 256. The power used is about 0.1W.
NeoPixelColor colors[] =
{
//  Red Green Blue
  { 255,   0,   0},   // red
  {  92,  64,   0},   // orange
  { 128, 128,   0},   // yellow
  {  64, 192,   0},   // lime
  {   0, 255,   0},   // green
  {   0, 192,  64},   // 
  {   0, 128, 128},   // cyan
  {   0,  64, 192},
  {   0,   0, 255},   // blue
  {  64,   0, 192},   // purple
  { 128,   0, 128},   // magenta
  { 192,   0,  64}
};

// 6) Once you are done setting everything up, you can set this to false
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

// Which step of brightening and dimming we're on. Values range fron 0 to
// Smooth.
int smoothCycle = 0;

#define NumberOfColors (sizeof(colors) / sizeof(NeoPixelColor))

// Ignore this comment unless you are a C++ expert:
// Put the flowColor function in the global namespace to avoid ambiguity
// with the function prototype that the Arduino IDE makes. We could keep
// the function in the FlowingLights namespace and use a qualified
// name to call the function, but maybe this makes the code less ugly.
};

// This function dims a color by phase/Smooth, and is used for both
// brightening and dimming.
NeoPixelColor flowColor(NeoPixelColor c, int phase)
{
  NeoPixelColor f;
  f.red   = c.red   * phase / Smooth;
  f.green = c.green * phase / Smooth;
  f.blue  = c.blue  * phase / Smooth;
  return f;
}

// Now back to our namespace
namespace FlowingLights {

// Create a task called FlowTask to run the light show, simultaneously
// with all other tasks that may be running. This code tells what happens
// when it's FlowTask's turn to run. For ech turn to run that FlowTask
// gets, it makes one step of brightening and dimming. The speed of the
// action is determined by how much time elpases between each turn, which
// you can change.
MakeGizmoGardenTaskWithStop(FlowTask)
{
  // Set the color and brightness of the leading and trailing pixels
  int lightNumber1 = (lightNumber + 1) % NumberOfPixels;
  int colorIndex1 = (colorIndex + 1) % NumberOfColors;
  lights.setPixelColor(lightNumber , flowColor(colors[colorIndex ], Smooth - smoothCycle));
  lights.setPixelColor(lightNumber1, flowColor(colors[colorIndex1],          smoothCycle));
  
  // The lights don't actually change until show is called.
  lights.show();

  // Advance to next phase. If done with the current brightening and dimming cycle,
  // advance to next NeoPixel and color.
  ++smoothCycle;
  if (smoothCycle > Smooth)
  {
    smoothCycle = 0;
    lightNumber = lightNumber1;
    colorIndex = colorIndex1;
  }

  // Ask for another turn to run.
  callMe(StepTime);
}

// When SpinTask is stopped, turn off any NeoPixel that may be lit.
CustomStop(FlowTask)
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
      FlowTask.start();
      break;
      
    case DriveStop:
      FlowTask.stop();
      break;
  }
}

GizmoGardenEventRegistry event(eventFunction);

#if IncludeMenus
// Make the color mixer menu
GizmoGardenColorMixer mixer(lights, &FlowTask);
#endif

};  // No idea why this semicolon is needed, seems like a compiler or IDE bug

#undef DigitalPinNumber
#undef NumberOfPixels
#undef StepTime
#undef Smooth
#undef NumberOfColors
#undef IncludeMenus

