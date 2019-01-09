#include "main.h"
#include "../robot/motors.h"
#include "../robot/robot_functions.hpp"
#include "headers/test.h"

void test (){
  drive("left", 90, 500);
  drive("right", 45, 500);
}
