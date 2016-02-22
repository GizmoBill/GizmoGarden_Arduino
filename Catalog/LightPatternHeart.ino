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
// * LightPatternHeart *
// *                   *
// *********************

// This code creates a beating heart.  It is an example of code that could
// be used to create a repeating set of patterns on the NeoPixel sheet. (And, with
// a few small changes, the basic LightPattern code easily can be used
// to create a single stationary pattern.)  
// 
// Within LightPattern, the pattern (which can have different numbers of phases within it)is
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
namespace Heart {

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
// (Note: The entries in PatternData below actually define four different hearts,
// but the entries in the PatternStart and PatternStop arrays only put two of
// these hearts to work.  It would be easy to include the other two hearts in
// your pattern by adding additional values to PatternStart and PatternStop.
// Hint: Try using the paperworksheet to trace out the complete patterns defined below.
// Can you see the four hearts?  Which two are being used by current code?  
// Can you define the places where the pattern needs to be interrupted?)
PROGSPACE char PatternData_[]= {
   3, 13, 17, 31, 32, 47, 49, 50, 44,
  52, 53, 41, 38, 25, 21, 11,
  
  12, 18, 30, 33, 45, 35, 43, 37, 26, 20,
  
   7,  6,  5,  4,  3,  2,  1, 15, 16, 31, 33, 34, 35,
  44, 51, 59, 58, 57, 55, 40, 39, 24, 23,  8,
  
   9, 10, 11, 12, 18, 28, 27,
  36, 42, 38, 25, 22,  9
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
#define NumOfPatterns  2

// The variables PatternStart and PatternStop tell the Gizmo how to find
// the start and finish of each pattern from the list of numbers that you
// entered into the PatternData array, above.
// The smallest and largest entries in PatternStart and PatternStop should not 
// fall outside the limits of the index number of the first and last entries in PatternData.
// The first entry in PatternData is indexed at (zero); the last entry in
// PatternData is indexed at (the number of entries in PatternData, minus 1).
//  
// Creative Note:  Patterns can overlap in the array.  For example, the last
// ten NeoPixels listed in the first pattern can also serve as the first ten
// NeoPixels in the second pattern.

int PatternStart[]={  50, 26 };
int PatternStop[] ={  62, 49 };

// Now, define how long you want each pattern to display before moving on to
// the next pattern and continuing the cycle.
// PatternTime entries are in milliseconds

int PatternTime[] ={  600,  400  };

// You can control the pitch and duration of a TONE that accompanies each pattern
// See user manual for list of pitches, or experiment to find one that works for you
// Tones in the range 500 to 4000 work best on the Piezo Buzzer
// Set Duration to zero to create no tone with a pattern

int PatternTonePitch[] = {  262,  294  };
int PatternToneDuration[] = {  30,  150  };

// The array PatternTime (above) defined the length of time that each pattern
// is displayed before moving to the next pattern.
// This sketch includes a special feature that allows you to VARY the rates at which
// the patterns change as your Gizmo cycles through the patterns, 
// making the pace of change gradually accelerate and decelerate.  
// As the pace increases, the tone pitches move up the scale also.
//
// The variable AccIncrBase tells the Gizmo how much to accelerate (or decelerate) with each
// cycle of patterns. The prior pace is multiplied (or divided) by the value of AccIncrBase 
// at the beginning of each cycle of patterns.  With a value of AccIncrBase = 1.04427, for example,
// the pace will double every 16 cycles through the patterns.  Set this variable to 1.0 if you
// prefer to disable the accelerating function.
//
// The variable AccelertorMax tells the Gizmo how far to increase the pace before allowing
// the pace to return to its starting rate.  For example, AcceleratorMax = 4.00 allows the speed
// of pattern change to get up to 4 times as fast as the original PatternTime values, at which point
// the pace of change gradually returns to the original PatternTime values.

float AccIncrBase = 1.04427  ;
float AcceleratorMax = 4.00  ;

// ASSIGN A COLOR TO EACH NEOPIXEL IN EACH PATTERN
// The array PatternColor should have the same number of elements as PatternData.
// Each entry in PatternData tells which color row from the NeoPixelColor
// table below to use in illuminating the NeoPixel that is listed in the corresponding
// entry in PatternData. Or, you can choose to let the computer randomly assign
// colors from the NeoPixelColor table, using the Random option below.

PROGSPACE char PatternColor_[]= {
  12, 12, 12, 12, 12, 12, 12, 12, 12,
  12, 12, 12, 12, 12, 12, 12,
  
  14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
  
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,

  12, 12, 12, 12, 12, 12, 12,
  12, 12, 12, 12, 12, 12
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
// In any case, the sum of all the colors on
// all the lights that will be on at one time must not exceed 1275.
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
};
// Note for Heart colors: Row 12 is Red, Row 13 is Pink, Row 14 is White

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

// We need to define a couple of variables that are used in the routine that
// accelerates and decelerates the heartbeat.
float Accelerator = 1.00 / AccIncrBase;
float AccIncrement = AccIncrBase;
// The variable 'AccWait' is used to create a waiting period before the hearbeat
// starts to accelerate or deccelerate
int AccWait =  5  ;


// Create a task called PatternTask to run the light show, simultaneously
// with all other tasks that may be running. This code tells what happens
// when it's PatternTask's turn to run. For each turn to run that PatternTask
// gets, it lights up the next pattern.  The speed of change is determined
// by how much time elpases between each turn, which you can change.
MakeGizmoGardenTaskWithStop(PatternTask)
{
  ProgChars PatternData = PatternData_;
  ProgChars PatternColor = PatternColor_;
  
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
  
// Now all the lights for this pattern are turned on

// Special lines of code to increase and decrease the heartbeat before each complete
// cycle of patterns, and to instruct when to switch between increasing and decreasing.
// The counter AccWait creates a delay number of cycles of a stable (slow or fast) heartbeat 
// before the acceleration or deccelaration begins.  
if ( PatternCount == 0 )
   {
   if ( AccWait  <  10 )
         AccWait  =  AccWait + 1  ;
   else {
         Accelerator  =  Accelerator  *  AccIncrement ;
 // now plan ahead for the pace calculations that will need to be done in
 // the next pass
         if ( Accelerator  >  AcceleratorMax )  {
              AccIncrement  =  1.0 / AccIncrBase ;
              AccWait  =  0  ;
              }
         if (Accelerator < 1.00) {
              AccIncrement =  AccIncrBase ;
              AccWait  =  5  ;
              }
         }
   }
   
// Play the designated tone for this pattern
// The pitch is adjusted to increase as the heartbeat accelerates
      GizmoGardenTone( int( Accelerator  *  PatternTonePitch[PatternCount]  ),  PatternToneDuration[PatternCount]  );
      
// Ask for another turn to run, specify duration until next turn
// The duration is adjusted to decrease (less time between each heartbeat)
// as the Accelerator variable increases.
  int StepTime = ( PatternTime[PatternCount]  /  Accelerator  );
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

