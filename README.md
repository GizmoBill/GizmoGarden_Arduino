This repository contains the Gizmo Garden Arduino libraries, which comprise the custom operating system used by our self-driving parade floats. It is available free of charge under the GNU General Public Licens. It may have many additional uses for Arduino-based projects, particularly for the Uno and for Adafruit accessories. To install, download and place these directories in the Arduino libraries directory.

* Powerful cooperative multitasking, very easy to use.
* Extensible menu system using Adafruit LCD shields.
  * Monitor changing values, examine and set parameters, take actions.
  * Easy to add menu items.
  * Runs with with the multitasking library.
* Jitter-free servo control, Adafruit NeoPixel compatible
  * Control up to a dozen servos on any pins.
  * Interrupt-driven, runs in the background.
  * Eliminates the jitter present with the standard Arduino servo library that is caused by interrupt latency.
  * Control hundreds of Adafruit NeoPixels with no servo interference.
  * Drop-in replacement for the Arduino servo library.
* Many classes of general utility in the Arduino environment.
* Libraries that run with the multitasking library:
  * Play music on a piezo buzzer by writing text strings using conventional musical terminology.
  * Indicate small integer values by flashing a single LED.
  * Steering control for self-driving vehicles
