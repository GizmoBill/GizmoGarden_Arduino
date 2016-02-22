/***************************************************************************
Copyright (c) 2016 Bill Silver, Member of Gizmo Garden LLC. This source code
is distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The license
can be found at <http://www.gnu.org/licenses/>. There is no express or
implied warranty, including merchantability or fitness for a particular
purpose.
***************************************************************************/

// ******************
// *                *
// *  Simple Music  *
// *                *
// ******************

// This code can be used to play music using the piezo buzzer. You create
// the melody you want to play by writing text to specify individual notes.
// The meoldy will be played in a continuous loop, with an optional pause
// before the next one starts.

// You create two text strings to specify the notes, one for pitch and one
// for time. Each note in the pitch string is a sequence of two or three
// characters:
//      <letter><octave><sharp/flat>
//          <letter>     is A-G (upper case)
//          <octave>     is 5, 6, or 7
//          <sharp/flat> is an optional # or b.
//
// Following conventional music notation, each octive is CDEFGAB, so that
// for example C6 follows B5. Spaces are ignored, except that they cannot
// separate the characters of one note. Octaves lower than 5 are not
// provided because the pitches are too low for piezo buzzers. The
// characters of a note must be in the order shown above.
//
// To specify a rest in the pitch string, use a - (dash) in place of the
// <letter><octave><sharp/flat> characters.
//
// The time string uses these characters:
//    S   sixteenth
//    E   eighth
//    Q   quarter
//    H   half
//    W   whole
//    T   triplet (1/3 of a quarter note)
//
// All notes except T can be followed by a dot (.) to extend the time by
// half. Spaces are ignored, except that they cannot appear between a note
// and the dot.
//
// The tempo of the music is determined by the length in milliseconds of a
// quarter note, which is called the BeatLength.
//
// The music will stop if any text is found that does not follow the above
// rules. If this happens, the dashboard menu MusicStatus can be used to
// figure out what went wrong. This menu shows on the dashboard up to 14
// characters starting at the place the error was found, to help you find
// the error.


// **************
// *            *
// *  Settings  *
// *            *
// **************

// Here is an example melody. To make it easier to read, spaces are used to
// separate phrases and line up the pitches and times.

// 1) Change the text inside the quotes to the melody you want.
//    Test your melody using the MusicStatus menu. The Select button will
//    start and stop the melody loop. If an error is found, the music will
//    stop and the dashboard will show the text starting at the error.
#define Pitches "C5D5E5F5G5A5B5C6D6E6F6G6A6B6A6G6F6E6D6C6B5A5G5F5E5D5"
#define Times   "Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q Q"

// 2) BeatLength is the length of a quarter note in milliseconds. You can
//    change it to speed up or slow down the tempo.
#define BeatLength 200

// 3) The music plays in a loop, pausing by the following time in
//    milliseconds before starting the next play.
#define RepeatAfterPause 0

// 4) Once you are done setting everything up, you can set this to false
//    to remove the MusicStatus menu from the dashboard to make it easier
//    to find what you really want. You can always get the menu back later
//    by setting it back to true.
#define IncludeMenus true

// ==========================================================================

// *****************
// *               *
// *  Source Code  *
// *               *
// *****************

namespace SimpleMusic {

// Make text strings that the code can use. GizmoGardenText behaves like a
// normal C++ string, but saves memory by not coping the string to dynamic
// memory.
MakeGizmoGardenText(pitches, Pitches);
MakeGizmoGardenText(times  , Times  );

// Create a GizmoGardenMusicPlayer object called Player that can be
// used to play a melody on the piezo buzzer. Player is a task that
// plays music simultaneously with all other tasks that may be running. The
// beatLength (length of time of a quarter note) is set as sepcified above.
MakeMusicPlayer(Player, BeatLength);

// When the Player task starts, tell it what music to play.
CustomStart(Player)
{
  loadMusic(pitches, times);
}

// The Player task stops when one of three things happens:
//   * The end of the melody is reached.
//   * An error in the text is found.
//   * Other code forces it to stop by doing "Player.stop();"
// If Player has stopped by reaching the end of the melody, start
// it again to make a continuous loop.
CustomStop(Player)
{
  if (normalEnd())
    start(RepeatAfterPause);
}

void eventFunction(byte eventCode)
{
  switch (eventCode)
  {
    case GizmoSetup:
    case DriveStart:
      Player.start();
      break;
      
    case DriveStop:
      Player.stop();
      break;
  }
}

GizmoGardenEventRegistry event(eventFunction);

#if IncludeMenus
// Here is the MusicStatus menu.
MakeGizmoGardenMenuItem(MusicStatus)
{
  switch (event)
  {
    case Enter:
      printName();
      break;

    case Monitor:
      ggPrint(lcd, Player.getStatus(), 14);
      break;

    case Select:
      if (Player.isRunning())
        Player.stop();
      else
        Player.start();
      break;
  }
}
#endif

};

#undef Pitchs
#undef Times
#undef BeatLength
#undef RepeatAfterPause
#undef IncludeMenus

