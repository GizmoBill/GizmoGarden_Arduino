#ifndef _GizmoGardenServo_
#define _GizmoGardenServo_

/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

/*
This is a jitter-free software-interrupt based servo controller. It uses
timer 1 and can control up to a dozen servos on any digital pins. It
presents a client interface that is compatible with the standard Arduino-
supplied servo library, as well as a new interface that is perhaps more
sensible. Servos can be controlled even in the presence of software that
keeps interrupts off for extended periods of time, such as Adafruit
NeoPixels.

Servo control is jitter free in the presence of all of the normal short-
duration events that run with interrupts off: interrupt handlers and
code that disables interrupts to avoid race conditions. "Short-duration"
is a settable parameter, default is 25 microseconds. For long-duration
events, such as updating Adafruit NeoPixels, explicit synchronization
with the servo controller is required. This is simple to do using the
ServoCallback class. "Long-duration" can safely be up to 13 milliseconds.

Credit to the author of the standard servo library for the general
concept of round-robin control. 

Theory of Operation
-------------------
Like the standard Arduino-supplied servo library, servos are pulsed in
round-robin fashion like this:
           ___                                 ___
Servo 1 __|   |_______________________________|   |_______________
                _____                               _____
Servo 2 _______|     |_____________________________|     |________
                       ____                                ____
Servo 2 ______________|    |______________________________|    |__
          A   B      C     D                  A   B      C     D

Interrupts occur at signal transition points, labeled A, B, C, and D.
Servo jitter occurs if the trailing edge of a pulse is late, which
in the standard servo library happens if interrupts are off at signal
transition times B, C, or D in the above example. It doesn't matter
if the leading edge of the pulse is late, because the trailing edge is
pushed back accordingly, so the pulse length is unchanged.

The period of pulses on any servo is not critical. It is nominally 20 ms,
but can be as long as 33 ms or a little shorter than 20.

To avoid jitter, interrupts are scheduled to happen slightly before the
signal transition time. The interrupt handler then spin-waits for the
proper time. "Slightly before" is set by the JitterMargin parameter.
Any short-duration event than runs with interrupts off for up to
JitterMargin microseconds will have no effect on servo timing. For
short-duration events that are longer, timing variability is reduced
by JitterMargin.

The spin-wait obviously makes the servo interrupt handler run slightly
longer, but doesn't increase the overall interrupt latency of the
system because JitterMargin can be adjusted to match the longest of
such events. But why bother, 25 microseconds seems to be fine and
is no big deal.

Long-duration events can safely run with interrupts off if they do so
in the time between D and A in the above example. Point A can be delayed by
up to 13 ms.

For long-duration events, create a class that has ServoCallback as a
private base class. Override the pure virtual function "callback" to
do whatever must be done with interrupts off. Interrupts will be off
when callback is called, and must remain off. When the event occurs,
call the "call" member function. If the servo control is currently in
the safe zone between D and A, callback will execute immediatly. If
not, callback will be called in the interrupt handler at the next point
D.

Note that there can be as many instances of classes using ServoCallback
as you need.
*/
#include <Arduino.h>
#include <inttypes.h>

// ********************************
// *                              *
// *  Gizmo Garden Servo Control  *
// *                              *
// ********************************

// I don't like using #define for integer constants because macro names
// do not obey C++ scoping rules.
enum GizmoGardenServoParameters
{
  MinPulseTime      =   544,  // microseconds
  MaxPulseTime      =  2400,  // microseconds
  MinMaxAdjustScale =     2,  // log2 scale factor for minAdjust, maxAdjust
  DefaultPulseTime  =  1500,  // microseconds
  RefreshInterval   = 20000,  // nominal, may vary, microseconds
  JitterMargin      =    25,  // microseconds
  TrimTicks         =     2,  // timer clock ticks, emperical adjustment
                              //   measured with oscilloscope
};

class ServoCallback
{
  friend class GizmoGardenServo;

  // Maintain linked list of all ServoCallbacks
  static ServoCallback* list;
  ServoCallback* next;

  // This callback needs to run at the next safe interrupt
  bool callbackScheduled;

protected:
  // Add or remove this ServoCallback from the global list
  ServoCallback();
  ~ServoCallback();

  // Call callback with interrupts off, either immediatly or by
  // scheduling for the next safe time
  void call();

  // Cancel any pending callback
  void cancel() { callbackScheduled = false; }

  // Override this to do whatever must be done with interrupts off.
  // Interrupts will be off when called, and must remain off.
  virtual void callback() = 0;
};

class GizmoGardenServo
{
public:
  // You can construct a servo with a pin number, so you don't need to
  // call attach. Making all of the arguments optional maintans compatibility
  // with the standard servo library.
  GizmoGardenServo(int pin = -1, int minPulseTime = MinPulseTime, int maxPulseTime = MaxPulseTime);
  ~GizmoGardenServo();

  // Enable or disable this servo, as specified by the argument. A disabled
  // servo doesn't generate pulses, but remembers its pin number and
  // other parameters so it can just be enabled again.
  void enable(bool);
  bool isEnabled() const { return pin >= 0; }

  // The following read and write functions are compatible with the
  // standard servo library.

  // Write pulse width in degrees (value < MinPulseTime) or microseconds
  // (value >= MinPulseTime).
  void write(int value);

  // Write pulse width in microseconds.
  void writeMicroseconds(int value);

  // Returns current pulse width as an angle between 0 and 180 degrees.
  int read();

  // Returns current pulse width in microseconds
  int readMicroseconds();

  // The following attach/detatch functions are no longer necessary
  // but are provided for compatibility with the standard servo linrary.
  void attach(int pin, int minTime = MinPulseTime, int maxTime = MaxPulseTime); 
  void detach() { enable(false); }
  bool attached() { return isEnabled(); } 

  // When using the new interface (i.e. providing pin number at
  // construction and not using attach), you must call begin in setup().
  // If you are using attach, it will do it if necessary.
  static void begin();

  // The interrupt handler is made public so it can be called from
  // SIGNAL. Don't call this otherwise.
  static void interruptHandler();

private:
  friend class ServoCallback;

  // Global linked list of all servos
  static GizmoGardenServo* servoList;
  static GizmoGardenServo* current;
  GizmoGardenServo* next;

  // The sign bit of pin is set for disabled servos. If pin == -1, the
  // servo was constructed with no pin, and so attach must be called.
  // Otherwise, the pin number is pin & 0x7F.
  int8_t pin;

  uint16_t pulseTicks;  // Current pulse duration in timer ticks
  uint16_t pulseEnd;    // Timer count for trailing edge of pulse

  // As is done in the standard servo library, the min and max pulse
  // times are set as an offset from the hard min and max constants.
  // We use int8 to save memory and scale the offset by MinMaxAdjustScale
  // (a shift count) to provide dynamic range.
  int8_t minAdjust;
  int8_t maxAdjust;

  // Initialization utility
  void init(int pin, int minTime, int maxTime);

  // Get and set min and max pulse duration.
  uint16_t minPulse() { return MinPulseTime + (minAdjust << MinMaxAdjustScale); }
  uint16_t maxPulse() { return MaxPulseTime + (maxAdjust << MinMaxAdjustScale); }
  void minPulse(int16_t us) { minAdjust = (int8_t)(us - MinPulseTime >> MinMaxAdjustScale); }
  void maxPulse(int16_t us) { maxAdjust = (int8_t)(us - MaxPulseTime >> MinMaxAdjustScale); }

  // Convert between microseconds and timer ticks, assuming a prescale of 8.
  static uint16_t usToTicks(uint16_t us) { return (uint16_t)(clockCyclesPerMicrosecond() * us / 8); }
  static uint16_t ticksToUs(uint16_t ticks) { return (uint16_t)(ticks * 8L / clockCyclesPerMicrosecond()); }
};

#endif
