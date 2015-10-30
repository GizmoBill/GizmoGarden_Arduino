#ifndef _GizmoGardenTone_
#define _GizmoGardenTone_

/********************************************************************
Copyright (c) 2015 Bill Silver (gizmogarden.org). This source code is
distributed under terms of the GNU General Public License, Version 3,
which grants certain rights to copy, modify, and redistribute. The
license can be found at <http://www.gnu.org/licenses/>. There is no
express or implied warranty, including merchantability or fitness for
a particular purpose.
********************************************************************/

#include <inttypes.h>

// Call this in setup() to initialize.
void GizmoGardenToneBegin(int pin1, int pin2);

// Play a tone at the indicated frequency for the indicated duration. Any currently
// playing tone is first stopped. If frequency is 0, stop any playing tone and
// do nothing else.
void GizmoGardenTone(uint16_t frequency, uint16_t duration = 0xFFFFU);

// The volume is 0 (off), 1 (half), or 2 (full). It can be changed at any time
// and takes effect immediatly, even while a tone is playing.
int  GizmoGardenGetVolume();
void GizmoGardenSetVolume(int volume);

#endif
