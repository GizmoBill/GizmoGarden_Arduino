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
// *  Rotating Motor  *
// *                  *
// ********************

// This code is used to control a rotating motor, including the motor used
// for a rotating turntable. You can set the initial speed of the motor when
// the gizmo starts up, and a dashboard menu is provided to allow you
// to change the speed.

// **************
// *            *
// *  Settings  *
// *            *
// **************

// 1) Digital pin number to which the motor control signal is wired
#define DigitalPinNumber 11

// 2) Scroll the dashboard menu to RotatingMotor and use the up and down
//    buttons to set the rotation to the speed and direction you want.
//    Positive speeds go in one direction and negative speeds go in the
//    other direction. The range is -100 to 100. Enter the speed you want
//    here.
#define MotorSpeed 40

// 3) Once you are done setting everything up, you can set this to false
//    to remove the RotatingMotor menu from the dashboard to make it easier
//    to find what you really want. You can always get the menu back later
//    by setting it back to true.
#define IncludeMenus true

// To add additional rotating motors:
//    1) Using the menu on the right side of the tabs (click the small,
//       downward-pointing arrow), choose Rename.
//    2) In the box that pops up below, change the name, for example by
//       adding a "2" on the end of the name.
//    3) Click OK. You will see the new name in the tab.
//    4) Change the name in the line of code just below that reads
//       "namespace RotatingMotor", for example by adding a "2" on the end of
//       RotatingMotor, which is the name.
//    5) Cange the menu title in the line of code just below that starts with
//       "MakeGizmoGardenText", for example by adding a "2" on the end of
//       RotateSpeed, which is the name.
//    6) Add the original file back in from the Code Catalog by selecting
//       Add File from the Sketch menu, browsig to the Catalog folder (you
//       will probably already be there), and choosing RotatingMotor.
//    7) You will now have two tabs, RotateSpeed that you just added and
//       RotateSpeed2 that you already had. Change the settings in the newly
//       added file as desired.
namespace RotatingMotor
{

MakeGizmoGardenText(menuTitle, "RotateSpeed")

// ==========================================================================

// *****************
// *               *
// *  Source Code  *
// *               *
// *****************

// Create a GizmoGardenRotatingMotor object called rotatingMotor that is
// used to control the speed of the motor.
GizmoGardenRotatingMotor rotatingMotor(DigitalPinNumber, 0);

// Create a function that can be called by other code to turn the motor
// on or off.
inline void setRotateOn(bool on)
{
  if (on)
    rotatingMotor.setFullSpeed(100);
  else
    rotatingMotor.setFullSpeed(0);
}

void eventFunction(byte eventCode)
{
  switch (eventCode)
  {
    case GizmoSetup:
      rotatingMotor.setSpeed(MotorSpeed);
      
    case DriveStart:
      setRotateOn(true);
      break;
      
    case DriveStop:
      setRotateOn(false);
      break;
  }
}

GizmoGardenEventRegistry event(eventFunction);

#if IncludeMenus
// Create a dashboard menu item to change the speed of the motor. If the
// motor is off when you click to this menu item it is turned on, and then
// turned off again when you click away.
GizmoGardenRotatingMotorMenu menu(rotatingMotor, menuTitle);
#endif

};

#undef DigitalPinNumber
#undef MotorSpeed
#undef IncludeMenus

