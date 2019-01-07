#include "main.h"

//Timer
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

//Math Functions
int inchToTicks (float inch);
int inchToTickStrafe (float inch);
int sgn (int input);
int degreesToTicks (float degree);
int percentToMotorValue (int percent);

//Helper Functions
void resetSensors();
void driveSet();
int rightEncoderSum();
int leftEncoderSum();

//Movement Functions
void reloadPuncher (void*);
void reloadPuncherAuton (void*);
void shootPuncher (bool intake = false);
void liftControl (void*);
void lift (int target);
void flip();
void drive (std::string direction, float target, float waitTime, int maxPower = 100);

//Auton Selection Functions
int getAutonomous();
std::string getAutonomousName();
