/***************************************************************************
Copyright (c) 2016 Bill Silver, Member of Gizmo Garden LLC. This source code
is distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The license
can be found at <http://www.gnu.org/licenses/>. There is no express or
implied warranty, including merchantability or fitness for a particular
purpose.
***************************************************************************/

// ******************
// *                *
// *  Waving Motor  *
// *                *
// ******************

// This code can be used to wave something back and forth using a
// positioning motor. You can choose the speed at which the motor waves,
// and the range of angles each wave covers. You can set the forward and
// back motions to different speeds to achieve a desired effect. Note:
// all angles are in degrees.

// The motor moves back and forth in steps. Each step is an angle of
// movement (degrees) you set, and taking an amount of time (milliseconds)
// you set. The motor moves faster if you decrease the time or increase
// the angle of movement. The motion is more smooth if the angle of
// movement is small, for example 1 degree. You shouldn't set the time
// smaller than 20 milliseconds, so for fast motion you may need to
// increase the angle of movement.
//
// You can set the angle of movement and the time to different values
// for the forward and back motions. If you set the angle of motion
// to 360 (or any value at least as high as the total angle range), for
// forward or back or both, the motor will make only one step in that
// direction as fast as it can, and then wait for the time you set for
// that direction before waving in the other direction.
//
// You can add a second motor synchronized to the first, covering the
// same angle range and same angles of movement and times. This second
// motor can move parallel to the first, or opposite from the first.
// You can add additional unsynchronized waving motors or synchronized
// pairs not synchronized to this motor or pair, by following the
// instructions in the Settings section.
//
// With this code you get a dashboard menu called ZeroOffsetA that allows
// you to make fine adjustments in the zero position of the motor. After
// adjusting with the menu, write down the final zero position and enter
// in into the ZeroOffsetA setting below. If you have a second
// synchronized motor, you get a second menu called ZeroOffsetB to adjust
// that one. 

// **************
// *            *
// *  Settings  *
// *            *
// **************

// 1) Digital pin number to which the motor control signal is wired. If
//    you are waving two synchronized motors, this is the first one.
//    After the code is uploaded, the motor should wave. If you have
//    two motors, note which one is "A".
#define DigitalPinNumberA 11   

  // 2) Set true to use a second, synchronized motor
#define SynchronizedMotor false

  // 3) Digital pin number to which the second motor control signal is
  //    wired. After code is uploaded, the second motor should wave.
#define DigitalPinNumberB 10    

  // 4) Set true to have the second motor move opposite the first instead
  //    of parallel.
#define OppositeMotion false

// 5) Mount your motors approximately where they will sit on your float.
//    Scroll dashboard menu to ZeroOffsetA. Motor will stop. Pull off
//    the arm and replace it as close as you can to the center of the
//    waving motion that you want. Use the up and down buttons to fine
//    tune the offset. When you like the position, read the number off
//    of the display and plug it in here. After code is uploaded, when
//    when you scroll menu to ZeroOffsetA the motor arm should go to
//    "perfect" zero.
#define ZeroOffsetA 0

  // 6) Repeat above but for ZeroOffsetB.
#define ZeroOffsetB 0

// 7) Low and high ends of the wave motion (degrees). The low end must
//    be less than the high end, both must be in the range -90 to 90,
//    and the angles do not have to be equal.
#define LowRangeAngle   -45 
#define HighRangeAngle   45

// 8) Time between forwards or backwards steps (milliseconds). A lower
//    step time will make the motor move faster, but don't go below 20.
//    If you are using settings #9 (below) to make large steps, the
//    instructions for #9 will tell you how to set these times.
#define ForwardStepTime  20
#define BackwardStepTime 60

// 9) Amount to change position for each step forwards or backwards
//    (degrees). Leave these at 1 unless your motor is not moving fast
//    enough at the minimum step time of 20. A bigger step angle gives
//    faster but jerkier motion.
//    To move the entire range in one step as fast as the motor can,
//    set one or both of these to 360 and adjust the corresponding times
//    (ForwardStepTime if you set ForwardStepAngle to 360, BackwardStepTime
//    if you set BackwardStepAngle to 360). You should make the time at
//    least as long as it takes the motor to move to the new position,
//    plus any extra pause you want after it gets there. Play around with
//    the time until the waving looks right.
#define ForwardStepAngle  1
#define BackwardStepAngle 1

// 10) Once you are done setting everything up, you can set this to false
//     to remove the ZeroOffset menus from the dashboard to make it easier to
//     find what you really want. You can always get the menus back later by
//     setting it back to true.
#define IncludeMenus true

// To add additional unsynchronized waving motors, or synchronized pairs,
// that are not synchronized to this motor or pair:
//    1) Using the menu on the right side of the tabs (click the small,
//       downward-pointing arrow), choose Rename.
//    2) In the box that pops up below, change the name, for example by
//       adding a "2" on the end of the name.
//    3) Click OK. You will see the new name in the tab.
//    4) Change the name in the line of code just below that reads
//       "namespace WavingMotor", for example by adding a "2" on the end of
//       WavingMotor, which is the name.
//    5) Cange the menu title(s) in the line(s) of code just below that
//       start with "MakeGizmoGardenText". You'll change one or two
//       lines depending on whether this new tab is controlling one or
//       two motors. The titles are emclosed in quotes, and you can just
//       add a "2" on the end of the name.
//    6) Add the original file back in from the Code Catalog by selecting
//       Add File from the Sketch menu, browsig to the Catalog folder (you
//       will probably already be there), and choosing WavingMotor.
//    7) You will now have two tabs, Wavingotor that you just added and
//       WavingMotor2 that you already had. Change the settings in the newly
//       added file as desired.

namespace WavingMotor
{

#if IncludeMenus
MakeGizmoGardenText(menuTitleA, "ZeroOffsetA");
#if SynchronizedMotor
MakeGizmoGardenText(menuTitleB, "ZeroOffsetB");
#endif
#endif

// ==========================================================================

// *****************
// *               *
// *  Source Code  *
// *               *
// *****************


// Create a GizmoGardenPositioningMotor object called waveMotorA that can be
// used to control a positioning motor.
GizmoGardenPositioningMotor waveMotorA(DigitalPinNumberA, false,
                                       ZeroOffsetA);

// If using a synchronized motor, create a second GizmoGardenPositioningMotor
// object called waveMotorB
#if SynchronizedMotor
GizmoGardenPositioningMotor waveMotorB(DigitalPinNumberB, OppositeMotion,
                                       ZeroOffsetB);
#endif

// Create an integer variable that holds the current angle of the motor.
int waveAngle = LowRangeAngle;

// Create a true/false variable that says whether the motor is moving
// forward (true) or back (false). The type of this variable is "bool",
// named after George Boole who invented a type of algebra that uses
// true/false values instead of numbers.
bool movingForward = true;

// Create a task called WaveTask to wave the motor, simultaneously with all
// other tasks that may be running. This code tells what happens when it's
// WaveTask's turn to run. For ech turn to run that WaveTask gets, it moves
// the motor one waveStep. The speed of the motor is determined by how much
// time elpases between each turn.
MakeGizmoGardenTask(WaveTask)
{
  if (movingForward)
  {
    waveAngle += ForwardStepAngle;
    if (waveAngle >= HighRangeAngle)
    {
      waveAngle = HighRangeAngle;
      movingForward = false;
    }
    callMe(ForwardStepTime);
  }
  else
  {
    waveAngle -= BackwardStepAngle;
    if (waveAngle <= LowRangeAngle)
    {
      waveAngle = LowRangeAngle;
      movingForward = true;
    }
    callMe(BackwardStepTime);
  }
  
  // Set the motor's position to the current waveAngle
  waveMotorA.setPosition(waveAngle);
#if SynchronizedMotor
  waveMotorB.setPosition(waveAngle);
#endif
}

void eventFunction(byte eventCode)
{
  switch (eventCode)
  {
    case GizmoSetup:
    case DriveStart:
      WaveTask.start();
      break;
      
    case DriveStop:
      WaveTask.stop();
      break;
  }
}

GizmoGardenEventRegistry event(eventFunction);

#if IncludeMenus
// Create a dashboard menu item to change the offset of the motor.
GizmoGardenMotorOffsetMenu menu1(waveMotorA, menuTitleA, &WaveTask);

#if SynchronizedMotor
GizmoGardenMotorOffsetMenu menu2(waveMotorB, menuTitleB, &WaveTask);
#endif

#endif

};

#undef DigitalPinNumberA
#undef ZeroOffsetA
#undef LowAngle
#undef HighAngle
#undef ForwardStepAngle
#undef BackwardStepAngle
#undef ForwardStepTime
#undef BackwardStepTime

#undef SynchronizedMotor
#undef DigitalPinNumberB
#undef OppositeMotion
#undef ZeroOffsetB
#undef IncludeMenus

