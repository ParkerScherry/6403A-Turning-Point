#include "main.h"
#include "../robot/motors.h"
#include "../robot/robot_functions.hpp"
#include "headers/blue_front.h"

void blue_front (){
  //collect second ball from under cap
  Intake.move(70);
  drive(north, 37, 500);
  Intake.move(0);
  drive(south, 34, 500);
  drive("right", 95, 500);

  //shoot high and middle flag
  shootPuncher(true);
  delay(500);
  drive (north, 27, 500);
  shootPuncher();
  delay(200);

  //toggle low flag and collect rebound
  drive(east, 6, 500);
  Intake.move(127);
  drive(north, 28, 250);

  //Move to middle flags
  drive(south, 15, 250);
  drive(west, 19, 250);
  drive(south, 2, 10);
  flip(true);

  //shoot and toggle middle middle and low flag
  drive(west, 17, 250);
  drive(north, 12, 10);
  drive(south, 22, 10);
  drive("left", 20, 250);
  // Intake.move(-127);
  // delay(200);
  // Intake.move(0);
  shootPuncher();
  delay(250);
}
