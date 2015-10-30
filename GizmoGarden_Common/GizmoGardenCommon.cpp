/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "GizmoGardenCommon.h"

// **********************************
// *                                *
// *  Timing for Music and Dancing  *
// *                                *
// **********************************

uint16_t getMusicTime(GizmoGardenText& notes, int beatLength)
{
  char c;
  do
  {
    c = *notes++;
  }
  while (c == ' ');

  static const char* durationTable = "SEQHWT";
  for (int i = 0; durationTable[i] != 0; ++i)
    if (durationTable[i] == c)
    {
      if (i < 5)
      {
        // Power of 2 notes
        uint16_t t = 2 << i;
        c = *notes;
        if (c == '.')
        {
          t += 1 << i;
          ++notes;
        }
      
        return t * beatLength >> 3;
      }
      else
        // "T" is a triplet note
        return (beatLength + 1) / 3;
    }

  --notes;
  return 0;
}

// ************************
// *                      *
// *  Formatted Printing  *
// *                      *
// ************************

struct MemoryPrinter : public Print
{
  uint8_t buf[16];
  int index;
  virtual size_t write(uint8_t);
  MemoryPrinter() : index(0) {}
  void sendNumber(Print& device, int columns);
};

size_t MemoryPrinter::write(uint8_t ch)
{
  if (index < sizeof(buf))
    buf[index++] = ch;
}

void MemoryPrinter::sendNumber(Print& device, int columns)
{
  int pad = columns - index;
  while (pad > 0)
  {
    device.write(' ');
    --pad;
  }
  if (pad == 0)
    for (int i = 0; i < index; ++i)
      device.write(buf[i]);
  else
    for (int i = 0; i < columns; ++i)
      device.write('*');
}

void ggPrint(Print& device, long n, int columns)
{
  MemoryPrinter mp;
  mp.print(n);
  mp.sendNumber(device, columns);
}

void ggPrint(Print& device, float x, int columns, int places)
{
  MemoryPrinter mp;
  mp.print(x, places);
  mp.sendNumber(device, columns);
}

void ggPrint(Print& device, GizmoGardenText s, int columns)
{
  while (columns > 0)
  {
    uint8_t c = *s++;
    if (c == 0)
      break;
    device.write(c);
    --columns;
  }
  while (columns-- > 0)
    device.write(' ');
}

bool objectsEqual(const byte* a, const byte* b, size_t n)
{
  for (int i = 0; i < n; ++i)
    if (a[i] != b[i])
      return false;
  return true;
}

// **********************
// *                    *
// *  Smoothing Filter  *
// *                    *
// **********************

GizmoGardenSmoother::GizmoGardenSmoother(int smoothness)
  : currentSmoothness(0), y(0)
{
  setSmoothness(smoothness);
}

void GizmoGardenSmoother::setFilter()
{
  filterK = (float)pow(2.0f, -0.5f * currentSmoothness);
}

void GizmoGardenSmoother::setSmoothness(int smoothness)
{
  this->smoothness = (int8_t)constrain(smoothness, 0, 12);
  currentSmoothness = min(currentSmoothness, this->smoothness);
  setFilter();
}

float GizmoGardenSmoother::input(float x)
{
  y += filterK * (x - y);
  if (currentSmoothness < smoothness)
  {
    ++currentSmoothness;
    setFilter();
  }
  return y;
}

// ***********
// *         *
// *  Rings  *
// *         *
// ***********

RingBase::RingBase(RingBase*& ring)
{
  if (ring == 0)
    ring = this;
  else
    ring->previous()->next = this;
  next = ring;
}

void RingBase::remove(RingBase*& ring)
{
  previous()->next = next;
  if (ring == this)
    if (next == this)
      ring = 0;
    else
      ring = next;
}

RingBase* RingBase::previous()
{
  RingBase* p;
  for (p = this; p->next != this; p = p->next);
  return p;
}
