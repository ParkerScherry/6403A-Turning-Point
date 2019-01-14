#include "main.h"
#include "../robot/motors.h"
#include "../robot/robot_functions.hpp"
#include "headers/test.h"

void test (){
  deployFlip();
  lift(500, 127);
  delay(10000);
}
