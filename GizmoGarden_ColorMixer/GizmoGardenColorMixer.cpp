/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "GizmoGardenColorMixer.h"

void GizmoGardenColorMixer::SpinTask::myTurn()
{
  pixels.setPixelColor(index, 0);
  index = (index + 1) % pixels.numPixels();
  pixels.setPixelColor(index, rgb[0], rgb[1], rgb[2]);
  pixels.show();
  callMe(100);
}

void GizmoGardenColorMixer::SpinTask::customStop()
{
  pixels.clear();
  pixels.show();
}

GizmoGardenColorMixer::SpinTask::SpinTask(GizmoGardenPixels& pixels)
  : GizmoGardenTask(false), pixels(pixels), index(0)
{
  for (int i = 0; i < 3; ++i)
    rgb[i] = 0;
}

void GizmoGardenColorMixer::MixMenu::action(uint8_t event, int8_t direction, GizmoGardenLCDPrint& lcd)
{
  switch(event)
  {
    case Enter:
      lcd.print(F("Mix Color"));
      printMode(lcd);
      restartInterruptTask = interruptTask != 0 && interruptTask->isRunning();
      if (restartInterruptTask)
        interruptTask->stop();
      spinTask.start();
      break;

    case Leave:
      spinTask.stop();
      if (restartInterruptTask)
        interruptTask->start();
      break;

    case ChangeAndShow:
      for (int i = 0; i < 3; ++i)
      {
        if (mode & (1 << i))
          spinTask.rgb[i] = constrain(spinTask.rgb[i] + 8 * direction, 0, 255);
        lcd.setCursor(5 * i + 2, 1);
        ggPrint(lcd, spinTask.rgb[i], 3);
        lcd.write("rgb"[i]);
      }
      break;

    case Select:
      if (++mode == 8)
        mode = 1;
      printMode(lcd);
      break;
  }
}

void GizmoGardenColorMixer::MixMenu::printMode(GizmoGardenLCDPrint& lcd)
{
  GizmoGardenText names(F("Red   Green YellowBlue  MagentCyan  White"));
  lcd.setCursor(10, 0);
  ggPrint(lcd, names + 6 * (mode - 1), 6);
}

GizmoGardenColorMixer::MixMenu::MixMenu(SpinTask& spinTask, GizmoGardenTask* interruptTask)
: spinTask(spinTask), mode(1), interruptTask(interruptTask), restartInterruptTask(false)
{
}

GizmoGardenColorMixer::GizmoGardenColorMixer(GizmoGardenPixels& pixels, GizmoGardenTask* interruptTask)
: spinTask(pixels), mixMenu(spinTask, interruptTask)
{
}
