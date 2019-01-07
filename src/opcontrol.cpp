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

void reloadPuncher(void*){
	while(true){
		if (master.get_digital(E_CONTROLLER_DIGITAL_L2)){
			int i = 0;
			int ii = 0;
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
			}
			Puncher.move(0);
		}
		delay(20);
	}
}

void opcontrol() {
	const int _buttonPressTime = 250;
	Timer T1 = Timer();
	T1.resetTimer();
	Timer T2 = Timer();
	T2.resetTimer();
	Timer T3 = Timer();
	T3.resetTimer();
	Timer SlueTimer = Timer();
	SlueTimer.resetTimer();
	bool driveDirection = true;
	bool holdFlip = true;
	int flipperPower = 0;
	//lcd::initialize();
	Task foo (reloadPuncher);
	//lcd::clear();

	//Lift stuff
	int liftError = 0;
	int liftTarget = 0;
	float lift_Kp = 0.5;
	while (true) {

		// Deadzone variables for base control
		const int deadzone = 10;
		const int threshold = 30;
		int X1 = 0, X2 = 0, Y1, L1 = 0;

		// Check deadzones
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

		// // Expo control for turing and lift
		// if (abs(X1) > threshold){
		// 	L1 = 0;
		// 	if (X1 > 0)
		// 	X1 = (pow((X1 / 100), 3) * 100) + 20;
		// 	else
		// 	X1 = (pow((X1 / 100), 3) * 100) - 20;
		// }
		// if (abs(L1) > threshold){
		// 	if (L1 > 0)
		// 	L1 = 127;
		// 	else
		// 	L1 = -127;
		// }

		// check for drive direction update
		if (master.get_digital(E_CONTROLLER_DIGITAL_A) && driveDirection && T1.getTime() > _buttonPressTime){
			driveDirection = false; //lift forward
			T1.resetTimer();
		}
		else if (master.get_digital(E_CONTROLLER_DIGITAL_A) && !driveDirection && T1.getTime() > _buttonPressTime){
			driveDirection = true; //shooter forward
			T1.resetTimer();
		}
		// Set base and lift motors
		if (driveDirection){
			master.clear();
			FrontLeftDrive.move(Y1 + X2 + X1);
			FrontRightDrive.move(Y1 - X2 - X1);
			BackLeftDrive.move(Y1 - X2 + X1);
			BackRightDrive.move(Y1 + X2 - X1);

		}
		else if (!driveDirection){
			master.set_text(1, 1, "DRIVE INVERTED!");
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
