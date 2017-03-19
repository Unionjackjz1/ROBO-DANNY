#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    autonSide,      sensorPotentiometer)
#pragma config(Sensor, dgtl1,  intake_encoder, sensorQuadEncoder)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           arm_l1,        tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           chassis_l,     tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port3,           intake_l,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           arm_l2,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           arm_l3,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           arm_r3,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           arm_r2,        tmotorVex393_MC29, openLoop, encoderPort, I2C_3)
#pragma config(Motor,  port8,           intake_r,      tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           chassis_r,     tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_2)
#pragma config(Motor,  port10,          arm_r1,        tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)
#include "Mod_Vex_Comp_Control.c"
#include "motor.h"
#include "RerunFunctions.c"
#include "Autons.h"

int iArmDes;
void
armControl( bool bBtnUp, bool bBtnDown, bool bFullSpeedDown, bool bPreset ) {
	if( bBtnUp || bBtnDown || bFullSpeedDown ) {
		if( bBtnDown )       arm( -90 );
		if( bBtnUp )         arm( 127 );
		if( bFullSpeedDown ) arm( -127 );
		iArmDes = SensorValue[ I2C_3 ];
		} else {
		iArmDes = bPreset ? 484 : iArmDes;
		arm( iArmPID(iArmDes) );
	}
}

int iIntakeDes, iIntakeOutput;
void
intakeControl( bool bBtnUp, bool bBtnDown, bool bManualOpen, bool bPreset ) {
	if( bManualOpen || bBtnDown) {
		iIntakeOutput = ( 127 * (bManualOpen - bBtnDown) );
		iIntakeDes = SensorValue[ intake_encoder ];
		} else {
		iIntakeDes = bBtnUp ? -120 : (bPreset ? -71 : iIntakeDes);
		iIntakeOutput = ( iIntakePID(iIntakeDes) );
	}
	motor[ intake_r ] =
	motor[ intake_l ] = iIntakeOutput;
}

int iLeftDes, iRightDes;
void
chassisControl( int iLeft, int iRight ) {
	if( abs(iLeft) > 10 || abs(iRight) > 10 ) {
		tank( iLeft, iRight );
		iLeftDes  =  SensorValue[ I2C_1 ];
		iRightDes = -SensorValue[ I2C_2 ];
	}
	else if( abs(iLeft) < 10 || abs(iRight) < 10 && SensorValue[I2C_3] > 300) {
		tank( (iLeftDes - SensorValue[I2C_1]) * 0.5, (iRightDes - -SensorValue[I2C_2]) * 0.5 );
	}
	else
		tank(0, 0);
}

void
init( ) {
	strTeam = "ROBO-DANNY";
	bLCDAuton =

	bLCDUserControl = true;
}

const int kMaxNumberOfPages = 3;
const int kMinNumberOfPages = 1;
int iAuton = 0;

void
pre_auton( ) {
	bool bLCD = nLCDButtons != 0 ? true : false;
	if(bLCD) iAuton = 1;
	while( bLCD ) {
		if( nLCDButtons == 1 ) {
			iAuton = iAuton == kMinNumberOfPages ? kMaxNumberOfPages : iAuton - 1;
			waitForRelease( );
		}
		if( nLCDButtons == 4 ) {
			iAuton = iAuton == kMaxNumberOfPages ? kMinNumberOfPages : iAuton + 1;
			waitForRelease( );
		}
		if( nLCDButtons == 2 ) {
			bLCD = false;
			waitForRelease( );
		}

		displayLCDCenteredString( 0, "Auton #" );
		displayLCDNumber( 0, 11, iAuton );
		displayLCDCenteredString( 1, "<    Select    >");
		delay( 50 );
		clearLCDLines( );
	}

	clearAllEncoder( );
}

task
autonomous( ) {
	if( SensorValue[ autonSide ] == 0 )
		bSide = true;
	if( SensorValue[ autonSide ] == 4095 )
		bSide = false;

	clearAllEncoder( );
	startTask( calculateRPMs );

	switch( iAuton ) {
	case 1:  auton1( ); break;
	case 2:  auton2( ); break;
	case 3:  auton3( ); break;
	default: auton1( ); break;
	}

	auton( 0, 0, 0, 0, 0, 0, 0, 0 );

	stopTask( calculateRPMs );
}

task
teleop( ) {
	while( true ) {
		intakeControl ( vexRT[ Btn6D ], vexRT[ Btn5D ], vexRT[ Btn8D ], vexRT[ Btn7D ] );
		armControl    ( vexRT[ Btn6U ], vexRT[ Btn5U ], vexRT[ Btn8U ], vexRT[ Btn7D ] );
		chassisControl( vexRT[ Ch3 ],   vexRT[ Ch2 ] );

		if( vexRT[ Btn7U ] ) {
			waitForReleased7U( );
			startTask( record );
		}

		if( vexRT[ Btn8R ] ) {
			waitForReleased8R( );
			SensorValue[ intake_encoder ] = 0;
		}

		if( vexRT[ Btn8L ] ) {
			waitForReleased8L( );
			SensorValue[ I2C_3 ] = 0;
		}

		delay( 20 );
	}
}
