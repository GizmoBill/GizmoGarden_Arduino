/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "../GizmoGarden_Menus/GizmoGardenMenus.h"

GizmoGardenMenuItem::GizmoGardenMenuItem() : RingBase(GizmoGardenMenuTask::menuRing.ring) {}
GizmoGardenMenuItem::~GizmoGardenMenuItem() { remove(GizmoGardenMenuTask::menuRing.ring); }

Ring<GizmoGardenMenuItem> GizmoGardenMenuTask::menuRing;

GizmoGardenMenuTask::GizmoGardenMenuTask(GizmoGardenLCDPrint& lcd)
: lcd(lcd), oldButtons(0), monitorCounter(0), holdCounter(0),
  GizmoGardenTask(false)
{
}

GizmoGardenText GizmoGardenMenuTask::name() const
{
  return F("Panel"); 
}

void GizmoGardenMenuTask::customStart()
{
  enter();
}

void GizmoGardenMenuTask::action(uint8_t eventCode, int8_t direction)
{
  menuRing.current()->action(eventCode, direction, lcd);
}

void GizmoGardenMenuTask::changeAndShow(int direction)
{
  lcd.setCursor(2, 1);
  action(GizmoGardenMenuItem::ChangeAndShow, direction);
}

void GizmoGardenMenuTask::enter()
{
  lcd.clear();
  action(GizmoGardenMenuItem::Enter);
  changeAndShow(0);
}

void GizmoGardenMenuTask::choose(GizmoGardenMenuItem& menu)
{
  RingBase* start = menuRing.ring;
  if (start == 0 || start == &menu)
    return;
  for (RingBase* p = start->next; p != start; p = p->next)
    if (p == &menu)
    {
      action(GizmoGardenMenuItem::Leave);
      menuRing.ring = p;
      enter();
      break;
    }
}

void GizmoGardenMenuTask::myTurn()
{
  int buttons = lcd.readButtons();
  if (oldButtons == 0 || (holdCounter >= 20 && monitorCounter == 0))
    switch (buttons)
    {
      case BUTTON_LEFT:
        action(GizmoGardenMenuItem::Leave);
        menuRing.backup();
        enter();
        break;
        
      case BUTTON_RIGHT:
        action(GizmoGardenMenuItem::Leave);
        menuRing.forward();
        enter();
        break;
      
      case BUTTON_UP:
        changeAndShow(1);
        break;

      case BUTTON_DOWN:
        changeAndShow(-1);
        break;
      
      case BUTTON_SELECT:
        action(GizmoGardenMenuItem::Select);
        break;
    }

  if (buttons == oldButtons)
    ++holdCounter;
  else
    holdCounter = 0;

  oldButtons = buttons;

  // Do this before the monitor action so that the Task Monitor menu item
  // shows the menu task as running
  callMe(50);

  ++monitorCounter;
  if (monitorCounter == 5)
  {
    lcd.setCursor(2, 1);
    action(GizmoGardenMenuItem::Monitor);
    monitorCounter = 0;
  }
}
