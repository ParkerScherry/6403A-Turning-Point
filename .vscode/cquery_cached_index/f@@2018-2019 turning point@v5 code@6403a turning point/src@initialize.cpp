#include "main.h"
#include "robot/motors.h"
#include "robot/robot_functions.hpp"

//Initialize method that runs everytime the code
//starts, and initalizes the LCD and calibrates the gyroscope
void initialize() {
	//Initalize and display on the LCD
	lcd::initialize();
	lcd::set_text(1, "Calibrating Gyro!");

	//Waits 2 seconds to properly calibrate the gyro
	delay(2000);
}

//Disabled method that executes when the robot is disabled
//via competition control, this displays which auton is
//selected and shows a map for each one
void disabled() {
	//Clears the LCD screen
	lcd::clear();

	//Infinate loop to constantly check for updates
	while(true){
		//Displays which auton is selected
		lcd::set_text(1, "Selected: " + getAutonomousName());
		lcd::set_text(3, "A= Front");
		lcd::set_text(4, "B= Back");
		lcd::set_text(7, "F = Skills / Switch");

		//Dont wanna stress out the poor brain dont we
		delay(100);
	}
}

//Competition initialize method which runs when robot
//first starts and is competition enabled
void competition_initialize() {}

//How to change upload port on cortex
//Run in TERMINAL: prosv5 upload --slot 4
