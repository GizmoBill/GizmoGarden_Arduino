// **********************
// *                    *
// *  Gizmo Playground  *
// *                    *
// **********************

// *********************
// *                   *
// *  Welcome Message  *
// *                   *
// *********************

/*
 * The Gizmo Garden dashboard is a liquid crystal display (LCD) containing 2 rows of 16
 * characters each. To print a welcome message on the dashboard, you decide:
 * 
 *          1) What characters to print.
 *          2) Where to print them.
 *          3) How long the message will stay up before the menu takes over.
 * 
 * The 2 rows and 16 columns are numbered like this:
 * 
 *                                     Column
 *          0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
 *        -----------------------------------------------------------------
 *     0  |   |   |   |   | W | e | l | c | o | m | e |   |   |   |   |   |
 *  R     |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
 *  O      ----------------------------------------------------------------
 *  W  1  |   |   | G | i | z | m | o |   | G | a | r | d | e | n |   |   |
 *        |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
 *        -----------------------------------------------------------------
 */

void welcome()
{
  lcd.setCursor(3, 0);
  lcd.print(F("Welcome to"));
  lcd.setCursor(2, 1);
  lcd.print(F("Gizmo Garden"));
  delay(2000);
}

// ****************
// *              *
// *  Beep Party  *
// *              *
// ****************

void beepParty()
{
}

// *****************
// *               *
// *  Dance Party  *
// *               *
// *****************

void danceParty()
{
  leftWheel.setSpeed(75);
  rightWheel.setSpeed(75);
  delay(250);
  
  leftWheel.setSpeed(0);
  rightWheel.setSpeed(0);
}

// ***************************
// *                         *
// *  Playground Menu Items  *
// *                         *
// ***************************

MakeGizmoGardenMenuItem(BeepParty)
{
  switch(event)
  {
    case Enter:
      printName();
      break;

    case Select:
      beepParty();
      break;
  }
}

MakeGizmoGardenMenuItem(DanceParty)
{
  switch(event)
  {
    case Enter:
      printName();
      break;

    case Select:
      danceParty();
      break;
  }
}

