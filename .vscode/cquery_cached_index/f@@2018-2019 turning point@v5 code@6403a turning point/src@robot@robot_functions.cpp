#include "main.h"
#include "motors.h"
#include "robot_functions.hpp"

/* ********** Math Functions ********** */

//Calculates the number of ticks it take to move one inch
int inchToTicks (float inch){
  //Define return variable
  int ticks;
  //For every 70 ticks, the robot has moved 1" forward or backward
  const int conversionFactor = 70;
  //Multiply the desiered inch by conversion rate
  ticks = inch * conversionFactor;

  //Return the number of ticks
  return ticks;
}

//Calculates the number of ticks it take to move one inch lateraly
int inchToTickStrafe (float inch){
  //Define return variable
  int ticks;
  //For every 127 ticks, the robot has moved 1" lateraly
  const int conversionFactor = 127;
  //Multiply the desiered inch by conversion rate
  ticks = inch * conversionFactor;

  //Return the number of ticks
  return ticks;
}

//Function to return the sign of a number
int sgn (int input){
  //Define return varialbe
  int output;
  //Takes the input (with a sign + or -) and divide by the
  //absolute value (+) of itselft to get -1 or 1 respectivly
  output = input / (abs(input));

  //Return the sign
  return output;
}


/* ********** Helper Functions ********** */

//Resets the zero position of the encoders to 0
void resetSensors(){
  FrontLeftDrive.set_zero_position(0);
  BackRightDrive.set_zero_position(0);
  FrontRightDrive.set_zero_position(0);
  BackLeftDrive.set_zero_position(0);
  gyro.reset();
}

//Sets the drive motors accordingly
void driveSet (int leftSpeed, int rightSpeed){
  FrontLeftDrive.move(leftSpeed);
  FrontRightDrive.move(rightSpeed);
  BackLeftDrive.move(leftSpeed);
  BackRightDrive.move(rightSpeed);
}

//Returns the sum of the two right wheels
int rightEncoderSum (){
  return fabs(FrontRightDrive.get_position()) + fabs(BackRightDrive.get_position());
}

//Returns the sum of the two left wheels
int leftEncoderSum (){
  return fabs(FrontLeftDrive.get_position()) + fabs(BackLeftDrive.get_position());
}

//Function that returns which autonomous is selected
//based on what jumpers are in which port. This allows
//12 different autons to be used
int getAutonomous(){
  //Initialize variable to be returned
  int myAutonomous;
  //Individualy check for through each ADI port for
  //jumper clips and set myAutonomous to corrisponding autons
  if(A.get_value() == 1 && F.get_value() == 1){myAutonomous = 7;}
  else if(B.get_value() == 1 && F.get_value() == 1){myAutonomous = 8;}
  else if(C.get_value() == 1 && F.get_value() == 1){myAutonomous = 9;}
  else if(D.get_value() == 1 && F.get_value() == 1){myAutonomous = 10;}
  else if(E.get_value() == 1 && F.get_value() == 1){myAutonomous = 11;}
  else if(A.get_value() == 1){myAutonomous = 1;}
  else if(B.get_value() == 1){myAutonomous = 2;}
  else if(C.get_value() == 1){myAutonomous = 3;}
  else if(D.get_value() == 1){myAutonomous = 4;}
  else if(E.get_value() == 1){myAutonomous = 5;}
  else if(F.get_value() == 1){myAutonomous = 6;}
  else{myAutonomous = 12;}
  //Return the variable
  return myAutonomous;
}

//Returns the autonomous name that selected from the
//getAutonomous function
string getAutonomousName(){
  //Initialize variable to be returned
  string myAutonomousName;
  //Check each case for the variable returned from the
  //function and sets a respective name for it
  switch (getAutonomous()){
    case 1: myAutonomousName = "Red Front";
    case 2: myAutonomousName = "Red Back";
    case 3: myAutonomousName = "Empty 3";
    case 4: myAutonomousName = "Empty 4";
    case 5: myAutonomousName = "Empty 5";
    case 6: myAutonomousName = "Skills";
    case 7: myAutonomousName = "Blue Front";
    case 8: myAutonomousName = "Blue Back";
    case 9: myAutonomousName = "Empty 9";
    case 10: myAutonomousName = "Empty 10";
    case 11: myAutonomousName = "Empty 11";
    case 12: myAutonomousName = "Test";
  }
  //Return the variable
  return myAutonomousName;
}

/* ********** Movement Functions ********** */

bool reload = false;
bool autoIntake = false;
//The main medthod for the auto reload control
void reloadPuncher(void*){
  //Start an infinate loop to check for new button press
  while(true){
    //When butotn is pressed start the sequence
    if (master.get_digital(E_CONTROLLER_DIGITAL_L2)){
      //Initialize iteration variable
      int i = 0;

      //Runs the puncher until the current draw dips and
      //until the 120th iteration to prevent the loop ending
      //when the current draw is low due to motor starting to spin
      while(true){
        //Moves puncher back and reset encoder position in
        //preperation for next procedure
        Puncher.move(127);
        Puncher.tare_position();

        //Adds one to iteration counter
        i++;

        //Checks if needs to break out of loop
        if (Puncher.get_current_draw() < 300 && i > 120)
        break;

        ///Dont wanna stress out the poor brain dont we
        delay(2);
      }

      //Cock backs the puncher until it is about to shoot
      //again, so the puncher is ready to fire instantly
      while (true){
        //Moves puncher back
        Puncher.move(127);

        //Checks if needs to break out of loop
        if (fabs(Puncher.get_position()) > 825)
        break;

        //Dont wanna stress out the poor brain dont we
        delay(2);
      }
      //Stops puncher from moving once it is out of the
      //auto reload procedure
      Puncher.move(0);
    }

    //Dont wanna stress out the poor brain do we
    delay(20);
  }
}

//The main medthod for the auto reload control
void reloadPuncherAuton(void*){
  //Start an infinate loop to check for new button press
  while(true){
    //When butotn is pressed start the sequence
    if (reload){
      //Initialize iteration variable
      int i = 0;

      //Checks if the intake was running and turns it on
      if (autoIntake){
        Intake.move(127);
      }

      //Runs the puncher until the current draw dips and
      //until the 120th iteration to prevent the loop ending
      //when the current draw is low due to motor starting to spin
      while(true){
        //Moves puncher back and reset encoder position in
        //preperation for next procedure
        Puncher.move(127);
        Puncher.tare_position();

        //Checks if needs to break out of loop
        if (Puncher.get_current_draw() < 300 && i > 120)
        break;

        //Adds one to iteration counter
        i++;

        ///Dont wanna stress out the poor brain dont we
        delay(2);
      }

      //Cock backs the puncher until it is about to shoot
      //again, so the puncher is ready to fire instantly
      while (true){
        //Moves puncher back
        Puncher.move(127);

        //Checks if needs to break out of loop
        if (fabs(Puncher.get_position()) > 825)
        break;

        //Dont wanna stress out the poor brain dont we
        delay(2);
      }
      //Stops puncher from moving once it is out of the
      //auto reload procedure
      Puncher.move(0);

      //Checks if the intake was running and turns it off
      if (autoIntake){
        Intake.move(0);
      }

      //Reset reload variable
      reload = false;
    }

    //Dont wanna stress out the poor brain do we
    delay(20);
  }
}

//Shoot puncher method that make it easy to set
//the variables for the auto reload task
void shootPuncher (bool intake){
  reload = true;
  autoIntake = intake;
}

//Proportional lift control that holds the lift where
//it was last left at. Uses the lift target variable to control
int liftTarget = 0;
bool toggleFlip = true;
void liftControl (void*){
  // Proportion lift control variables
  int liftError = 0;
  int liftPower = 0;
  float lift_Kp = 0.5;

  // Proportion flipper hold control variables
  int flipError = 0;
  int flipTarget = 30;
  float flip_Kp = 0.5;

  //Infite loop to constantly check if lift needs to be moved
  while(true){
    //Sets lift error to tartget value (set from moving the lift,
    //but by defult is 0) minus its current position
    liftError = liftTarget - Lift.get_position();

    //If error is nothing stops moving lift to prevent unnecisarry
    //movement of the lift
    if (abs(liftError) == 0){
      liftError = 0;
    }

    //Sets lift adjustment power to the error times the constant
    //of proportion
    liftPower = liftError * lift_Kp;
    Lift.move(liftPower);

    //Check if the lift is being raised and pitches the
    //flipper at an agle to keep from sliding off
    if (Lift.get_position() > 40 && toggleFlip){
      //Calcuate the flip error from the set target value
      flipError = flipTarget - Flipper.get_position();

      //Move the flipper accordingly
      Flipper.move(flipError * flip_Kp);
    }
    //Turns off flipper when lift is down and flipper is not up
    else if (toggleFlip){
      Flipper.move(0);
    }

    //Dont wanna stress out the poor brain now do we
    delay(100);
  }
}

//Controls the lift through changing the target value
void lift (int target){
  liftTarget = target;
}

void flip (){
  //Initialize variables and reset timers
  int flipperPower = 0;
  Timer FlipperControl = Timer();
  FlipperControl.resetTimer();
  Timer SlueTimer = Timer();
  SlueTimer.resetTimer();

  //Checks if flipper needs to go up
  if (toggleFlip){
    //Uses timer to move up for 500 ms
    while(FlipperControl.getTime() < 500){
      //Moves motor full power
      Flipper.move(127);

      //Dont wanna over load the poor cortex now do we
      delay(10);
    }

    //Holds flipper at the top after flip
    while(flipperPower != 750){
      //Sets the flipper power to slue timer, which
      //slowly ramps up the motor hold power to prevent
      //motor and port burnout from exessive current
      flipperPower = SlueTimer.getTime() * 3;

      //Sets a limit to the flipper power
      if (flipperPower > 750){
        flipperPower = 750;
      }

      //Moves the flipper motor through volate to limit
      //the torque output
      Flipper.move_voltage(flipperPower);

      //Dont wanna over load the poor cortex now do we
      delay(10);
    }
    //Reverses bool value
    toggleFlip = false;
  }

  //Moves the flipper down
  else if (!toggleFlip){
    //Slue flipper movement down to prevent an imediate
    //loss of currents
    while (SlueTimer.getTime() <= 60){
      flipperPower = SlueTimer.getTime() - 60;

      //Sets flipper power limit
      if (SlueTimer.getTime() >= 60){
        flipperPower = 0;
      }

      //Set flipper power to motor
      Flipper.move(flipperPower);

      //Dont wanna over load the poor cortex now do we
      delay(10);
    }
    //Reverses bool value
    toggleFlip = true;
  }
}

//The PID contorl function
void drive (string direction, float target, float waitTime, int maxPower){
  //Constants for Axial Movement
  const float Kp = 0.2;
  const float Kp_C = 1;
  const float Ki = 0.1;
  const float Kd = 0.4;

  //Constants for Turing
  const float Kp_turn = 0.2;
  const float Ki_turn = 0.1;
  const float Kd_turn = 0.4;

  //PID Variables
  float error;
  float proportion;
  float integralRaw;
  float integral;
  float lastError;
  float derivative;

  //Drift Variables
  float error_drift;
  float proportion_drift;

  //Intigral Limits
  const float integralPowerLimit = 20 / Ki;
  const float integralActiveZone = inchToTicks(3);

  //Final Power
  int finalPower;
  const int minPower = 15;

  //Initialize Sensors and Timers
  bool timerLock = true;
  resetSensors();
  Timer endLoopTimer = Timer();
  endLoopTimer.resetTimer();

  //Set minimum wait time
  if (waitTime <= 250){
    waitTime = 250;
  }

  //TESTING
  int maxLeftPower = 0;
  int maxRightPower = 0;

  //Check for axial movement
  if (direction == "north" || direction == "south"){
    //Infinate loop
    while (true){
      //Proportion control
      error = inchToTicks(target) - ((leftEncoderSum() + rightEncoderSum())/4);
      proportion = Kp * error;

      //Drift control
      error_drift = gyro.get_value();
      proportion_drift = Kp_C * error_drift;

      //Integral control
      if (fabs(error) < integralActiveZone && error != 0)
      integralRaw = integralRaw + error;
      else
      integralRaw = 0;

      if (integralRaw > 20)
      integralRaw = 20;
      if (integralRaw < -20)
      integralRaw = -20;

      integral = Ki * integralRaw;

      //Derivative control
      derivative = Kd * (error - lastError);
      lastError = error;
      if (error == 0)
      derivative = 0;

      //Set final power
      finalPower = proportion + integral + derivative;

      if (abs(finalPower) > maxPower){
        finalPower = maxPower;
      }
      else if (abs(finalPower) < 20){
        finalPower = 20;
      }

      //Move motors
      if (direction == "north")
      driveSet(finalPower - proportion_drift, finalPower + proportion_drift);
      else if (direction == "south")
      driveSet(-finalPower - proportion_drift, -finalPower + proportion_drift);

      if ((finalPower - proportion_drift) > maxLeftPower){
        maxLeftPower = finalPower - proportion_drift;
      }
      if ((finalPower + proportion_drift) > maxRightPower){
        maxRightPower = finalPower + proportion_drift;
      }
      //When where is 100 error left (0.3") start the wait timer to end the loop
      if (error < 100){
        timerLock = false;
      }
      if (timerLock){
        endLoopTimer.resetTimer();
      }
      if (endLoopTimer.getTime() >= waitTime){
        break;
      }

      //Dont wanna over load the poor CPU now do we
      delay(20);
    }
  }

  //Check for turning
  else if (direction == "left" || direction == "right"){
    //Infinate loop
    while (true){
      //Proportion control
      error = fabs(target * 10) - fabs(gyro.get_value());
      proportion = Kp_turn * error;

      //Integral control
      if (fabs(error) < integralActiveZone && fabs(error) != 0)
      integralRaw = integralRaw + error;
      else
      integralRaw = 0;

      if (integralRaw > 20)
      integralRaw = 20;
      if (integralRaw < -20)
      integralRaw = -20;

      integral = Ki_turn * integralRaw;

      //Derivative control
      derivative = Kd_turn * (error - lastError);
      lastError = error;
      if (error == 0)
      derivative = 0;

      //Set final power
      finalPower = proportion + integral + derivative;

      if (abs(finalPower) > maxPower){
        finalPower = maxPower;
      }
      else {
        finalPower = sgn(finalPower) * (abs(finalPower) + 15);
      }

      //Move motors
      if (direction == "left")
      driveSet(-finalPower - proportion_drift, finalPower + proportion_drift);
      else if (direction == "right")
      driveSet(finalPower - proportion_drift, -finalPower + proportion_drift);

      //When where is 100 error left (1.0") start the wait timer to end the loop
      if (error < 100){
        timerLock = false;
      }

      if (timerLock){
        endLoopTimer.resetTimer();
      }

      if (endLoopTimer.getTime() >= waitTime){
        break;
      }

      //Dont wanna over load the poor CPU now do we
      delay(20);
    }
  }

  //Checks for lateral movement
  if (direction == "east" || direction == "west"){
    while(true){
      //Proportion control
      error = inchToTickStrafe(target) - ((leftEncoderSum() + rightEncoderSum())/4);
      proportion = Kp * error;

      //Drift control
      error_drift = gyro.get_value();
      proportion_drift = Kp_C * error_drift;

      //Integral control
      if (fabs(error) < integralActiveZone && error > 5)
      integralRaw = integralRaw + error;
      else
      integralRaw = 0;

      if (integralRaw > 20)
      integralRaw = 20;
      if (integralRaw < -20)
      integralRaw = -20;

      integral = Ki * integralRaw;

      //Derivative control
      derivative = Kd * (error - lastError);
      lastError = error;
      if (error == 0)
      derivative = 0;

      //Set final power
      finalPower = proportion + integral + derivative;

      if (abs(finalPower) > maxPower){
        finalPower = maxPower;
      }
      else {
        finalPower = sgn(finalPower) * (abs(finalPower) + 15);
      }

      //Move motors
      driveSet(-finalPower - proportion_drift, -finalPower + proportion_drift);
      if (direction == "west"){
        FrontLeftDrive.move(-finalPower - proportion_drift);
        FrontRightDrive.move(finalPower + proportion_drift);
        BackLeftDrive.move(finalPower - proportion_drift);
        BackRightDrive.move(-finalPower + proportion_drift);
      }
      else if (direction == "east"){
        FrontLeftDrive.move(finalPower - proportion_drift);
        FrontRightDrive.move(-finalPower + proportion_drift);
        BackLeftDrive.move(-finalPower - proportion_drift);
        BackRightDrive.move(finalPower + proportion_drift);
      }

      //When where is 100 error left (1.0") start the wait timer to end the loop
      if (error < 100){
        timerLock = false;
      }
      if (timerLock){
        endLoopTimer.resetTimer();
      }
      if (endLoopTimer.getTime() >= waitTime){
        break;
      }

      //Dont wanna over load the poor CPU now do we
      delay(20);
    }
  }
  //Turn off the drive
  driveSet(0, 0);
}
