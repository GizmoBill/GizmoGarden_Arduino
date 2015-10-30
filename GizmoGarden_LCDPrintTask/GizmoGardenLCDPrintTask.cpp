/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "GizmoGardenLCDPrintTask.h"

enum
{
  CharCode   = 0x00,
  ClearCode  = 0x80,
  CursorCode = 0xC0,
};

GizmoGardenLCDPrint::GizmoGardenLCDPrint()
: GizmoGardenTask(false)
{
  reset();
}

GizmoGardenText GizmoGardenLCDPrint::name() const
{
  return F("LCD"); 
}

void GizmoGardenLCDPrint::begin(int columns, int rows)
{
  lcd.begin(columns, rows);
}

void GizmoGardenLCDPrint::reset()
{
  readIndex = writeIndex = 0;
  full = false;
}

void GizmoGardenLCDPrint::clear()
{
  reset();
  write(ClearCode);
}

void GizmoGardenLCDPrint::setCursor(uint8_t col, uint8_t row)
{
  write(CursorCode + ((col & 15) << 2) + (row & 3));
}

size_t GizmoGardenLCDPrint::write(uint8_t c)
{
  if (full)
    sendOne();
  buf[writeIndex] = c;
  writeIndex = (writeIndex + 1) & 0x1F;
  full = readIndex == writeIndex;
  if (!isRunning())
    start();
  return 1;
}

void GizmoGardenLCDPrint::sendOne()
{
  uint8_t c = buf[readIndex];
  readIndex = (readIndex + 1) & 0x1F;
  full = false;

  if (c < 0x80)
    lcd.write(c);
  else if (c == ClearCode)
    lcd.clear();
  else if ((c & 0xC0) == CursorCode)
    lcd.setCursor((c >> 2) & 15, c & 3);
}

void GizmoGardenLCDPrint::myTurn()
{
  sendOne();
  if (readIndex != writeIndex)
    fitMeIn(2);   // Assumes I2C set to 400KHz in twi.h
}
