/***************************************************************************
Copyright (c) 2016 Bill Silver, Member of Gizmo Garden LLC. This source code
is distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The license
can be found at <http://www.gnu.org/licenses/>. There is no express or
implied warranty, including merchantability or fitness for a particular
purpose.
***************************************************************************/

// The leading gizmo (leadMode = true) limits its speed to 80% so the followers
// can catch up and keep a fixed distance. Followers (leadMode = false) can go
// as fast as they need to.
bool leadMode = false;

// Parameters to control following the vehicle in front
int   stopDistance    = 475;    // Proximity sensor reading at which we should stop   
float springStiffness = 0.36;   // Follower's spring stiffness

// ***************
// *             *
// *  Gas Pedal  *
// *             *
// ***************

void gasPedal()
{
}

// *********************************************
// *                                           *
// *  Experiment with Spring to Control Speed  *
// *                                           *
// *********************************************

MakeGizmoGardenTaskWithStartStop(SpringTask)
{
  gasPedal();
  callMe(50);
}

CustomStart(SpringTask)
{
  proximityMonitor.reset();
  leftWheel.setSpeed(100);
  rightWheel.setSpeed(100);
}

CustomStop(SpringTask)
{
  leftWheel .setSpeed(0);
  rightWheel.setSpeed(0);
}

// *************************
// *                       *
// *  Speed Control Menus  *
// *                       *
// *************************

MakeGizmoGardenMenuItem(StopDistance)
{
  switch(event)
  {
    case Enter:
      printName();
      break;
      
    case ChangeAndShow:
      stopDistance += 25 * direction;
      stopDistance = constrain(stopDistance, 100, 750);
      ggPrint(lcd, stopDistance, 4);
      break;
  }
}

MakeGizmoGardenMenuItem(SpringStiffness)
{
  switch(event)
  {
    case Enter:
      printName();
      break;
      
    case ChangeAndShow:
      springStiffness += 0.02 * direction;
      springStiffness = constrain(springStiffness, 0.10, 2.0);
      ggPrint(lcd, springStiffness, 4, 2);
      break;
  }
}

MakeGizmoGardenMenuItem(LeaderOrFollower)
{
  switch (event)
  {
    case ChangeAndShow:
      if (direction != 0)
        leadMode = !leadMode;
      lcd.setCursor(0, 0);
      lcd.print(leadMode ? F("Leader  ") : F("Follower"));    
      break;
  }
}

MakeGizmoGardenMenuItem(Spring)
{
  switch(event)
  {
    case Enter:
      printName();
      break;

    case Leave:
      SpringTask.stop();
      break;

    case Monitor:
      ggPrint(lcd, (int)proximityMonitor.getOutput(), 4);
      lcd.print(F(" => "));
      ggPrint(lcd, driver.getFullSpeed(), 4);
      break;

    case ChangeAndShow:
      leftWheel.setZero(leftWheel.getZero() + direction);
      rightWheel.setZero(leftWheel.getZero());
      lcd.setCursor(12, 0);
      ggPrint(lcd, leftWheel.getZero(), 4);
      break;

    case Select:
      if (SpringTask.isRunning())
        SpringTask.stop();
      else
        SpringTask.start();
      break;
  }
}
/*
  // Figure out how fast we should be going based on the distance
  // to the vehicle infront
  float distance = readProximitySensor();
  float driveSpeed = (stopDistance - distance) * springStiffness;

  // If we are the leader, limit speed to 80
  if (leadMode && driveSpeed > 80)
    driveSpeed = 80;
    
  driver.setFullSpeed(driveSpeed);
*/

