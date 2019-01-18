#include "main.h"
#include "../robot/motors.h"
#include "../robot/robot_functions.hpp"
#include "headers/blue_front.h"

void blue_front (){
  //collect second ball from under cap
  Intake.move(70);
  drive(north, 37, 500);
  Intake.move(0);
  drive(south, 35, 500);
  drive("right", 96, 500);

  //shoot high and middle flag
  shootPuncher();
  delay(500);
  Intake.move(127);
  drive (north, 27, 500);
  shootPuncher();
  delay(200);

  //toggle low flag and collect rebound
  drive(east, 3, 500);
  drive(north, 28, 250);
  changeBool(false);

  //Move to middle flags
  drive(south, 13, 250);
  drive(west, 16, 250);
  changeBool(true);
  //drive(south, 2, 10);
  flip(true);

  //shoot and toggle middle middle and low flag
  drive(west, 17, 250);
  drive(north, 12, 10);
  drive(south, 22, 10);
  drive("left", 23, 250);
  shootPuncher();
  delay(250);
}
