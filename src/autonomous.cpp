#include "main.h"
#include "robot/motors.h"
#include "robot/robot_functions.hpp"
#include "autonomous/headers/all_autons.h"

void autonomous() {
  //Start tasks to control puncher and lift
  Task reloadPuncherAutonTask (reloadPuncherAuton);
  Task liftTask (liftControl);

  //All autons go here
  switch (getAutonomous()){
    case 1:
      red_front();
    case 2:
      red_back();
    case 3:
      //Empty 3
    case 4:
      //Empty 4
    case 5:
      //Empty 5
    case 6:
      skills();
    case 7:
      blue_front();
    case 8:
      blue_back();
    case 9:
      //Empty 9
    case 10:
      //Empty 10
    case 11:
      //Empty 11
    case 12:
      test();
  }
  liftTask.suspend();
  reloadPuncherAutonTask.suspend();
}
