#include "main.h"
#include "../robot/motors.h"
#include "../robot/robot_functions.hpp"
#include "headers/skills.h"

void skills (){
  //drive forward grab ball and flip cap
  Intake.move(70);
  drive(north, 37, 750);
  drive(south, 10, 750);
  drive("left", 194, 750);
  Intake.move(0);
  drive(south, 9, 500);
  flip(true);

  //turn and flip second cap
  drive("right", 78, 750);
  drive(south, 4, 500);
  flip(true);

  //come back and line up for flags
  drive("left", 50, 750);
  drive(north, 43, 750);
  drive("right", 68, 750);
  Intake.move(-80);
  delay(400);
  Intake.move(0);

  //score all three flags
  drive(north, 42, 750);
  shootPuncher();
  delay(300);
  Intake.move(127);
  drive(north, 25, 750);
  shootPuncher();
  delay(300);
  drive(west, 6, 500);
  delay(800);
  Intake.move(127);
  drive(north, 25, 750);

  //back up and flip cap
  drive("left", 5, 500);
  drive(south, 52, 750);
  Intake.move(0);
  drive(east, 5, 750);
  drive("right", 96, 750);

  //goto cap and pick up ball and flip cap

  //score all three flags
  //park

}
