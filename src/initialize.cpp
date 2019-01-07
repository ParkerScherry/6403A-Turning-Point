#include "main.h"

//Define and initialize gyroscope
ADIGyro gyro (8);

//Define "jumper clips"
ADIDigitalIn A (1);
ADIDigitalIn B (2);
ADIDigitalIn C (3);
ADIDigitalIn D (4);
ADIDigitalIn E (5);
ADIDigitalIn F (6);

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
std::string getAutonomousName(){
	//Initialize variable to be returned
	std::string myAutonomousName;

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
