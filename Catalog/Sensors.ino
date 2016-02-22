/***************************************************************************
Copyright (c) 2016 Bill Silver, Member of Gizmo Garden LLC. This source code
is distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The license
can be found at <http://www.gnu.org/licenses/>. There is no express or
implied warranty, including merchantability or fitness for a particular
purpose.
***************************************************************************/

// ****************************************
// *                                      *
// *  Monitor Proximity and Road Sensors  *
// *                                      *
// ****************************************

MakeGizmoGardenMenuItem(Proximity)
{
  switch(event)
  {
    case Enter:
      printName();
      proximityMonitor.reset();
      break;
      
    case ChangeAndShow:
      proximityMonitor.setSmoothness(proximityMonitor.getSmoothness() + direction);
      lcd.setCursor(14, 1);
      ggPrint(lcd, proximityMonitor.getSmoothness(), 2);
      break;

    case Monitor:
      ggPrint(lcd, readProximitySensor(), 4);
      break;
  }
}

MakeGizmoGardenMenuItem(RoadSensors)
{
  switch(event)
  {
    case Enter:
      printName();
      break;
      
    case Monitor:
      ggPrint(lcd, analogRead(LeftSensorPin), 4);
      lcd.setCursor(12, 1);
      ggPrint(lcd, analogRead(RightSensorPin), 4);
      break;
  }
}

