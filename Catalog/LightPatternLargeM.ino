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
// *  LightPatternM    *
// *                   *
// *********************

// This code creates a single pattern that doesn't move, in this case a large
// letter "M". It is an example of code that could
// be used to create any still pattern on the NeoPixel sheet.  

// The pattern is totally determined by the user via a data array in this code. You 
// can design the patterns and choose color and brightness of each NeoPixel, 
// and sound effects.  

// Just for fun, this particular sketch is designed so that the color
// changes gradually (even though the shape does not change), and it is easy 
// to modify the rate at which the color changes.
// With a few relatively minor changes in the code, an easier pattern to design
// and implement would be a pattern for which neither the pattern nor the color changes
 
// Within the general LightPattern code, the pattern (which can have 
// different numbers of phases within it)is
// totally determined by the user via a data array in this code. You 
// can design the patterns and choose color and brightness of each NeoPixel, 
// sound effects, and the rate at which the sequence advances.

// **************
// *            *
// *  Settings  *
// *            *
// **************

// Digital pin number to which the motor control signal is wired
#define DigitalPinNumber 3

// This line of code prevents files from the code catalog from
// interfeering with each other due to name conflicts.
namespace LargeM {

// DEFINE YOUR PATTERNS 
// In LightPattern, the user can define a series of patterns that will be
// illuminated on the NeoPixel sheet.  The NeoPixel numbers (0 to 63) for all the patterns 
// are entered into an array "PatternData" sequentially. The variables PatternStart and 
// PatternStop tell the code which range of entries in PatternData to use in creating
// the first pattern, the second pattern, etc.  The variable PatternTime controls the length
// of time that each pattern is displayed.  
//
// Entries in the PatternData array should be NeoPixel numbers, 0 to 63.  Use the 
// paper worksheet to plan out your patterns on the 8x8 grid, and to find the NeoPixel
// number for each light you wish to illuminate in your pattern.  If you are going
// to use more than one pattern, simply list the NeoPixel number for the next pattern
// after you have finished listing all the NeoPixel numbers for the current pattern.
//
// (Note: The entries in PatternData for the LargeM sketch actually produce two complete
// copies of the large letter M (note that pattern of NeoPixels repeats, starting halfway
// through the entries).  The first complete version of the letter M is Green (see color 
// controls below) and the second complete version of the letter M is Blue.  The NeoPixels
// for each copy of the Letter M are broken up into a few NeoPixels per line of text
// below for reader ease in following the way that the PatternStart and PatternStop variables
// (later) control a gradual shift of portions of the letter M from green to blue,
// and back again to green.)
//
char PatternData[]= {
    0,  15,   1,
   16,  14,  31,  17,
   32,  30,  47,  33,
   48,  46,   6,  63,  49,  45,  35,   9,   7,
   62,  50,  44,  36,  22,   8,  43,  25,  23,
   42,  38,  24,  53,  41,  39,
   54,  40,  57,  55,
   56,
    0,  15,   1,
   16,  14,  31,  17,
   32,  30,  47,  33,
   48,  46,   6,  63,  49,  45,  35,   9,   7,
   62,  50,  44,  36,  22,   8,  43,  25,  23,
   42,  38,  24,  53,  41,  39,
   54,  40,  57,  55,
   56  
  } ;

// NOW PROVIDE THE KEY INFORMATION THAT WILL CONTROL HOW THE GIZMO USES THE
// LIST OF NEOPIXELS THAT YOU PROVIDED IN PatternData
//
// NumOfPatterns:  How many different patterns do you want to define and utilize
// from the list of NeoPixels in PatternData above?  
// This code is designed to cycle continuously through the list of patterns that
// you define.
// This could be calculated from the size of the PatternStart array, but
// we allow it to be set independently for testing and experimentation.
#define NumOfPatterns  16

// The variables PatternStart and PatternStop tell the Gizmo how to find
// the start and finish of each pattern from the list of numbers that you
// entered into the PatternData array, above.
// The smallest and largest entries in PatternStart and PatternStop should not 
// fall outside the limits of the index number of the first and last entries in PatternData.
// The first entry in PatternData is indexed at (zero); the last entry in
// PatternData is indexed at (the number of entries in PatternData, minus 1).
//  
// Creative Note:  Patterns can overlap in the array, and this Large M sketch uses that flexibility
// extensively.  Observe that each pattern includes 40 NeoPixels, and only a few of them are different
// from one pattern to the next.

char PatternStart[] = {   0,  3,  7, 11, 20, 29, 35, 39, 40, 39, 35, 29, 20, 11,  7,  3    };
char PatternStop[]  = {  39, 42, 46, 50, 59, 68, 74, 78, 79, 78, 74, 68, 59, 50, 46, 42    };

// Now, define how long you want each pattern to display before moving on to
// the next pattern and continuing the cycle.
// PatternTime entries are in milliseconds
//
// Note that for LargeM, PatternTime holds the Green-only and Blue-only stages for a full
// second (1000 milliseconds), and holds the transitional patterns 
// for just 1/10 of a second (100 milliseconds)
//
int PatternTime[] = {  1000,  100, 100, 100,    100, 100, 100, 100,  
                       1000,  100, 100, 100,    100, 100, 100, 100
                       };

// You can control the pitch and duration of a TONE that accompanies each pattern
// See user manual for list of pitches, or experiment to find one that works for you
// Tones in the range 500 to 4000 work best on the Piezo Buzzer
// Set Duration to zero to create no tone with a pattern
// This sketch sets all of the Duration values to zero, i.e., no sound

int PatternTonePitch[]   =  { 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0  };
int PatternToneDuration[] = { 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0  };

// ASSIGN A COLOR TO EACH NEOPIXEL IN EACH PATTERN
// The array PatternColor should have the same number of elements as PatternData.
// Each entry in PatternData tells which color row from the NeoPixelColor
// table below to use in illuminating the NeoPixel that is listed in the corresponding
// entry in PatternData. Or, you can choose to let the computer randomly assign
// colors from the NeoPixelColor table, using the Random option below.

// In LargeM, the first 40 NeoPixel numbers in PatternData represent the first pattern, which
// is the letter M entirely in Green (row 16 of the color table), and the second 40
// NeoPixel numbers in PatternData represent the ninth patter, which is the letter M entirely
// in Blue (row 17 of the color table).  This is accomplished by having the first 40
// elements of the PatternColor variable (below) set to Green and the next 40 elements
// of the PatternColor variable set to Blue.  The other patterns defined by PatternStart and 
// PatternStop then create a gradual transition between the Green-only and Blue-only patterns
// by specifying a shifting mix of the Green and Blue colors below.
//
char PatternColor[]= {
     16,16,16,16,   16,16,16,16,
     16,16,16,16,   16,16,16,16,
     16,16,16,16,   16,16,16,16,
     16,16,16,16,   16,16,16,16,
     16,16,16,16,   16,16,16,16,

     17,17,17,17,   17,17,17,17,
     17,17,17,17,   17,17,17,17,
     17,17,17,17,   17,17,17,17,
     17,17,17,17,   17,17,17,17,
     17,17,17,17,   17,17,17,17
  };

// If you wish, choose the color for each step from the color table (below) randomly
// instead of using colors that you have specified for each NeoPixel
// that you are lighting up
#define RandomColor false

// Once you are done setting everything up, you can set this to false
// to remove the Mix Color menu from the dashboard to make it easier
// to find what you really want. You can always get the menu back later
// by setting it back to true.
#define IncludeMenus true

// CREATE A TABLE THAT CREATES THE COLORS YOU WILL USE
// Create a table of colors that is used to specify the palette of colors (and
// brightness) of NeoPixel lit. The code is written to 
// allow you to select a specific color for each NeoPixel that you
// utilize in a pattern -- but in order for this to work, any colors that 
// you decide to use must be included in the table below (you also can
// leave in the table colors that you ultimately decide not to use). There need be no
// relation between the number of different NeoPixels used in your pattern
// and the number of different colors that you utilize -- several NeoPixels
// can use the same color. Alternatively, you can allow the computer to choose a
// color randomly from the color table below for each NeoPixel,each time it lights.
// 
// Use the color mixing tool on the Gizmo's dashboard to experiment with different
// colors, and to find the recipe for the colors you like.  Write down the 3-number recipe
// and a description of the color (for yourself) on a piece of paper.  You will need
// to enter each of these recipes into the NeoPixelColor table below, and you will want
// to remember what color is created by each different recipe that you select.
//
// The red, green, and blue components of the first eleven colors in this table sum to
// 32 in each row, and if four NeoPixels are on at a time using colors from these rows, the
// total is 128, and the power used is about 0.05W.  
//
// If using more than about 10 NeoPixels, consider scaling down
// the numerical values a bit.  The Heart pattern turns on up to 24 NeoPixels at a time,
// using rows 12, 13, and 14 of the NeoPixelColor table, so we have reduced the brightness
// of the colors in those three rows of the table below.
//
// In any case, the sum of all the colors on all the lights that will be on at one time 
// must not exceed 1275.  In fact, much lower totals produce very visible displays.
//
// The rows of this table are numbered 0,1,2,etc.  Add more rows if you need more
// or different colors.

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
  {  24,   0,  8},
  {  20,   0,  0},   
  {  12,   0,  4},  
  {   8 ,  8,  8}, 
  {  24,   8,  0},
  {   0,   4,  0},
  {   0,   0,  4}
};
// Note for Large M:  Row 16 is Green, Row 17 is Blue.  The small brightness values in these two rows
// were selected because this pattern lights up 40 NeoPixels at a time, which could overwhelm the Gizmo
// if the individual NeoPixels were not tuned to a lesser brighness settings.
//
// The other rows of the table are not used in this Large M sketch, but are left in place for 
// potential use in a different modification of this sketch.

// ==========================================================================

// *****************
// *               *
// *  Source Code  *
// *               *
// *****************

// Number of NeoPixels in your assembly.
// Note that this sketch is intended for use with NeoPixel sheet,
// which has 64 NeoPixels.
#define NumberOfPixels 64

// Create a GizmoGardenPixels object called lights that can be used to
// control a NeoPixel assembly.
GizmoGardenPixels lights(NumberOfPixels, DigitalPinNumber);

// Create and initialize a variable that will cycle through the number of 
// different patterns that you want to light up.  Set the initial value
// so that on the first cycle the variable will cycle to the value of "0",
// which is the index number for the first pattern defined in PatternStart
// and PatternStop
int PatternCount = NumOfPatterns - 1 ;

// Calculate the number of colors in the color table, so the Gizmo will
// know how many colors are available to select from.
#define NumberOfColors (sizeof(colors) / sizeof(NeoPixelColor))

// Create a task called PatternTask to run the light show, simultaneously
// with all other tasks that may be running. This code tells what happens
// when it's PatternTask's turn to run. For each turn to run that PatternTask
// gets, it lights up the next pattern.  The speed of change is determined
// by how much time elpases between each turn, which you can change.
MakeGizmoGardenTaskWithStop(PatternTask)
{

// Turn off all NeoPixels
  lights.clear();
  lights.show();
  
// Step to the next pattern; cycle through the specified number of patterns
  PatternCount = ( PatternCount + 1 ) % NumOfPatterns;

// Work our way through the different NeoPixels that are specified in this pattern
int PatternCell = 0;
  for ( PatternCell = PatternStart[PatternCount] ; PatternCell<=PatternStop[PatternCount] ; PatternCell=PatternCell+1  ) 
  {

// Get the NeoPixel number of the next NeoPixel in the pattern
 int lightNumber = PatternData[PatternCell];

// The color of the NeoPixels can be picked randomly, or determined by you
// by creating and using an array of values (PatternColor) that references the row of the 
// NeoPixelColor table in Settings.
// The variable colorIndex is the color selected for the NeoPixel we just selected above
int colorIndex = 0;
    if (RandomColor)
    colorIndex = random(NumberOfColors);
  else
    colorIndex = PatternColor[PatternCell];

// Now that we have a new NeoPixel and a new color index, put them together to 
// tell the Gizmo that this NeoPixel will be turned on to this color 
  lights.setPixelColor(lightNumber, colors[colorIndex]);

//return to top of the loop to do the next NeoPixel in this pattern
   }

// The lights don't actually change until show is called.
// Turn on all the NeoPixels that we have defined in this pattern, each in its specified color
  lights.show();
  
// Now all the lights for this pattern have been turned on

// Play the designated tone for this pattern
// The pitch is adjusted to increase as the heartbeat accelerates
      GizmoGardenTone( int( PatternTonePitch[PatternCount]  ),  PatternToneDuration[PatternCount]  );
      
// Ask for another turn to run, specify duration until next turn
  int StepTime = ( PatternTime[PatternCount] );
  callMe(StepTime);
}

// When PatternTask is stopped, turn off any NeoPixel that may be lit.
CustomStop(PatternTask)
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
      PatternTask.start();
      break;
      
    case DriveStop:
      PatternTask.stop();
      break;
  }
}

GizmoGardenEventRegistry event(eventFunction);

// Make the color mixer menu
#if IncludeMenus
GizmoGardenColorMixer mixer(lights, &PatternTask);
#endif

};

#undef DigitalPinNumber
#undef NumOfPatterns
#undef RandomColor
#undef IncludeMenus
#undef NumOfColors
#undef NumberOfPixels

