/***************************************************************************
Copyright (c) 2016 Bob Conger, Agent Gizmo Garden LLC, and Bill Silver,
Member of Gizmo Garden LLC. This source code is distributed under terms of
the GNU General Public License, Version 3, which grants certain rights to
copy, modify, and redistribute. The license can be found at
<http://www.gnu.org/licenses/>. There is no express or implied warranty,
including merchantability or fitness for a particular purpose.
***************************************************************************/

// *********************
// *                   *
// * SnakeLightSlither *
// *                   *
// *********************

// This code, which is a derivative of SpinningLights, creates a
// snake that slithers around a NeoPixel sheet on a pathway that is
// totally determined by the user via a data array in this code. The code
// lights up several NeoPixels at a time on the user-determined path. You 
// can choose the length of the snake, color and brightness of each NeoPixel, 
// and the rate at which the sequence advances.
//
// This is a good sketch on which to experiment changing the various 
// elements that define the journey of the snake. Try some variations!

// **************
// *            *
// *  Settings  *
// *            *
// **************

// Digital pin number to which the motor control signal is wired
#define DigitalPinNumber 3

// This line of code prevents files from the code catalog from
// interfeering with each other due to name conflicts.
namespace SnakeLight {

// Number of NeoPixels in your assembly.
// Note that SnakeLightSlither is intended for use with 8x8 NeoPixel sheet
// i.e., 64 NeoPixels
#define NumberOfPixels 64

// Length of snake (number of NeoPixels you want lit at any one time)
// NOTE: If using long Snakelengths, reduce the settings in the light table
// so that the total on at any time << 1275 (and really, less than 200 is
// pretty bright and should be sufficient)
#define SnakeLength 5

// The time (milliseconds) that each NeoPixel is lit before stepping
// to the next one.  A higher number means that the snake will pause longer
// at each NeoPixel, and thus will travel more slowly.
#define StepTime 100

// Specify the path that you want the snake to follow.
// In SnakeLightSlither, the user can define the path of the snake's movement
// around the NeoPixel sheet.  See paper worksheet for the light number configuration
// on the NeoPixel sheet, and use the paperworksheet to plan out the path you wish
// to define for the snake.
//
// The snake's path can be of any length, can jump around, can repeat NeoPixels,
// and does not have to use every NeoPixel on the sheet.  
//
// The variable SnakePath[] is used to specify the order in which the snake is
// to travel along the NeoPixel sheet.  Enter the NeoPixel numbers from the path
// you have planned on the paper worksheet, into the variable SnakePath.  The
// variable already includes the NeoPixel numbers for a sample path.
//
char SnakePath[]={
        0,   1,   2,   3,     4,   5,   6,   7,
      8,  23,  24,  39,    40,  55,  56,  57,
     58,  59,  60,  61,    62,  63,  48,  47,
     32,  31,  16,  15,    14,  13,  12,  11,
     10,   9,  22,  25,    38,  41,  54,  53,
     52,  51,  50,  49,    46,  33,  30,  17,
     18,  19,  20,  21,    26,  37,  42,  43,
     44,  45,  34,  29,    28,  27,  36,  35,
     34,  33,  32,  31,    16,  15
  };

// Create a table of colors that is used to specify the color (and
// brightness) of each NeoPixel when it is lit. The code is written to cycle
// through the colors in sequence, and then repeat. There need be no relation between the number 
// of NeoPixels and the number of colors. If SnakePathLenght is a 
// multiple of the number of colors, each NeoPixel will use the same color on every 
// cycle.  If not, more complex patterns will be generated.
// The pattern will repeat every N steps, where N is the least common
// multiple of SnakePathLength and the number of colors.
//
// Use the Color Mix tool on the dashboard to experiment with recipes for
// colors you like, and add them to the table below, or replace some of the 
// rows of the table with colors you prefer for your display.  The color table
// can have as few as 1 row (defining just a single color) or as many different
// rows (colors) as you prefer, and the colors can be in any order you wish.
// Suggestion:  On the worksheet you used to plan the snake's path, write down
// the 3-color recipe for any color combinations that you decide you want to 
// include in the snake's journey, and jot down a short descriptive label to 
// help you remember what color that recipe produces (such as "light pink").
//
// The red, green, and blue components of each color in this table sum to
// 32, and if four NeoPixels are on at a time, the power used is about
// 0.05W.  
// Waarning -- if using SnakeLength longer than 10, scale lights down further)
NeoPixelColor colors[] =
{
//  Red Green Blue
  { 32,   0,   0},
  {  24,  8,   0},
  {  16,  16,   0},
  {  8,  24,   0},
  {   0, 32,   0},
  {   0,  24,  8},
  {   0,  16,  16},
  {   0,  8,  24},
  {   0,   0, 32},
  {  8,   0,  24},
  {  16,   0,  16},
  {  24,   0,  8}
};

// If you define the RandomColor to be "true", each NeoPixel will be 
// assigned a color at random from the color table above, rather than
// cycling through the color table in sequential order.
// 
#define RandomColor false

// Once you are done setting everything up, you can set this to false
// to remove the Mix Color menu from the dashboard to make it easier
// to find what you really want. You can always get the menu back later
// by setting it back to true.
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

// Calculate the number of points in the SnakePath
#define SnakePathLength  (  sizeof(SnakePath)  /  sizeof(char)  )

// Create an integer variable that will hold the index number of the SnakePath element
// currently being worked on. Initialize so the the first element worked on,
// when the program enters the first pass through the programming loop, will be 
// the first element of the SnakePath, with index = 0.
int SnakeNumber = SnakePathLength - 1;

// Create an array that will hold the NeoPixel numbers of the current snake position,
// with the first element of the array [index number 0] being the "head" of the snake,
// the direction of movement.  This is the element of the array that will be entering
// a new NeoPixel each turn through the programming loop.
int LightNumArray[SnakeLength];

// To initialize the LightNumArray variable, set each element of the variable equal to 65, a non-existent
// NeoPixel, as if the entire snake started out by being curled up just off the edge of the
// NeoPixel sheet, near the first NeoPixel of your snake path, just waiting for the 
// opportunity to stretch out and slither along the path you have defined.  
// This initialization takes place in the EventRegistry section near the very bottom
// of this code.

#define NumberOfColors (sizeof(colors) / sizeof(NeoPixelColor))

// Create an integer variable that holds the index in the color table of
// the color mixture of the current NeoPixel.  Initialize this variable so that
// the first color selected will be the color in the first row of the color table
// (unless you have selected the RandomColor selection of colors from the table).
int colorIndex = NumberOfColors - 1  ;

// Create a task called SnakeTask to run the light show, simultaneously
// with all other tasks that may be running. This code tells what happens
// when it's SnakeTask's turn to run. For each turn to run that SnakeTask
// gets, it moves the snake one position. The speed of the snake is determined
// by how much time elpases between each turn, which you can change.
MakeGizmoGardenTaskWithStop(SnakeTask)
{

// Turn off the NeoPixel that is currently lit as the snake's tail, 
// and move the snake forward by one cell
// New NeoPixel for snake's head will be determined and lit in next step
//
 lights.setPixelColor(LightNumArray[SnakeLength-1], 0) ;
 int i ;
 for (  i=1 ;  i < SnakeLength ;  i = i + 1  ) {
      LightNumArray[ SnakeLength - (i) ]  =  LightNumArray[ SnakeLength - (i+1) ] ;
      }

//Now, move the snake's head by one cell along the defined path
//
  SnakeNumber  =  ( SnakeNumber + 1 ) % SnakePathLength ;
  LightNumArray[0]= SnakePath[SnakeNumber];

//Next. select the color for the snake's new head position
  // When the color is updated sequentially (i.e. not
  // randomly), update it by adding 1 and then using the %
  // operation to create a cycle. The % operation does "clock arithmetic".
  // Think about 24-hour time, where 1 hour past 23:00 is 0:00. Written
  // in C++, this would be (23 + 1) % 24 is 0. This is also like the
  // odometer on a car that reads 999999 miles and then you drive 1 more
  // mile: (999999 + 1) % 1000000 is 0. Mathematicians and programmers
  // call the % operation "modulo".
  
  if (RandomColor)
    colorIndex = random(NumberOfColors);
  else
    colorIndex = (colorIndex + 1) % NumberOfColors;

  // Now that we have a new NeoPixel and a new color index, light it up.
  lights.setPixelColor(LightNumArray[0], colors[colorIndex]);

  // The lights don't actually change until show is called.
  lights.show();

  // Ask for another turn to run.
  callMe(StepTime);
}

// When SnakeTask is stopped, turn off any NeoPixel that may be lit.
CustomStop(SnakeTask)
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

// initialize the variable LightNumArray, which will hold the NeoPixel numbers of
// the current snake position at any time.  For the initialization settings, we set
// each cell of the snake to be at non-existent NeoPixel 65, i.e., off the NeoPixel sheet.
       int j ; 
       for (  j=0 ;  j < SnakeLength ;  j = j + 1  ) {
            LightNumArray[ j ]  =  65 ;
            }
      
    case DriveStart:
      SnakeTask.start();
      break;
      
    case DriveStop:
      SnakeTask.stop();
      break;
  }
}

GizmoGardenEventRegistry event(eventFunction);

// Make the color mixer menu
#if IncludeMenus
GizmoGardenColorMixer mixer(lights, &SnakeTask);
#endif

};

#undef DigitalPinNumber
#undef NumberOfPixels
#undef NumberOfColors
#undef StepTime
#undef SnakePathLength
#undef RandomColor
#undef SnakeLength
#undef IncludeMenus

