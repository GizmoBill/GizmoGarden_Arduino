/***************************************************************************
Copyright (c) 2016 Bill Silver, Member of Gizmo Garden LLC. This source code
is distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The license
can be found at <http://www.gnu.org/licenses/>. There is no express or
implied warranty, including merchantability or fitness for a particular
purpose.
***************************************************************************/

// *********************************************
// *                                           *
// *  Drive on the Road and Follow the Leader  *
// *                                           *
// *********************************************

bool parading;
uint32_t stopTimer;

// Show the current speed and road sensor readings while driving
MakeGizmoGardenTask(Dashboard)
{
  lcd.setCursor(12, 0);
  ggPrint(lcd, driver.getFullSpeed(), 4);
  for (int s = 0; s < 2; ++s)
  {
    lcd.setCursor(8 * s + 2, 1);
    ggPrint(lcd, driver.getSmoothedOnRoad(s) * 100 / 128, 4);
  }

  callMe(250);
}

// This is the main driving task. It follows the vehicle in front by controlling speed,
// and starts the driver task (which does steering) and the Dashboard task.
MakeGizmoGardenTaskWithStartStop(SpeedControl)
{
  // If we are still on the road, keep going
  if (driver.isRunning())
  {
    gasPedal();

    int speed = driver.getFullSpeed();
    uint32_t now = millis();
    if (speed > 10)
      stopTimer = now;

    if (parading)
    {
      if (now - stopTimer >= 1000)
      {
        GizmoGardenEventRegistry::raiseEvent(DriveStop);
        parading = false;
      }
    }
    else
    {
      if (speed >= 30)
      {
        GizmoGardenEventRegistry::raiseEvent(DriveStart);
        parading = true;
      }
    }
    
    callMe(50);
  }
}

// Here are things we have to do to when starting DriveTask
CustomStart(SpeedControl)
{
  proximityMonitor.reset();
  parading = false;
  driver.start();
  Dashboard.start();
}

CustomStop(SpeedControl)
{
  driver.stop();
  Dashboard.stop();
  GizmoGardenEventRegistry::raiseEvent(DriveStop);
}

// ******************
// *                *
// *  Driver Menus  *
// *                *
// ******************

MakeGizmoGardenMenuItem(DriveStiffness)
{
  switch(event)
  {
    case Enter:
      printName();
      break;
      
    case ChangeAndShow:
      driver.setStiffness(driver.getStiffness() + 0.1 * direction);
      ggPrint(lcd, driver.getStiffness(), 4, 2);
      break;
  }
}

MakeGizmoGardenMenuItem(Calibrate)
{
  switch(event)
  {
    case Enter:
      printName();
      break;

    case Select:
      driver.calibrate();
      
    case ChangeAndShow:
      for (int s = 0; s < 2; ++s)
      {
        lcd.setCursor(8 * s, 1);
        ggPrint(lcd, driver.getDark(s), 3);
        lcd.write('-');
        ggPrint(lcd, driver.getRange(s), 3);
      }
      break;
  }
}

MakeGizmoGardenMenuItem(Drive)
{
  switch (event)
  {
    case Enter:
      printName();
      break;

    case Leave:
    case ChangeAndShow:
      SpeedControl.stop();
      break;

    case Select:
      if (SpeedControl.isRunning())
        SpeedControl.stop();
      else
        SpeedControl.start();
      break;
  }
}

