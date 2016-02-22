 /***************************************************************************
Copyright (c) 2016 Bill Silver, Member of Gizmo Garden LLC. This source code
is distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The license
can be found at <http://www.gnu.org/licenses/>. There is no express or
implied warranty, including merchantability or fitness for a particular
purpose.
***************************************************************************/

// ***********************
// *                     *
// *  Include Libraries  *
// *                     *
// ***********************

// Arduino and Adafruit libraries
#include <EEPROM.h>

#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <Adafruit_NeoPixel_GizmoGardenModified.h>

#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

#include <avr/pgmspace.h>

// Gizmo Garden libraries
#include <GizmoGardenCommon.h>
#include <GizmoGardenServo.h>
#include <GizmoGardenTone.h>
#include <GizmoGardenMultitasking.h>
#include <GizmoGardenLCDPrintTask.h>
#include <GizmoGardenMenus.h>
#include <GizmoGardenMotors.h>
#include <GizmoGardenDriver.h>
#include <GizmoGardenMusicPlayer.h>
#include <GizmoGardenPixels.h>
#include <GizmoGardenColorMixer.h>
#include <GizmoGardenMotorMenus.h>

typedef ProgSpacePointer<NeoPixelColor> ProgColors;

// ************************************
// *                                  *
// *  Global Definitions and Objects  *
// *                                  *
// ************************************

enum DigitalPins
{
  MusicPin1     =  4,
  MusicPin2     =  5,
  
  LeftWheelPin  = 12,
  RightWheelPin = 13,
};

enum AnalogPins
{
  BatterySensorPin   = 0,
  ProximitySensorPin = 1,
  
  LeftSensorPin      = 3,
  RightSensorPin     = 2,
  
  // 4 & 5 reserved for LCD
};

enum EventCodes
{
  GizmoSetup,
  DriveStart,
  DriveStop,
};

// An Adafruit liquid crystal display is used to make a dashboard that shows useful
// information while driving and allows us to control the vehicle with a menu system.
// Here we create a GizmoGardenLCDPrint object called "lcd" that allows us to print
// on the display as a separate task so we don't hog the computer while printing.
GizmoGardenLCDPrint lcd;

// Create motors for each wheel, and then a GizmoGardenDriver that uses those
// wheels to steer.
GizmoGardenRotatingMotor leftWheel (LeftWheelPin ,  100);
GizmoGardenRotatingMotor rightWheel(RightWheelPin, -100);
GizmoGardenDriver driver(leftWheel, rightWheel, LeftSensorPin, RightSensorPin);

// Create a GizmoGardenSmoother to smooth out the signal from the proximity sensor
GizmoGardenSmoother proximityMonitor(4);
float readProximitySensor()
{
  return proximityMonitor.input(analogRead(ProximitySensorPin));
}


// *****************
// *               *
// *  Menu System  *
// *               *
// *****************

GizmoGardenMenuTask menuTask(lcd);

// **********************************
// *                                *
// *  Fuel Guage (Battery Voltage)  *
// *                                *
// **********************************

GizmoGardenSmoother batteryMonitor(9);

MakeGizmoGardenMenuItem(Battery)
{
  float volts;
  
  switch(event)
  {
    case Enter:
      printName();
      batteryMonitor.reset();
      break;

    case ChangeAndShow:
      batteryMonitor.setSmoothness(batteryMonitor.getSmoothness() + direction);
      lcd.setCursor(14, 1);
      ggPrint(lcd, batteryMonitor.getSmoothness(), 2);
      break;

    case Monitor:
      batteryMonitor.input(analogRead(BatterySensorPin));
      volts = batteryMonitor.getOutput() / 102.3 + 0.8;
      ggPrint(lcd, volts, 4, 2);
      lcd.print('V');    
      break;
  }
}

// ********************
// *                  *
// *  Volume Control  *
// *                  *
// ********************

MakeGizmoGardenMenuItem(Volume)
{
  switch(event)
  {
    case Enter:
      printName();
      break;
      
    case ChangeAndShow:
      GizmoGardenSetVolume(GizmoGardenGetVolume() + direction);
      lcd.print(GizmoGardenGetVolume());
      break;
  }
}

// ***********
// *         *
// *  Setup  *
// *         *
// ***********
//
// Initialize everything

void setup()
{
  GizmoGardenTask::begin();
  lcd.begin();
  GizmoGardenServo::begin();
  GizmoGardenToneBegin(MusicPin1, MusicPin2);

  welcome();

  GizmoGardenEventRegistry::raiseEvent(GizmoSetup);

  menuTask.choose(Battery);
  menuTask.start();
}

// **********
// *        *
// *  Loop  *
// *        *
// **********

void loop()
{
  GizmoGardenTask::run();
}

// ------1---------2---------3---------4---------5---------6---------7------

