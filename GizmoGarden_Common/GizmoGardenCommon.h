#ifndef _GizmoGardenCommon_
#define _GizmoGardenCommon_

/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include <Arduino.h>
#include <inttypes.h>

// *************************************
// *                                   *
// *  Gizmo Garden Common Definitions  *
// *                                   *
// *************************************
//
// Arduino.h defines min, max, abs, constrain, sq, etc., as macros, following a
// very bad programming style intended to create type-independent inline-ish
// functions, which can now be accomplished properly with inline templates. The
// documentation warns against improper use of the macros, but seriously? Here we
// undefine the offensive macros and replace them with inline template functions. 

#undef min
#undef max
#undef constrain
#undef sq

template <class T>
inline T min(T x, T y)
{
  return x < y ? x : y;
}

template <class T>
inline T max(T x, T y)
{
  return x > y ? x : y;
}

template <class T, class A, class B>
inline T constrain(T x, A lo, B hi)
{
  return x <= lo ? lo : (x >= hi ? hi : x); 
}

template <class T>
inline T sq(T x)
{
  return x * x;
}

// The absolute value function is a mess. If we undefine the offensive
// macro, we fall back to a C-linkage version in stdlib.h, which I'm not
// sure I trust since Arduino replaced it with the macro. So we define
// inline Gizmo Garden versions.

template <class T>
inline T ggAbs(T x)
{
  return x >= 0 ? x : -x;
}

// ***************************
// *                         *
// *  Text Strings in Flash  *
// *                         *
// ***************************
//
// GizmoGardenText is a pointer to a string in flash. Like native pointers,
// it can be used with *, [], ++, and -- operators. It can be converted
// implicitly to or from a (const __FlashStringHelper*), for use with
// the F macro and the various print classes. Unlike F, one can make a
// string in flash with static initialization.

class GizmoGardenText
{
  const char* text;

public:
  GizmoGardenText() {}
  GizmoGardenText(const __FlashStringHelper* s) : text((const char*) s) {}

  operator const __FlashStringHelper*() const { return (const __FlashStringHelper*) text; }

  char operator[](int i) const { return pgm_read_byte(text + i); }
  char operator*() const { return pgm_read_byte(text); }

  GizmoGardenText& operator++() { ++text; return *this; }
  GizmoGardenText operator++(int) { GizmoGardenText gt = *this; ++text; return gt; }

  GizmoGardenText& operator--() { --text; return *this; }
  GizmoGardenText operator--(int) { GizmoGardenText gt = *this; --text; return gt; }
};

// This macro makes a string in flash using static initialization.
#define MakeGizmoGardenText(name, text)                         \
const char _##name[] PROGMEM = text;                            \
const GizmoGardenText name((const __FlashStringHelper*)_##name);

// ***********************
// *                     *
// *  Utility Functions  *
// *                     *
// ***********************

// These print functions extend the Arduino-supplied print by allowing
// you to specify the number of columns.
void ggPrint(Print& device, long n, int columns);
void ggPrint(Print& device, float x, int columns, int places);
void ggPrint(Print& device, GizmoGardenText, int columns);

// These functions are useful for comparing a struct in EEPROM with
// one in SRAM, to see if data has been saved.
bool objectsEqual(const byte*, const byte*, size_t);

template <class T>
bool objectsEqual(const T& a, const T& b)
{
  return objectsEqual((const byte*)&a, (const byte*)&b, sizeof(T));
}

// ***********************************
// *                                 *
// *  Music and Choreography Timing  *
// *                                 *
// ***********************************

// Return time in milliseconds for the next note in the specified
// string, and update the string pointer. beatlength is the time
// for a quarter note. Times are specified by a character:
//    S   sixteenth
//    E   eighth
//    Q   quarter
//    H   half
//    W   whole
//    T   triplet (beatlength / 3)
//
// All notes except T can be followed by a "." to extend the
// time by half. Spaces are ignored, except that they cannot appear
// between a note and the ".".
// Return 0 when the end of the string, or any unrecognized text,
// is reached.
uint16_t getMusicTime(GizmoGardenText&, int beatLength);

// **********************
// *                    *
// *  Smoothing Filter  *
// *                    *
// **********************
//
// One-pole low-pass filter with well-behaved initialization.
// The smoothness parameter is an integer [0 .. 12], where 0 means
// no smoothing (output = input) and the filter constant is
// 2^-(smoothness/2).

// For well-behaved initialization, when smoothness is set to
// a new value the actual smoothness is never increased by more
// than one step for each call to input. 

class GizmoGardenSmoother
{
public:
  GizmoGardenSmoother(int smoothness);

  // Get and set the current smoothness.
  int getSmoothness() const { return smoothness; }
  void setSmoothness(int);

  // Get the current output value
  float getOutput() const { return y; }

  // Input the next value, return output.
  float input(float x);

  // Reset the filter, throwing away all history
  void reset() { currentSmoothness = 0; }

private:
  void setFilter();
  int8_t currentSmoothness;
  int8_t smoothness;
  float filterK;
  float y;
};

// ***********
// *         *
// *  Rings  *
// *         *
// ***********
//
// A Ring is a circular list of objects, using single forward pointers.
// It is used by system components that need to keep track of all
// objects of certain classes, for example all tasks or all menu
// items. A Ring of the objects is gathered automatically, with
// no Sketch code necessary.

// RingBase must be a base class of any class whose objects will be
// gathered into a Ring. It implements construction and removal
// for automatic gathering, and getting the next and previous objects
// on the Ring.
class RingBase
{
public:
  // These always return a valid pointer
  RingBase* next;
  RingBase* previous();

protected:
  // Can only be constructed from a derived class and only
  // on a Ring.
  RingBase(RingBase*&);

  // We can't remove from a destructor, since we don't know
  // where the Ring is. Derived classes can call remove from
  // their destructors. 
  void remove(RingBase*&);
};

// Ring<T> is a ring of objects of type T. T must derive from
// RingBase. Rings maintain a current pointer, which
// can be moved forward or backward. Typically Ring<T> is a
// static member of some other class.
template <class T>
class Ring
{
public:
  RingBase* ring;

  Ring() : ring(0) {}

  void forward();
  void backup ();
  T* current() { return (T*)ring; }
};

template <class T>
void Ring<T>::forward()
{
  if (ring)
    ring = ring->next;
}

template <class T>
void Ring<T>::backup()
{
  if (ring)
    ring = ring->previous();
}

// **************************
// *                        *
// *  Interrupts Off Block  *
// *                        *
// **************************
//
// To create a code block that runs with interrupts off, do this:
//
// {
//   IntOffBlock iof;
//   ... any code ...
// }
//
// Interrupts will be restored to their previous state on exit from
// the block, whether that exit is by means of normal termination,
// break, continue, return, goto, or throw.

class IntOffBlock
{
  uint8_t saveSREG;

public:
  IntOffBlock()
  {
    saveSREG = SREG;
    cli();
  }

  ~IntOffBlock() { SREG = saveSREG; }
};

#endif
