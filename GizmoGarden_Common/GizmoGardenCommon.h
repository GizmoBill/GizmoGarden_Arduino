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
// *  Flash Memory Pointers  *
// *                         *
// ***************************
//
// Create pointers to constant arrays in program (flash) memory that, by means of operator
// overloading, behave like normal C++ pointers, including in particular array access. This
// allows code that creates and uses constant arrays of various types, using normal array
// access syntax, to keep those arrays in program memory with very small changes to the
// code and without having to learn much about program memory access.

// Place PROGSPACE at the beginning of an array declaration with static initialization.
// Then construct a ProgSpacePointer using the typedefs below for access to the array.
// The ProgSpacePointer can be a local variable in the function that needs the array
// access.
#define PROGSPACE const PROGMEM

template <class T>
class ProgSpacePointer
{
protected:
  const T* p;

public:
  ProgSpacePointer() {}
  ProgSpacePointer(const T* p) : p(p) {}

  T operator[](int i) const;
  T operator*() const { return (*this)[0]; }

  ProgSpacePointer& operator++() { ++p; return *this; }
  ProgSpacePointer operator++(int) { ProgSpacePointer t = *this; ++p; return t; }

  ProgSpacePointer& operator--() { --p; return *this; }
  ProgSpacePointer operator--(int) { ProgSpacePointer t = *this; --p; return t; }

  bool operator==(ProgSpacePointer s) const { return p == s.p; }
  bool operator!=(ProgSpacePointer s) const { return p != s.p; }

  ProgSpacePointer operator+(int n) const { ProgSpacePointer s; s.p = p + n; return s; }
  ProgSpacePointer operator-(int n) const { ProgSpacePointer s; s.p = p - n; return s; }

  int operator-(ProgSpacePointer s) const { return p - s.p; }
};

// The only template function needed to mechanize ProgSpacePointer for a particular type
// is operator[]. The following works for any type (somewhat inefficiently), with
// efficient specializations for built-in numeric types below.
template<class T>
T ProgSpacePointer<T>::operator[](int i) const
{
  T c;
  for (int i = 0; i < sizeof(T); ++i)
    ((char*)&c)[i] = pgm_read_byte(((char*)(p + i))[i]);
  return c;
}

template <>
inline int ProgSpacePointer<int>::operator[](int i) const
{
  return pgm_read_word(p + i);
}

template <>
inline unsigned int ProgSpacePointer<unsigned int>::operator[](int i) const
{
  return pgm_read_word(p + i);
}

template <>
inline char ProgSpacePointer<char>::operator[](int i) const
{
  return pgm_read_byte(p + i);
}

template <>
inline byte ProgSpacePointer<byte>::operator[](int i) const
{
  return pgm_read_byte(p + i);
}

template <>
inline long ProgSpacePointer<long>::operator[](int i) const
{
  return pgm_read_dword(p + i);
}

template <>
inline unsigned long ProgSpacePointer<unsigned long>::operator[](int i) const
{
  return pgm_read_dword(p + i);
}

typedef ProgSpacePointer<char> ProgChars;
typedef ProgSpacePointer<byte> ProgBytes;
typedef ProgSpacePointer<int> ProgInts;
typedef ProgSpacePointer<unsigned int> ProgUInts;
typedef ProgSpacePointer<long> ProgLongs;
typedef ProgSpacePointer<unsigned long> ProgULongs;

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

class GizmoGardenText : public ProgSpacePointer<char>
{
public:
  GizmoGardenText() {}
  GizmoGardenText(const __FlashStringHelper* s) : ProgSpacePointer<char>((const char*) s) {}
  GizmoGardenText(ProgSpacePointer<char> p) : ProgSpacePointer<char>(p) {}

  operator const __FlashStringHelper*() const { return (const __FlashStringHelper*) p; }
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

  Ring(bool noStaticInit) { if (!noStaticInit) ring = 0; }
  // Most of the objects that end up on rings are constructed at static initialization
  // time and inserted on the ring in that construction. In these cases the ring itself
  // is constructed at static initialization time. Since the order of static
  // initialization is hard to control, the ring may be constructed after objects that
  // need to go on it and clear the ring. By constructing with noStaticInit = true,
  // clearing ring is skipped, and we rely on the normal clearing of uninitialized 
  // memory.

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

// ************************
// *                      *
// *  Event Registration  *
// *                      *
// ************************
//
// This class allows a function to be registerd so that it is called when certain
// events occur. This is intended to be used by modules loaded into a sketch as a
// separate tab from a catalog of software modules. Those modules can have a
// function that will be called on certain events without needing to modify any
// other parts of the sketch. It costs some memory (4 bytes per registered function)
// and provides some convenience in writing and using modules.

class GizmoGardenEventRegistry
{
  static GizmoGardenEventRegistry* list;
  GizmoGardenEventRegistry* next;
  void (*eventFunction)(uint8_t eventCode);

public:
  GizmoGardenEventRegistry(void (*eventFunction)(uint8_t eventCode));
  static void raiseEvent(uint8_t eventCode);
};

#define GizmoGardenSetup(name) GizmoGardenEventRegistry name##Item(name)

#endif
