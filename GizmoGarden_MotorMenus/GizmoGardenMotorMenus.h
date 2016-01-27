#ifndef _GizmoGardenMotorMenus_
#define _GizmoGardenMotorMenus_

/********************************************************************
Copyright (c) 2016 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "../GizmoGarden_Multitasking/GizmoGardenMultitasking.h"
#include "../GizmoGarden_Menus/GizmoGardenMenus.h"
#include "../GizmoGarden_Motors/GizmoGardenMotors.h"

class GizmoGardenRotatingMotorMenu : public GizmoGardenMenuItem
{
  int8_t saveFullSpeed;

protected:
  GizmoGardenRotatingMotor& motor;
  GizmoGardenText name;

  virtual void action(uint8_t event, int8_t direction, GizmoGardenLCDPrint&);

public:
  GizmoGardenRotatingMotorMenu(GizmoGardenRotatingMotor&, GizmoGardenText name);
};

class GizmoGardenMotorOffsetMenu : public GizmoGardenMenuItem
{
  GizmoGardenTask* interruptTask;
  bool restartInterruptTask;

protected:
  GizmoGardenPositioningMotor& motor;
  GizmoGardenText name;

  virtual void action(uint8_t event, int8_t direction, GizmoGardenLCDPrint&);

public:
  GizmoGardenMotorOffsetMenu(GizmoGardenPositioningMotor& motor, GizmoGardenText name,
                             GizmoGardenTask* interruptTask = 0);
};

#endif
