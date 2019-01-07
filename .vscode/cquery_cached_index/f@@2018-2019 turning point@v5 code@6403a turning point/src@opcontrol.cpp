#include "main.h"
#include "robot/motors.h"

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
	int flipperPower = 0;

	// Proportion lift control variables
	int liftError = 0;
	int liftTarget = 0;
	float lift_Kp = 0.5;

	// Proportion flipper hold control variables
	int flipError = 0;
	int flipTarget = 30;
	float flip_Kp = 0.5;

	// Deadzone variables for base control
	const int deadzone = 10;
	const int threshold = 30;
	int X1 = 0, X2 = 0, Y1, L1 = 0;

	//Starts reload puncher task which runs side by side with opcontrol
	Task reloadPuncherTask (reloadPuncher);

	//Infinate loop for usercontrol
	while (true) {

		// Check deadzones for base control
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

		// Check for drive direction update
		if (master.get_digital(E_CONTROLLER_DIGITAL_A) && driveDirection &&
		ButtonPressTimer.getTime() > _buttonPressTime){
			// Change drive direction to reverse
			driveDirection = false;

			// Reset button press timer
			ButtonPressTimer.resetTimer();
		}
		else if (master.get_digital(E_CONTROLLER_DIGITAL_A) && !driveDirection &&
		ButtonPressTimer.getTime() > _buttonPressTime){
			// Normalizes the drive direciton
			driveDirection = true;

			// Reset button press timer
			ButtonPressTimer.resetTimer();
		}

		// Set base and lift motors accordinly
		if (driveDirection){
			FrontLeftDrive.move(Y1 + X2 + X1);
			FrontRightDrive.move(Y1 - X2 - X1);
			BackLeftDrive.move(Y1 - X2 + X1);
			BackRightDrive.move(Y1 + X2 - X1);
		}
		else if (!driveDirection){
			FrontLeftDrive.move(-(Y1 + X2 - X1));
			FrontRightDrive.move(-(Y1 - X2 + X1));
			BackLeftDrive.move(-(Y1 - X2 - X1));
			BackRightDrive.move(-(Y1 + X2 + X1));
		}

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
			//Set variable to unhold flipper
			toggleFlip = false;

			//Reset Timers
			ButtonPressTimer.resetTimer();
			FlipperControl.resetTimer();
			SlueTimer.resetTimer();
		}
		else if (master.get_digital(E_CONTROLLER_DIGITAL_R2) && !toggleFlip &&
	  ButtonPressTimer.getTime() > _buttonPressTime){
			//Set variable to unhold flipper
			toggleFlip = true;

			//Reset Timers
			ButtonPressTimer.resetTimer();
			FlipperControl.resetTimer();
			SlueTimer.resetTimer();
		}

		//Checks if flipper needs to go up
		if (toggleFlip){
			//Uses timer to move up for 500 ms
			if(FlipperControl.getTime() < 500){
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

			//Sets flipper power limit
			if (SlueTimer.getTime() >= 60){
				flipperPower = 0;
			}

			//Check if the lift is being raised and pitches the
			//flipper at an agle to keep from sliding off
			if (Lift.get_position() > 40){
				//Calcuate the flip error from the set target value
				flipError = flipTarget - Flipper.get_position();

				//Move the flipper accordingly and reset slue timer
				Flipper.move(flipError * flip_Kp);
				SlueTimer.resetTimer();
			}
			//Moves flipper normaly because there is no hold
			else{
				Flipper.move(flipperPower);
			}
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
			if (abs(liftError) == 0){
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

		//Dont wanna stress out the poor brain do we
		delay(20);
	}
}
