#include "main.h"
#include "../robot/motors.h"
#include "../robot/robot_functions.hpp"
#include "headers/blue_back.h"

void blue_back (){
  //wait 5 seconds and shoot ball
  delay(7000);
  shootPuncher();
  delay(400);

  //drive and pick up ball
  drive("left", 50, 250);
  Intake.move(127);
  drive(north, 38, 250);

  //turn and flip cap
  drive("right", 120, 250);
  drive(south, 12, 250);
  flip(true);

  //park
  drive("left", 22, 250);
  drive(east, 2, 500);
  drive(north, 49, 500);


}
