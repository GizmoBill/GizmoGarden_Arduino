#ifndef _GizmoGardenMenus_
#define _GizmoGardenMenus_

/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

// *******************************************************
// *                                                     *
// *  See examples for description of using menu system  *
// *                                                     *
// *******************************************************

#include "../GizmoGarden_Common/GizmoGardenCommon.h"
#include "../GizmoGarden_Multitasking/GizmoGardenMultitasking.h"
#include "../GizmoGarden_LCDPrintTask/GizmoGardenLCDPrintTask.h"

class GizmoGardenMenuItem : public RingBase
{
protected:
  friend class GizmoGardenMenuTask;

  enum EventCodes
  {
    Enter, Leave, ChangeAndShow, Monitor, Select
  };

  GizmoGardenMenuItem();
  ~GizmoGardenMenuItem();

  virtual void action(uint8_t event, int8_t direction, GizmoGardenLCDPrint&) = 0;
};

#define MakeGizmoGardenMenuItem(name)                                   \
class name##Class : public GizmoGardenMenuItem                          \
{                                                                       \
  void printName() const { lcd.print(F(#name)); }                       \
  virtual void action(uint8_t event, int8_t direction,                  \
                      GizmoGardenLCDPrint&);                            \
} name;                                                                 \
void name##Class::action(uint8_t event, int8_t direction,               \
                         GizmoGardenLCDPrint& lcd)

class GizmoGardenMenuTask : public GizmoGardenTask
{
  friend class GizmoGardenMenuItem;
  static Ring<GizmoGardenMenuItem> menuRing;

  GizmoGardenLCDPrint& lcd;

  uint8_t oldButtons;
  uint8_t monitorCounter;
  uint8_t holdCounter;

  void action(uint8_t eventCode, int8_t direction = 0);
  void enter();

protected:
  virtual void myTurn();

public:
  GizmoGardenMenuTask(GizmoGardenLCDPrint&);

  virtual void customStart();

  virtual GizmoGardenText name() const;

  void choose(GizmoGardenMenuItem&);
  void changeAndShow(int direction);
  void select() { action(GizmoGardenMenuItem::Select); }
  GizmoGardenMenuItem* current() { return menuRing.current(); }
};

#endif
