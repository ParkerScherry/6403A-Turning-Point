#include "main.h"
#include "motors.h"

//Define motors, controller, and sensors
Controller master (E_CONTROLLER_MASTER);
Motor FrontLeftDrive (FrontLeftDirve_Port, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_COUNTS);
Motor FrontRightDrive (FrontRightDrive_Port, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_COUNTS);
Motor BackLeftDrive (BackLeftDrive_Port, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_COUNTS);
Motor BackRightDrive (BackRightDrive_Port, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_COUNTS);
Motor Lift (Lift_Port, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor Intake (Intake_Port, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_COUNTS);
Motor Puncher (Puncher_Port, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor Flipper (Flipper_Port, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
ADIGyro gyro (Gyro_Port);
ADIDigitalIn A (DgtlA_Port);
ADIDigitalIn B (DgtlB_Port);
ADIDigitalIn C (DgtlC_Port);
ADIDigitalIn D (DgtlD_Port);
ADIDigitalIn E (DgtlE_Port);
ADIDigitalIn F (DgtlF_Port);
