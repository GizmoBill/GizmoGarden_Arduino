/***************************************************************************
Copyright (c) 2016 Bill Silver, Member of Gizmo Garden LLC. This source code
is distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The license
can be found at <http://www.gnu.org/licenses/>. There is no express or
implied warranty, including merchantability or fitness for a particular
purpose.
***************************************************************************/

// *************************************
// *                                   *
// *  Save To and Restore From EEPROM  *
// *                                   *
// *************************************

enum EepromConstants
{
  EepromAddress = 128,
  EepromValid   = 231,
  EepromVersion = 1
};

struct ParadeData
{
  byte       validCode;
  byte       version;
  GizmoGardenDriverData
             driverData;
  byte       batterySmoothness;
  byte       proximitySmoothness;
  int        stopDistance;
  float      springStiffness;
  bool       leadMode;
  byte       volume;
  
  void getData();
  void putData();
};

void ParadeData::getData()
{
  driver.getData(driverData);
  batterySmoothness   = batteryMonitor.getSmoothness();
  proximitySmoothness = proximityMonitor.getSmoothness();
  stopDistance        = ::stopDistance;
  springStiffness     = ::springStiffness;
  leadMode            = ::leadMode;
  volume              = GizmoGardenGetVolume();

  validCode           = EepromValid;
  version             = EepromVersion;
}

void ParadeData::putData()
{
  if (validCode == EepromValid)
  {
    switch (version)
    {
      // Newer versions go here

      case 1:
        driver.setData(driverData);
        batteryMonitor.setSmoothness(batterySmoothness);
        proximityMonitor.setSmoothness(proximitySmoothness);
        ::stopDistance    = stopDistance;
        ::springStiffness = springStiffness;
        ::leadMode        = leadMode;
        GizmoGardenSetVolume(volume);
        break;
    }
  }  
}

void restoreData()
{
  ParadeData saved;
  EEPROM.get(EepromAddress, saved);
  saved.putData();
}

void saveData()
{
  ParadeData current;
  current.getData();
  EEPROM.put(EepromAddress, current);
}

bool isDataSaved()
{
  ParadeData current, saved;
  current.getData();
  EEPROM.get(EepromAddress, saved);
  return objectsEqual(current, saved);
}

void saveEventFunction(byte eventCode)
{
  switch (eventCode)
  {
    case GizmoSetup:
      restoreData();
      break;
  }
}

GizmoGardenEventRegistry saveEvent(saveEventFunction);

// *************************
// *                       *
// *  Save Data to EEPROM  *
// *                       *
// *************************

MakeGizmoGardenMenuItem(Save)
{
  switch (event)
  {
    case Select:
      saveData();
      lcd.clear();

    case Enter:
      if (!isDataSaved())
        lcd.print(F("Not "));
      lcd.print(F("Saved"));
      break;
  }
}

