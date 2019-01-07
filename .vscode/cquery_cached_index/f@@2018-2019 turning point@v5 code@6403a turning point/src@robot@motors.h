#include "main.h"

#ifndef MOTOR_H
#define MOTOR_H

//Motor and sensor ports
#define FrontLeftDirve_Port (7)
#define FrontRightDrive_Port (10)
#define BackLeftDrive_Port (12)
#define BackRightDrive_Port (1)
#define Lift_Port (3)
#define Intake_Port (20)
#define Puncher_Port (6)
#define Flipper_Port (14)
#define Gyro_Port ('H')
#define DgtlA_Port ('A')
#define DgtlB_Port ('B')
#define DgtlC_Port ('C')
#define DgtlD_Port ('D')
#define DgtlE_Port ('E')
#define DgtlF_Port ('F')

//Delcare motors and sensors
extern Controller master;
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
extern ADIDigitalIn B;
extern ADIDigitalIn C;
extern ADIDigitalIn D;
extern ADIDigitalIn E;
extern ADIDigitalIn F;
extern ADIDigitalIn G;

#endif
