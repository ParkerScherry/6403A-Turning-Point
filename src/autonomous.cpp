#include "main.h"

extern Motor FrontLeftDrive;
extern Motor FrontRightDrive;
extern Motor BackLeftDrive;
extern Motor BackRightDrive;
extern Motor Lift;
extern Motor Intake;
extern Motor Puncher;
extern Motor Flipper;
extern ADIGyro gyro;
extern ADIDigitalIn A;
extern ADIDigitalIn C;
extern ADIDigitalIn D;
extern ADIDigitalIn E;
extern ADIDigitalIn F;
extern ADIDigitalIn G;

extern int getAutonomous();


class Timer{
private:
  int zeroTime = millis();

public:
  int getTime (){
    int currentTime = millis();
    int timerCount = currentTime - zeroTime;
    return timerCount;
  }
  void resetTimer (){
    zeroTime = millis();
  }
};

/**
* Runs the user autonomous code. This function will be started in its own task
* with the default priority and stack size whenever the robot is enabled via
* the Field Management System or the VEX Competition Switch in the autonomous
* mode. Alternatively, this function may be called in initialize or opcontrol
* for non-competition testing purposes.
*
* If the robot is disabled or communications is lost, the autonomous task
* will be stopped. Re-enabling the robot will restart the task, not re-start it
* from where it left off.
*/

int inchToTicks (float inch){
  int ticks;
  const int conversionFactor = 72;
  ticks = inch * conversionFactor * 4;

  return ticks;
}

int inchToTickStrafe (float inch){
  int ticks;
  const int conversionFactor = 127;
  ticks = inch * conversionFactor * 4;

  return ticks;
}

int sgn (int input){
  int output;
  output = input / (abs(input));
  return output;
}

int degreesToTicks (float degree){
  const float ticksPerTurn = 15.64;
  int ticks = degree * ticksPerTurn * 4;

  return ticks;
}

//Converts percents to corrisponding motor power
int percentToMotorValue (int percent){
  int motorValue;
  percent = percent / 100;
  motorValue = percent * 127;
  return motorValue;
}

const std::string north = "north";
const std::string south = "south";
const std::string east = "east";
const std::string west = "west";
const std::string left = "left";
const std::string right = "right";
bool reload = false;
bool autoIntake = false;

void resetSensors(){
  FrontLeftDrive.set_zero_position(0);
  BackRightDrive.set_zero_position(0);
  FrontRightDrive.set_zero_position(0);
  BackLeftDrive.set_zero_position(0);
  gyro.reset();
}


void driveSet (int leftSpeed, int rightSpeed){
  FrontLeftDrive.move(leftSpeed);
  FrontRightDrive.move(rightSpeed);
  BackLeftDrive.move(leftSpeed);
  BackRightDrive.move(rightSpeed);
}

int rightEncoderSum (){
  return fabs(FrontRightDrive.get_position()) + fabs(BackRightDrive.get_position());
}

int leftEncoderSum (){
  return fabs(FrontLeftDrive.get_position()) + fabs(BackLeftDrive.get_position());
}

void reloadPuncherAuton(void*){
  while(true){
    if (reload){
      int i = 0;
      int ii = 0;
      if (autoIntake){
        Intake.move(127);
      }
      while(true){
        Puncher.move(127);
        delay(2);
        Puncher.tare_position();
        if (Puncher.get_current_draw() < 300 && i > 120){
          break;
        }
        i++;
      }
      while (true){
        Puncher.move(127);
        delay(2);

        if (fabs(Puncher.get_position()) > 825){
          break;
        }
        ii++;
      }
      reload = false;
      Puncher.move(0);
      if (autoIntake){
        Intake.move(0);
      }
    }
    delay(100);
  }
}

void shootPuncher (bool extra = false){
  reload = true;
  autoIntake = extra;
}

//The PID contorl function
void drive (std::string direction, float target, float waitTime, int maxPower = 100){
  //Constants for Axial Movement
  const float Kp = 0.2;
  const float Kp_C = 1;
  const float Ki = 0.1; //can be a very small value and to disable set to 0.1*10^-10
  const float Kd = 0.5;

  //Constants for Turing
  const float Kp_turn = 0.2; //.4
  const float Kp_C_turn = 0.0; //.1
  const float Ki_turn = 0.1; //.1
  const float Kd_turn = 0.4; //.2

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
  const float integralPowerLimit = 10 / Ki;
  const float integralActiveZone = inchToTicks(3);

  //Final Power
  int finalPower;
  int minPower = 15;

  //Initialize Sensors and Timers
  bool timerLock = true;
  resetSensors();
  Timer endLoopTimer = Timer();
  endLoopTimer.resetTimer();
  if (waitTime <= 250){
    waitTime = 250;
  }


  if (direction == "north" || direction == "south"){
    while (true){
      //Proportion control
      error = inchToTicks(target) - (leftEncoderSum() + rightEncoderSum());
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

      //Set motor powers
      finalPower = proportion + integral + derivative;

      if (abs(finalPower) > maxPower){
        finalPower = maxPower;
      }
      else {
        finalPower = sgn(finalPower) * (abs(finalPower) + 15);
      }


      if (direction == "north")
      driveSet(finalPower - proportion_drift, finalPower + proportion_drift);
      else if (direction == "south")
      driveSet(-finalPower - proportion_drift, -finalPower + proportion_drift);

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

      lcd::set_text(1, std::to_string(abs(leftEncoderSum()) + abs(rightEncoderSum())));
      //Dont wanna over load the poor CPU now do we
      delay(20);
    }
  }

  else if (direction == "left" || direction == "right"){
    while (true){
      //Proportion control
      error = fabs(target * 10) - fabs(gyro.get_value());
      proportion = Kp_turn * error;

      //Drift control
      error_drift = (leftEncoderSum() + 120) - rightEncoderSum();
      proportion_drift = Kp_C_turn * error_drift;

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

      //Set motor powers
      finalPower = proportion + integral + derivative;

      if (abs(finalPower) > maxPower){
        finalPower = maxPower;
      }
      else {
        finalPower = sgn(finalPower) * (abs(finalPower) + 15);
      }


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

      // lcd::set_text(1, "Target: " + std::to_string(target * 10));
      // lcd::set_text(2, "Sensor: " + std::to_string(gyro.get_value()));
      // lcd::set_text(3, "P= " + std::to_string(proportion) + " I= " + std::to_string(integral) + " D= " + std::to_string(derivative));
      // lcd::set_text(4, "Power: " + std::to_string(finalPower));
      // lcd::set_text(5, "Time: " + std::to_string(waitTime - endLoopTimer.getTime()));
      // lcd::set_text(6, "Error: " + std::to_string(error/10) + " degrees");
      //Dont wanna over load the poor CPU now do we
      delay(20);
    }
  }

  if (direction == "east" || direction == "west"){
    while(true){
      error = inchToTickStrafe(target) - (leftEncoderSum() + rightEncoderSum());
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

      //Set motor powers
      finalPower = proportion + integral + derivative;

      if (abs(finalPower) > maxPower){
        finalPower = maxPower;
      }
      else {
        finalPower = sgn(finalPower) * (abs(finalPower) + 15);
      }

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

      lcd::set_text(1, std::to_string(abs(leftEncoderSum()) + abs(rightEncoderSum())));
      //Dont wanna over load the poor CPU now do we
      delay(20);
    }

  }

  //Turn off the drive
  driveSet(0, 0);
}

void autonomous() {
  //Make the puncher a thing
  Task foo (reloadPuncherAuton);

  switch (getAutonomous()){
    case 1:{

    }
    case 2:{

    }
    case 3:{

    }
    case 4:{

    }
    case 5:{

    }
    case 6:{

    }
    case 7:{

    }
    case 8:{

    }
    case 9:{

    }
    case 10:{

    }
    case 11:{

    }
    case 12:{
      lcd::clear_line(1);
      drive(north, 25, 1000);
    }
  }
}
