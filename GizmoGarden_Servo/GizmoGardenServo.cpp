/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "GizmoGardenServo.h"

// ********************************
// *                              *
// *  Gizmo Garden Servo Control  *
// *                              *
// ********************************

// The following are taken from GizmoGardenCommon.h and copied here
// so that this code can stand alone. See comments there for more
// info.

#undef min
#undef max
#undef constrain

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

// ***********************
// *                     *
// *  Interrupt Handler  *
// *                     *
// ***********************

// You can use a different timer by changing these macros
#define TCNT  TCNT1
#define OCRA  OCR1A
#define TCCRA TCCR1A
#define TCCRB TCCR1B
#define TIFR  TIFR1
#define TIMSK TIMSK1

inline void GizmoGardenServo::interruptHandler()
{
  if (current == 0)
  {
    TCNT = 0;
    current = servoList;
  }
  else
  {
    while (TCNT < current->pulseEnd);
    digitalWrite(current->pin, LOW);
    current = current->next;
  }

  for (; current != 0; current = current->next)
    if (current->isEnabled())
    {
      current->pulseEnd = TCNT + current->pulseTicks + TrimTicks;
      OCRA = current->pulseEnd - usToTicks(JitterMargin);
      digitalWrite(current->pin, HIGH);
      return;
    }

  OCRA = max(usToTicks(RefreshInterval), TCNT + 4);

  for (ServoCallback* sc = ServoCallback::list; sc != 0; sc = sc->next)
    if (sc->callbackScheduled)
    {
      sc->callback();
      sc->callbackScheduled = false;
    }
}

#ifndef WIRING
SIGNAL(TIMER1_COMPA_vect)
{
  GizmoGardenServo::interruptHandler();
}
#else
void Timer1Service()
{
  GizmoGardenServo::interruptHandler();
}
#endif

// *******************
// *                 *
// *  ServoCallback  *
// *                 *
// *******************

ServoCallback* ServoCallback::list = 0;

ServoCallback::ServoCallback()
: next(list), callbackScheduled(false)
{
  IntOffBlock iof;
  list = this;
}

ServoCallback::~ServoCallback()
{
  IntOffBlock iof;
  ServoCallback* q = 0;
  for (ServoCallback*p = list; p != this; q = p, p = p->next);
  if (q != 0)
    q->next = next;
  else
    list = next;
}

void ServoCallback::call()
{
  IntOffBlock iof;
  if (GizmoGardenServo::current == 0)
    callback();
  else
    callbackScheduled = true;
}

// **************************
// *                        *
// *  Timer Initialization  *
// *                        *
// **************************

void GizmoGardenServo::begin()
{
  TCCRA = 0;             // normal counting mode
  TCCRB = _BV(CS11);     // set prescaler to 8
  TCNT  = 0;             // clear the timer count
  TIFR |= _BV(OCF1A);    // clear any pending interrupts
  TIMSK|= _BV(OCIE1A);   // enable output compare interrupt

#if defined(WIRING)
  timerAttach(TIMER1OUTCOMPAREA_INT, Timer1Service);
#endif
}

// ********************************
// *                              *
// *  Construction & Destruction  *
// *                              *
// ********************************

GizmoGardenServo* GizmoGardenServo::servoList = 0;
GizmoGardenServo* GizmoGardenServo::current = 0;

GizmoGardenServo::GizmoGardenServo(int pin, int minPulseTime, int maxPulseTime)
{
  init(pin, minPulseTime, maxPulseTime);
  writeMicroseconds(DefaultPulseTime);

  next = 0;
  GizmoGardenServo* q = 0;
  for (GizmoGardenServo* p = servoList; p != 0; q = p, p = p->next);

  {
    IntOffBlock iof;
    if (q != 0)
      q->next = this;
    else
      servoList = this;
  }
}

GizmoGardenServo::~GizmoGardenServo()
{
  GizmoGardenServo* q = 0;
  for (GizmoGardenServo* p = servoList; p != 0; q = p, p = p->next)
    if (p == this)
    {
      IntOffBlock iof;
      if (q != 0)
      {
        q->next = next;
        if (current == this)
          current = q;
      }
      else
      {
        servoList = next;
        if (current == this)
          current = 0;
      }
    }
}

void GizmoGardenServo::init(int pin, int minTime, int maxTime)
{
  this->pin = (int8_t)pin;
  minPulse(minTime);
  maxPulse(maxTime);

  if (this->pin != -1)
    pinMode(pin & 0x7F, OUTPUT);
}

void GizmoGardenServo::enable(bool enabled)
{
  if (enabled && pin != -1)
    pin &= ~0x80;
  else
    pin |= 0x80;
}

// ******************
// *                *
// *  Read & Write  *
// *                *
// ******************

void GizmoGardenServo::write(int value)
{
  if (value < MinPulseTime)
    value = (int)map(value, 0, 180, minPulse(),  maxPulse());
  writeMicroseconds(value);
}

void GizmoGardenServo::writeMicroseconds(int value)
{
  uint16_t ticks = usToTicks(constrain(value, minPulse(), maxPulse()));
  {
    IntOffBlock iof;
    pulseTicks = ticks;
  }
}

int GizmoGardenServo::read()
{
  return (int)map(readMicroseconds() + 1, minPulse(), maxPulse(), 0, 180);
}

int GizmoGardenServo::readMicroseconds()
{
  return ticksToUs(pulseTicks);
}

// ***********************
// *                     *
// *  Servo.h Interface  *
// *                     *
// ***********************

void GizmoGardenServo::attach(int pin, int minPulseTime, int maxPulseTime)
{
  init(pin, minPulseTime, maxPulseTime);
  if ((TIMSK & _BV(OCIE1A)) == 0)
    begin();
}
