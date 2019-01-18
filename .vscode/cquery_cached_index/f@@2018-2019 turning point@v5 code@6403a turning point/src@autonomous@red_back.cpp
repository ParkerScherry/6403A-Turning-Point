#include "main.h"
#include "../robot/motors.h"
#include "../robot/robot_functions.hpp"
#include "headers/red_back.h"

void red_back (){
  //wait 5 seconds and shoot ball
  delay(1);
  shootPuncher();
  delay(400);

  //drive and pick up ball
  drive("right", 50, 250);
  Intake.move(80);
  drive(north, 39, 250);

  //turn and flip cap
  drive("left", 120, 250);
  drive(south, 12, 250);
  flip(true);

  //park
  drive("right", 22, 250);
  drive(west, 5, 500);
  drive(north, 52, 500);

}
