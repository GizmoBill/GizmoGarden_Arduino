Gizmo Garden library provides powerful cooperative multitasking, simple to use. See the examples for a good how-to guide.

If you are using the GizmoGarden_Menus library and you don't mind using a little more memory, uncomment the line "#define TASK_MONITOR" in GizmoGardenMultitasking.h. This will add a task monitor menu item to your menu. This item allows you to cycle through every task in the program with UP/DOWN, showing whether the task is currently running, and how long it is taking to execute each turn. You can start or stop a task from the menu item by clicking SELECT (although some tasks do not allow the menu to stop them).

Requires GizmoGarden_Common.
