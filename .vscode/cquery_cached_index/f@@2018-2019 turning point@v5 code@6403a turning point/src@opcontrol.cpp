#include "main.h"
#include "robot/motors.h"
#include "robot/robot_functions.hpp"

//User control method
void opcontrol() {
	//Button press time
	const int _buttonPressTime = 250;

	//Initalze timers
	Timer ButtonPressTimer = Timer();
	ButtonPressTimer.resetTimer();
	Timer FlipperControl = Timer();
	FlipperControl.resetTimer();
	Timer SlueTimer = Timer();
	SlueTimer.resetTimer();

	// Drive reversal and flipper control variables
	bool driveDirection = true;
	bool toggleFlip = false;
	bool toggleFlipHold = true;
	int flipperPower = 0;

	// Proportion lift control variables
	int liftError = 0;
	int liftTarget = 0;
	float lift_Kp = 0.5;

	// Proportion flipper hold control variables
	int flipError = 0;
	int flipTarget = 100;
	float flip_Kp = 0.5;

	// Deadzone variables for base control
	const int deadzone = 10;
	const int threshold = 30;
	int X1 = 0, X2 = 0, Y1, L1 = 0;

	//Starts reload puncher task which runs side by side with opcontrol
	Task reloadPuncherTask (reloadPuncher);

	//Infinate loop for usercontrol
	while (true) {
		//Displays which auton is selected
		lcd::set_text(1, "Selected: " + getAutonomousName());
		lcd::set_text(3, "A= Front");
		lcd::set_text(4, "B= Back");
		lcd::set_text(7, "F = Skills / Switch");

		// Check deadzones for base and lift control
		if (abs(master.get_analog(ANALOG_LEFT_X)) > deadzone)
		X1 = master.get_analog(ANALOG_LEFT_X);
		else
		X1 = 0;
		if (abs(master.get_analog(ANALOG_RIGHT_X)) > deadzone)
		X2 = master.get_analog(ANALOG_RIGHT_X);
		else
		X2 = 0;
		if (abs(master.get_analog(ANALOG_RIGHT_Y)) > deadzone)
		Y1 = master.get_analog(ANALOG_RIGHT_Y);
		else
		Y1 = 0;
		if (abs(master.get_analog(ANALOG_LEFT_Y)) > deadzone)
		L1 = master.get_analog(ANALOG_LEFT_Y);
		else
		L1 = 0;

		// Set base and lift motors accordinly
		FrontLeftDrive.move(Y1 + X2 + X1);
		FrontRightDrive.move(Y1 - X2 - X1);
		BackLeftDrive.move(Y1 - X2 + X1);
		BackRightDrive.move(Y1 + X2 - X1);

		// Intake Control
		if (master.get_digital(E_CONTROLLER_DIGITAL_R1)){
			Intake.move(127);
		}
		else if (master.get_digital(E_CONTROLLER_DIGITAL_L1)){
			Intake.move(-127);
		}
		else {
			Intake.move(0);
		}

		//Flipper Control
		if (master.get_digital(E_CONTROLLER_DIGITAL_R2) && toggleFlip &&
		ButtonPressTimer.getTime() > _buttonPressTime){
			//Set variable to flip
			toggleFlip = false;

			//Reset Timers
			ButtonPressTimer.resetTimer();
			FlipperControl.resetTimer();
			SlueTimer.resetTimer();
		}
		else if (master.get_digital(E_CONTROLLER_DIGITAL_R2) && !toggleFlip &&
		ButtonPressTimer.getTime() > _buttonPressTime){
			//Set variable to unflip
			toggleFlip = true;

			//Reset Timers
			ButtonPressTimer.resetTimer();
			FlipperControl.resetTimer();
			SlueTimer.resetTimer();
		}

		if (master.get_digital(E_CONTROLLER_DIGITAL_A) && toggleFlipHold &&
		ButtonPressTimer.getTime() > _buttonPressTime){
			//Set variable to unhold flipper
			toggleFlipHold = false;

			//Reset Timers
			ButtonPressTimer.resetTimer();
			FlipperControl.resetTimer();
			SlueTimer.resetTimer();
		}
		else if (master.get_digital(E_CONTROLLER_DIGITAL_A) && !toggleFlipHold &&
		ButtonPressTimer.getTime() > _buttonPressTime){
			//Set variable to hold flipper
			toggleFlipHold = true;

			//Reset Timers
			ButtonPressTimer.resetTimer();
			FlipperControl.resetTimer();
			SlueTimer.resetTimer();
		}

		//Checks if flipper needs to go up
		if (toggleFlip){
			//Uses timer to move up for 750 ms
			if(FlipperControl.getTime() < 750){
				//Moves motor full power
				Flipper.move(127);
			}

			//Holds flipper at the top after flip
			else{
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
			}
		}

		//Moves the flipper down
		else if (!toggleFlip){
			//Slue flipper movement down to prevent an imediate
			//loss of currents
			flipperPower = SlueTimer.getTime() - 60;

			//Check if the lift is being raised and pitches the
			//flipper at an agle to keep from sliding off
			if (Lift.get_position() > 40 && toggleFlipHold){
				//Calcuate the flip error from the set target value
				flipError = flipTarget - Flipper.get_position();

				//Move the flipper accordingly and reset slue timer
				flipperPower = flipError * flip_Kp;
				SlueTimer.resetTimer();
			}

			//Sets flipper power limit
			else if (SlueTimer.getTime() >= 60){
				flipperPower = 0;
			}

			Flipper.move(flipperPower);
		}

		//Proportional lift control that holds the lift where
		//it was last left at. Checks for when the the lift joystick
		//is not being moved
		if (abs(L1) <= deadzone){
			//Sets lift error to tartget value (set from moving the lift,
			//but by defult is 0) minus its current position
			liftError = liftTarget - Lift.get_position();

			//If error is nothing stops moving lift to prevent unnecisarry
			//movement of the lift
			if (abs(liftError) < 5){
				liftError = 0;
			}

			//Sets lift adjustment power to the error times the constant
			//of proportion
			L1 = liftError * lift_Kp;
			Lift.move(L1);
		}

		//Checks for when the joysticks are being moved past the deadzones
		else if (abs(L1) > deadzone){
			//Moves the lift corrisponding to the joystick position
			Lift.move(L1);

			//Sets lift target to its current position
			liftTarget = Lift.get_position();
		}

		//Resets flipper hold when the lift moves all the way down
		if (Lift.get_position() < 40){
			toggleFlipHold = true;
		}

		//Dont wanna stress out the poor brain do we
		delay(20);
	}
}
