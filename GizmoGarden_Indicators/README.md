Gizmo Garden library for controlling a digital output pin, presumably wired to an LED, so as to indicate to a human observer an integer value in the range -10 to +10. The value n is indicated by continuously repeating n short flashes and a longer pause. If n is positive, the flashes are HIGH and the pause is LOW. If n is negative, the flashes are LOW and the pause is HIGH. If n is 0, the pin is held LOW. If n is -128, the pin is held HIGH.

The indicator is a task running with the Gizmo Garden multitasking system and so it runs cooperatively with other tasks.

Requires GizmoGarden_Common and GizmoGarden_Multitasking.
