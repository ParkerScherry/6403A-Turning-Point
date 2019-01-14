#include "main.h"
#include "robot/motors.h"
#include "robot/robot_functions.hpp"
#include "autonomous/headers/all_autons.h"

void autonomous() {
  //Start tasks to control puncher and lift
  Task reloadPuncherAutonTask (reloadPuncherAuton);
  Task liftTask (liftControl);
  Timer autonTimer = Timer();

  //All autons go here
  switch (getAutonomous()){
    case 1:
      red_front();
      break;
    case 2:
      autonTimer.resetTimer();
      red_back();
      break;
    case 3:
      //Empty 3
      break;
    case 4:
      //Empty 4
      break;
    case 5:
      //Empty 5
      break;
    case 6:
      skills();
      break;
    case 7:
      blue_front();
      break;
    case 8:
      blue_back();
      break;
    case 9:
      //Empty 9
      break;
    case 10:
      //Empty 10
      break;
    case 11:
      //Empty 11
      break;
    case 12:
      test();
      break;
  }
  lcd::set_text(5, to_string(autonTimer.getTime()));
  liftTask.suspend();
  reloadPuncherAutonTask.suspend();
}
