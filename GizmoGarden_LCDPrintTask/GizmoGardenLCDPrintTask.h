#ifndef _GizmoGardenLCDPrintTask_
#define _GizmoGardenLCDPrintTask_

/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

// ********************
// *                  *
// *  LCD Print Task  *
// *                  *
// ********************

// Printing to an Adafruit LCD display is surprisingly slow. It's
// about 5.5 ms per character for the default 100KHz clock in twi.h,
// and about 2.1 ms when changed to 400KHz.So printing many characters
// is too slow for cooperative multitasking. Instead, we create a
// special print task that prints one character (or clear or set cursor)
// per call to myTurn. Other tasks can print entire strings using this
// task, and the characters go into a ring buffer to be printed at
// leasure.

#include "../GizmoGarden_Common/GizmoGardenCommon.h"
#include "../GizmoGarden_Multitasking/GizmoGardenMultitasking.h"
#include "../Adafruit-RGB-LCD-Shield-Library-master/Adafruit_RGBLCDShield.h"

class GizmoGardenLCDPrint : public Print, public GizmoGardenTask
{
  Adafruit_RGBLCDShield lcd;

  byte buf[32];
  byte readIndex, writeIndex;
  bool full;

  void reset();
  void sendOne();

  virtual void myTurn();

protected:

public:
  GizmoGardenLCDPrint();
  void begin(int columns = 16, int rows = 2);

  void clear();
  void setCursor(uint8_t, uint8_t);

  uint8_t readButtons() { return lcd.readButtons(); }

  virtual size_t write(uint8_t);

  virtual GizmoGardenText name() const;
};

#endif
