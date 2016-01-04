/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include "../GizmoGarden_Tone/GizmoGardenTone.h"
#include "GizmoGardenMusicPlayer.h"

GizmoGardenMusicPlayer::GizmoGardenMusicPlayer(int beatLength, bool allowMenuStartStop)
  : beatLength(beatLength), GizmoGardenTask(allowMenuStartStop)
{
}

#ifdef TASK_MONITOR
GizmoGardenText GizmoGardenMusicPlayer::name() const
{
  return F("Music"); 
}
#endif

void GizmoGardenMusicPlayer::loadMusic(GizmoGardenText pitches,
                                       GizmoGardenText durations)
{
  nextPitch = pitches;
  nextDuration = durations;
  endCode = ForcedEnd;
  getPitchIndex();
}

void GizmoGardenMusicPlayer::play(GizmoGardenText pitches,
                                  GizmoGardenText durations)
{
  loadMusic(pitches, durations);
  start();
}

enum PitchCodes
{
  RestPitch  = -1,
  EndPitch   = -2,
  ErrorPitch = -3
};

void GizmoGardenMusicPlayer::getPitchIndex()
{
  //                                A   B  C  D  E  F  G
  static const char noteTable[] = { 9, 11, 0, 2, 4, 5, 7 };
  char c;
  do
  {
    c = *nextPitch++;
  }
  while (c == ' ');

  if (c == '-')
  {
    nextPitchIndex = nextWhiteIndex = RestPitch;
    return;
  }

  uint8_t note = c - 'A';
  uint8_t octive = *nextPitch++ - '5';
  if (note >= 7 || octive >= 3)
  {
    --nextPitch;
    nextPitchIndex = c == 0 ? EndPitch : ErrorPitch;
    return;
  }

  note = noteTable[note];
  //note = pgm_read_byte(&noteTable[note]);
  nextWhiteIndex = 7 * octive + ((note + 1) >> 1);
  note += 12 * octive;

  c = *nextPitch;
  if (c == '#')
  {
    ++note;
    ++nextPitch;
  }
  else if (c == 'b')
  {
    --note;
    ++nextPitch;
  }

  nextPitchIndex = note;
}

const int pitchTable[] PROGMEM =
{
   523,  554,  587,  622,  659,  698,  740,  784,  831,  880,  932,  988,
  1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
};

void GizmoGardenMusicPlayer::myTurn()
{
  int8_t currentPitchIndex = nextPitchIndex;
  int8_t currentWhiteIndex = nextWhiteIndex;

  if (currentPitchIndex == EndPitch)
  {
    endCode = NormalEnd;
    return;
  }

  if (currentPitchIndex == ErrorPitch)
  {
    endCode = PitchError;
    return;
  }

  uint16_t duration = getMusicTime(nextDuration, beatLength);
  if (duration == 0)
  {
    endCode = DurationError;
    return;
  }

  getPitchIndex();

  // zero pitch is a rest, chill out and do nothing if the pitch is a rest
  if (currentPitchIndex >= 0)
  {
    // If sequential pitches are identical, shorten the first one to separate them
    // as distinct notes.
    uint16_t d = duration;
    if (nextPitchIndex == currentPitchIndex)
      d -= d >> 2;
    GizmoGardenTone(pgm_read_word(&pitchTable[currentPitchIndex]), d);
  }

  // hold off on playing any more notes until the current note is done playing
  callMe(duration);
  customNote(currentPitchIndex, currentWhiteIndex);
}

void GizmoGardenMusicPlayer::customNote(int, int)
{
}

GizmoGardenText GizmoGardenMusicPlayer::getStatus() const
{
  if (isRunning())
    return F("Running");

  switch (getEndCode())
  {
  case NormalEnd:
  case ForcedEnd:
  default:
    return F("Stopped");

  case PitchError:
    return nextPitch - 1;

  case DurationError:
    return nextDuration;
  }
}
