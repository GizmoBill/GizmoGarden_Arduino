#ifndef _GizmoGardenMusicPlayer_
#define _GizmoGardenMusicPlayer_

/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "../GizmoGarden_Common/GizmoGardenCommon.h"
#include "../GizmoGarden_Multitasking/GizmoGardenMultitasking.h"

// ******************
// *                *
// *  Music Player  *
// *                *
// ******************

// GizmoGardenMusicPlayer is a task that plays music using
// GizmoGardenTone. Notes are specified as two GizmoGardenText strings,
// one for pitch and one for duration. The duration string uses
// the getMusicTime function described in GizmoGardenCommon.h. The
// pitch string is a sequence of <note><octave><sharp/flat>
// characters, where <note> is A-G (upper case), <octave> is 5-7,
// and <sharp/flat> is an optional # or b. Following conventional
// music notation, each octive is CDEFGAB, so that for example
// C6 follows B5. Spaces are ignored, except that they cannot separate
// the characters of one note. Octaves lower than 5 are not
// implemented because the pitches are too low for piezo buzzers.
//
// A - (dash) can be used is place of <note><octave> characters to
// signify a rest.
//
// One can derive a class from GizmoGardenMusicPlayer that makes
// use of currentNote and currentWhiteNote to do some related
// action, synchronized to the music.

class GizmoGardenMusicPlayer : public GizmoGardenTask
{
public:
  GizmoGardenMusicPlayer(int beatLength);

  void play(GizmoGardenText pitches, GizmoGardenText durations);

  char currentNote() const { return currentPitchIndex; }
  char currentWhiteNote() const { return currentWhiteIndex; }

  DECLARE_TASK_NAME

private:
  GizmoGardenText nextPitch;
  GizmoGardenText nextDuration;
  int beatLength;

  int8_t currentPitchIndex;
  int8_t nextPitchIndex;

  int8_t currentWhiteIndex;
  int8_t nextWhiteIndex;

  void getPitchIndex();

protected:
  virtual void myTurn();
};

#define MakeCustomMusic(name, beatLength)             \
class name##Class : public GizmoGardenMusicPlayer     \
{                                                     \
public:                                               \
  name##Class(int bl) : GizmoGardenMusicPlayer(bl) {} \
protected:                                            \
  virtual void myTurn();                              \
} name(beatLength);                                   \
void name##Class::myTurn()

#endif
