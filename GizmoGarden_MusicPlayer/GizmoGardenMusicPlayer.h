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
  GizmoGardenMusicPlayer(int beatLength, bool allowMenuStartStop = false);

  int getBeatLength() const { return beatLength; }
  void setBeatLength(int bl) { beatLength = bl; }

  // Load specified music so that it will play when the task is started. This
  // must be done before each start, since the music is not remembered.
  void loadMusic(GizmoGardenText pitches, GizmoGardenText durations);

  // Load and start
  void play(GizmoGardenText pitches, GizmoGardenText durations);

  DECLARE_TASK_NAME

  enum EndCodes
  {
    NormalEnd,
    ForcedEnd,
    PitchError,
    DurationError
  };

  uint8_t getEndCode() const { return endCode; }
  bool normalEnd() const { return endCode == NormalEnd; }

  GizmoGardenText getStatus() const;
  
private:
  GizmoGardenText nextPitch;
  GizmoGardenText nextDuration;
  int beatLength;

  int8_t nextPitchIndex;
  int8_t nextWhiteIndex;

  uint8_t endCode;

  void getPitchIndex();

protected:
  virtual void myTurn();
  virtual void customNote(int pitchIndex, int whiteIndex);
};

#define MakeMusicPlayer(name, beatLength)                     \
class Class##name : public GizmoGardenMusicPlayer             \
{                                                             \
public:                                                       \
  Class##name() : GizmoGardenMusicPlayer(beatLength, true) {} \
  CUSTOM_START                                                \
  CUSTOM_STOP                                                 \
} name

#define MakeCustomMusicPlayer(name, beatLength)               \
class Class##name : public GizmoGardenMusicPlayer             \
{                                                             \
public:                                                       \
  Class##name() : GizmoGardenMusicPlayer(beatLength, true) {} \
  CUSTOM_START                                                \
  CUSTOM_STOP                                                 \
  virtual void customNote(int pitchIndex, int whiteIndex);    \
} name;                                                       \
void Class##name::customNote(int pitchIndex, int whiteIndex)

#endif
