#include "main.h"
//#include "initialize.cpp"
//#include "motors.h"

Motor FrontLeftDrive (7, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_COUNTS);
Motor FrontRightDrive (10, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_COUNTS);
Motor BackLeftDrive (12, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_COUNTS);
Motor BackRightDrive (1, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_COUNTS);
Motor Lift (3, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor Intake (20, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_COUNTS);
Motor Puncher (6, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor Flipper (14, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Controller master (E_CONTROLLER_MASTER);

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
			while(Puncher.get_current_draw() < 300 && i > 120){
				//Moves puncher back and reset encoder position in
				//preperation for next procedure
				Puncher.move(127);
				Puncher.tare_position();

				//Adds one to iteration counter
				i++;

				///Dont wanna stress out the poor brain dont we
				delay(2);
			}

			//Cock backs the puncher until it is about to shoot
			//again, so the puncher is ready to fire instantly
			while (fabs(Puncher.get_position()) > 825){
				//Moves puncher back
				Puncher.move(127);

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
	Timer T2 = Timer();
	T2.resetTimer();
	Timer T3 = Timer();
	T3.resetTimer();
	Timer SlueTimer = Timer();
	SlueTimer.resetTimer();

	// Drive reversal and flipper control variables
	bool driveDirection = true;
	bool holdFlip = true;
	int flipperPower = 0;

	// Proportion lift control variables
	int liftError = 0;
	int liftTarget = 0;
	float lift_Kp = 0.5;

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
		if (master.get_digital(E_CONTROLLER_DIGITAL_A) && driveDirection && T1.getTime() > _buttonPressTime){
			// Change drive direction to reverse
			driveDirection = false;

			// Reset button press timer
			T1.resetTimer();
		}
		else if (master.get_digital(E_CONTROLLER_DIGITAL_A) && !driveDirection && T1.getTime() > _buttonPressTime){
			// Normalizes the drive direciton
			driveDirection = true;

			// Reset button press timer
			T1.resetTimer();
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
		if (master.get_digital(E_CONTROLLER_DIGITAL_R2) && holdFlip && T2.getTime() > _buttonPressTime){
			holdFlip = false;
			T2.resetTimer();
			T3.resetTimer();
			SlueTimer.resetTimer();
		}
		else if (master.get_digital(E_CONTROLLER_DIGITAL_R2) && !holdFlip && T2.getTime() > _buttonPressTime){
			holdFlip = true;
			T2.resetTimer();
			T3.resetTimer();
			SlueTimer.resetTimer();
		}

		if (!holdFlip){
			if(T3.getTime() < 450){
				Flipper.move(127);
			}
			else{
				flipperPower = SlueTimer.getTime() * 3;
				if (flipperPower > 750){
					flipperPower = 750;
				}
				Flipper.move_voltage(flipperPower);
			}
		}

		else if (holdFlip){
			flipperPower = SlueTimer.getTime() - 61;
			if (SlueTimer.getTime() >= 61){
				flipperPower = 0;
			}
			Flipper.move(flipperPower);

		}

		//Lift Control
		if (abs(L1) <= deadzone){
			liftError = liftTarget - Lift.get_position();
			if (abs(liftError) == 0){
				liftError = 0;
			}
			L1 = liftError * lift_Kp;
			Lift.move(L1);
		}
		else if (abs(L1) > deadzone){
			Lift.move(L1);
			liftTarget = Lift.get_position();
		}

		delay(20);
	}
}
