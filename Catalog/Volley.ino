/***************************************************************************
Copyright (c) 2016 Bill Silver, Member of Gizmo Garden LLC. This source code
is distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The license
can be found at <http://www.gnu.org/licenses/>. There is no express or
implied warranty, including merchantability or fitness for a particular
purpose.
***************************************************************************/

// ************
// *          *
// *  Volley  *
// *          *
// ************

// This is the code used by the Wimbledon tennis gizmo to create a volley
// game, where "rackets" volley a "ball" back and forth. The ball is
// made with NeoPixels, the rackets are swung with positioning motors, and
// the sound of a racket hitting the ball is made with the piezo buzzer.
// This code can be used as an example to create similar volley games or
// other electro-artistic options that choreograph light, motion, and sound.
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
#define DigitalPinNumber_Lights 6

// 2) Number of NeoPixels in your assembly.
#define NumberOfPixels 16

// 3) The ball is one color when it movers from racket A to racket B, and a
//    different color when it moves from racket B to racket A. Use the
//    colors already provided, or create colors you like by entering
//    values for a red, green, and blue mixture. Use can use the Mix Color
//    menu that is described in ComputerColors.pdf to find mixtures you
//    like.
//                 red, green, blue
#define AtoB_Color  64,    64,    0     // yellow
#define BtoA_Color   0,    64,   64     // cyan

// 4) When a racket swings, it can swing hard, soft, or normal, which makes
//    the ball move fast, slow, or medium. The ball moves is steps, one
//    NeoPixel for each step. The overall speed of the volley is set by the
//    amount of time for one step when the ball is moving fast. Set the
//    time you want here, in milliseconds. Smaller times make faster volleys,
//    but don't go below 20.
#define FastStepTime 50

// 5) Each racket makes a random choice to decide whether to swing fast, slow,
//    or medium. Here you can set the probability of choosing fast or slow as
//    a percentage. The probability of choosing medium is whatever percentage
//    is left over. Making either number 0 makes that choice impossible.
#define PercentProbability_Fast 20
#define PercentProbability_Slow 20

// 6) The sound of a racket hitting a ball is controlled here. You can
//    choose the pitch of the sound for a normal swing, and how much to
//    increase or decrease the pitch when the swing is hard or soft. You
//    can also choose the duration of the sound in milliseconds.
#define NormalSwingPitch                3600
#define HardOrSoftSwingPitchDifference  600
#define SwingSoundDuration              250

// 7) Before moving on to set up the racket motors, now is a good time to
//    verify and upload the code to see if it is working right.

// 8) Digital pin numbers to which the two racket motors are connected.
#define DigitalPinNumber_RacketA 10
#define DigitalPinNumber_RacketB 11

// 9) Eack racket has two positions, relax and swing. When the ball hits
//    a racket it moves to its swing position. After a few steps of the
//    ball it moves back to its relax position. Upload the code and scroll
//    to the Volley menu to adjust the swing and relax positions of both
//    rackets. The menu shows the positions for relax and swing for each
//    racket. Click the select button to cycle around these four positions.
//    Click the up and down buttons to adjust; the racket that you are
//    adjusting will move to the positions you set so you can see them.
//    The volley stops when you scoll to the Volley menu to make your
//    adjustments. When you scroll away the volley starts again and all of
//    the positions are saved even if you turn the gizmo off.

// 10) Set the number of ball steps you want each racket to wait after
//     hitting the ball before moving back to the relax position. These
//     numbers must be more than 1 and less than half of NumberOfPixels.
#define RacketA_SwingBackSteps  3
#define RacketB_SwingBackSteps  3

// 11) Once you are done setting everything up, you can set this to false
//     to remove the Mix Color and Volley menus from the dashboard to
//     make it easier to find what you really want. You can always get
//     the menus back later by setting it back to true.
#define IncludeMenus true

// ==========================================================================

// *****************
// *               *
// *  Source Code  *
// *               *
// *****************

namespace Volley {

// Create a GizmoGardenPixels object called lights that can be used to
// control a NeoPixel assembly.
GizmoGardenPixels lights(NumberOfPixels, DigitalPinNumber_Lights);

// Create a GizmoGardenPositioningMotor object for each of two positioning
// motors that are used to swing the rackets.
GizmoGardenPositioningMotor racketA(DigitalPinNumber_RacketA);
GizmoGardenPositioningMotor racketB(DigitalPinNumber_RacketB);

// Create an integer variable that holds the number of the NeoPixel
// currently lit.
int lightNumber = 1;

// Create an integer variable that holds the length of time between
// motions of the ball. The shorter the time, the faster the ball moves.
// The code uses the values 1 (fast), 2 (medium), and 3 (slow).
int stepTime = 2;

// Create an integer variable that holds the current direction of
// motion: 1 is from racket A to racket B, and -1 is from racket B to
// racket A.
int direction = -1;

// Create a NeoPixelColor object to hold the current color (and brightness)
// of the ball. The code uses two colors, one for each direction of motion.
NeoPixelColor lightColor;

char racketPositions[] = { 0, -30, 0, -30};

// Create a function that sets a new ball speed when it hits a racket,
// and beeps a tone whose pitch depends on the new speed. The ball speed
// is set using a random number so that the game is a little unpredictable.
// A function is created so it can be called in two places, one for each
// racket.
void setBallSpeed()
{
  // Create an integer variable r and set it to a random number between
  // 0 and 99. Since the variable is created inside this function, it
  // is called a "local" variable. It is invisible to any code outside
  // this function, so you don't have to worry about other code using the
  // same name, and it only uses computer memory when this function is
  // running.
  int r = random(100);

  // Probability to set the speed to 3 (slow)
  if (r < PercentProbability_Slow)
    stepTime = 3;
    
  // Probability to set the speed to 1 (fast)
  else if (r < PercentProbability_Slow + PercentProbability_Fast)
    stepTime = 1;
    
  // Probability to set the speed to 2 (medium)
  else
    stepTime = 2;
  
  // Make the sound of a racket hitting the ball. Higher pitches correspond
  // to faster speeds. 
  GizmoGardenTone(NormalSwingPitch + HardOrSoftSwingPitchDifference * (2 - stepTime),
                  SwingSoundDuration);
}

// Create a task called VolleyTask to run the volley game, simultaneously
// with all other tasks that may be running. This code tells what happens
// when it's VolleyTask's turn to run. For ech turn to run that VolleyTask
// gets, it moves the ball one position. At certain positions, it also
// swings a racket forward or back. The speed of the ball is determined by
// how much time elpases between each turn.
MakeGizmoGardenTaskWithStop(VolleyTask)
{
  // Turn off the NeoPixel that is currently lit.
  lights.setPixelColor(lightNumber, 0);

  // Update the number of the lit NeoPixel in the direction the ball is
  // moving.
  lightNumber += direction;

  // Use a switch statement to do special things for certain values of
  // lightNumber (i.e. the ball position).
  switch (lightNumber)
  {
    // The ball is hitting racket A.
    case 0:
      // Change direction
      direction = 1;

      // Set the color to use when the ball moves from racket A to racket B.
      lightColor = NeoPixelColor(AtoB_Color);

      // Choose a new ball speed
      setBallSpeed();

      // Move racket A to swing position
      racketA.setPosition(racketPositions[1]);
      break;

    // The ball is hitting racket B.
    case NumberOfPixels - 1:
      // Change direction
      direction = -1;
      
      // Set the color to use when the ball moves from racket B to racket A.
      lightColor = NeoPixelColor(BtoA_Color);

      // Choose a new ball speed
      setBallSpeed();

      // Move racket B to swing position
      racketB.setPosition(racketPositions[3]);
      break;

    // Some number of steps after hitting the ball, racket A starts to
    // swing back to its relaxed position.
    case RacketA_SwingBackSteps:
      racketA.setPosition(racketPositions[0]);
      break;

    // Some number of steps after hitting the ball, racket B starts to
    // swing back to its relaxed position.
    case NumberOfPixels - RacketB_SwingBackSteps - 1:
      racketB.setPosition(racketPositions[2]);
      break;
  }

  // Now that we have a new NeoPixel and maybe a new color, light it up.
  lights.setPixelColor(lightNumber, lightColor);
  
  // The lights don't actually change until show is called.
  lights.show();

  // Ask for another turn to run in FastStepTime times stepTime in
  // milliseconds. This is how stepTime controls the speed of the ball.
  callMe(FastStepTime * stepTime);
}

// When VolleyTask is stopped, turn off any NeoPixel that may be lit.
CustomStop(VolleyTask)
{
  lights.clear();
  lights.show();
}

#define ValidCode 163
#define EepromAddress 200

void eventFunction(byte eventCode)
{
  switch (eventCode)
  {
    case GizmoSetup:
      lights.begin();
      if (EEPROM.read(EepromAddress) == ValidCode)
        for (int i = 0; i < 4; ++i)
          racketPositions[i] = EEPROM.read(EepromAddress + 1 + i);
      
    case DriveStart:
      VolleyTask.start();
      break;
      
    case DriveStop:
      VolleyTask.stop();
      break;
  }
}

GizmoGardenEventRegistry event(eventFunction);

#if IncludeMenus
// Make the color mixer menu
GizmoGardenColorMixer mixer(lights, &VolleyTask);

byte menuMode = 0;

MakeGizmoGardenMenuItem(Volley)
{
  switch (event)
  {
    case Enter:
      printName();
      VolleyTask.stop();
      break;

    case Leave:
      EEPROM.update(EepromAddress, ValidCode);
      for (int i = 0; i < 4; ++i)
        EEPROM.update(EepromAddress + 1 + i, racketPositions[i]);
        
      VolleyTask.start();
      break;

    case Select:
      menuMode = (menuMode + 1) % 4;
     
    case ChangeAndShow:
      {
        GizmoGardenPositioningMotor& menuMotor =  menuMode < 2 ? racketA : racketB;
        menuMotor.setPosition(racketPositions[menuMode] + 3 * direction);
        racketPositions[menuMode] = menuMotor.getPosition();
      }

      lcd.print(menuMode < 2 ? F("A ") : F("B "));
      lcd.print((menuMode & 1) == 0 ? F("Relax") : F("Swing"));
      ggPrint(lcd, racketPositions[menuMode], 5);
      break;
  }
}
#endif

};

#undef DigitalPinNumber_Lights
#undef NumberOfPixels
#undef AtoB_Color
#undef BtoA_Color
#undef FastStepTime
#undef PercentProbability_Fast
#undef PercentProbability_Slow
#undef NormalSwingPitch
#undef HardOrSoftSwingPitchDifference
#undef SwingSoundDuration
#undef DigitalPinNumber_RacketA
#undef DigitalPinNumber_RacketB
#undef RacketA_SwingBackSteps
#undef RacketB_SwingBackSteps
#undef ValidCode
#undef EepromAddress
#undef IncludeMenus

