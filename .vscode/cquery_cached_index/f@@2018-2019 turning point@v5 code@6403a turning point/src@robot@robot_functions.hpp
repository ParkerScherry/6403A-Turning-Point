#include "main.h"

const string north = "north";
const string south = "south";
const string east = "east";
const string west = "west";
const string left = "left";
const string right = "right";

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
void flip(bool moveDown = false);
void drive (std::string direction, float target, float waitTime, int maxPower = 115);

//Auton Selection Functions
int getAutonomous();
string getAutonomousName();
void displayAuton(void*);
