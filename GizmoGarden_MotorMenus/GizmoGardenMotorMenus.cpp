/********************************************************************
Copyright (c) 2016 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "GizmoGardenMotorMenus.h"

GizmoGardenRotatingMotorMenu::GizmoGardenRotatingMotorMenu(GizmoGardenRotatingMotor& motor,
                                                           GizmoGardenText name)
  : motor(motor), name(name)
{
}

void GizmoGardenRotatingMotorMenu::action(uint8_t event, int8_t direction,
                                          GizmoGardenLCDPrint& lcd)
{
  switch(event)
  {
    // When we click to this menu item, make sure it's on but don't change
    // rotateOn so we remember whether it was on or off.
    case Enter:
      lcd.print(name);
      saveFullSpeed = motor.getFullSpeed();
      motor.setFullSpeed(100);
      break;

    // When we click away from this menu item, restore the on/off state to
    // what it was.
    case Leave:
      motor.setFullSpeed(saveFullSpeed);
      break;

    // Up or down clicked.
    case ChangeAndShow:
      // Set a new speed in increments of 10 (out of 100).
      motor.setSpeed(motor.getSpeed() + 10 * direction);

      // Print the new speed on the dashboard.
      ggPrint(lcd, motor.getSpeed(), 4);
      break;
  }
}


GizmoGardenMotorOffsetMenu::GizmoGardenMotorOffsetMenu(GizmoGardenPositioningMotor& motor,
                                                       GizmoGardenText name,
                                                       GizmoGardenTask* interruptTask)
  : motor(motor), name(name), interruptTask(interruptTask)
{
}

void GizmoGardenMotorOffsetMenu::action(uint8_t event, int8_t direction,
                                        GizmoGardenLCDPrint& lcd)
{
  switch(event)
  {
    case Enter:
      lcd.print(name);
      restartInterruptTask = interruptTask != 0 && interruptTask->isRunning();
      if (restartInterruptTask)
        interruptTask->stop();
      motor.setPosition(0);
      break;

    case Leave:
      if (restartInterruptTask)
        interruptTask->start();
      break;

    case ChangeAndShow:
      // Set a new offset in increments of 1 degree.
      motor.setOffset(constrain(motor.getOffset() + direction, -30, 30));

      // Print the new offset on the dashboard.
      ggPrint(lcd, motor.getOffset(), 4);
      break;
  }
}