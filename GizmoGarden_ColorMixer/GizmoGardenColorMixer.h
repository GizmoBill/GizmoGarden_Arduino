#ifndef _GizmoGardenColorMixer_
#define _GizmoGardenColorMixer_

/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "../GizmoGarden_Multitasking/GizmoGardenMultitasking.h"
#include "../GizmoGarden_Menus/GizmoGardenMenus.h"
#include "../GizmoGarden_Pixels/GizmoGardenPixels.h"

class GizmoGardenColorMixer
{
  class SpinTask : public GizmoGardenTask
  {
    uint8_t index;

  protected:
    virtual void myTurn();
    CUSTOM_STOP

    GizmoGardenPixels& pixels;

  public:
    uint8_t rgb[3];
    SpinTask(GizmoGardenPixels&);
  }
  spinTask;

  class MixMenu : public GizmoGardenMenuItem
  {
    uint8_t mode;
    void printMode(GizmoGardenLCDPrint& lcd);

    GizmoGardenTask* interruptTask;
    bool restartInterruptTask;

  protected:
    virtual void action(uint8_t event, int8_t direction, GizmoGardenLCDPrint&);
    SpinTask& spinTask;

  public:
    MixMenu(SpinTask&, GizmoGardenTask* interruptTask);
  }
  mixMenu;

public:
  GizmoGardenColorMixer(GizmoGardenPixels&, GizmoGardenTask* interruptTask = 0);
};

#endif
