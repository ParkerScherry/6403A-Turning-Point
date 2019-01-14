#include "main.h"
#include "../robot/motors.h"
#include "../robot/robot_functions.hpp"
#include "headers/red_back.h"

void red_back (){
  flip(true);
  drive(south, 31, 500);
  holdFlip(true);
  lift(70, 60);
  delay(250);
  drive(north, 28, 500);
  lift(650, 80);
  drive("right", 97, 500);
  drive(south, 23, 500);
  holdFlip(false);
  lift(530, -20);
}
