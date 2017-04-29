bool bSide;
int iArmCurrentRPM, iChassisLeftRPM, iChassisRightRPM, iClawRPM;
task
calculateRPMs( ) {
	while( true ) {
		int iFirstArm             =  SensorValue[ I2C_3 ];
		int iFirstChassisLeftRPM  =  SensorValue[ I2C_1 ];
		int iFirstChassisRightRPM = -SensorValue[ I2C_2 ];
		int iFirstClawRPM         =  SensorValue[ intake_encoder ];
		delay( 50);
		iArmCurrentRPM   =  SensorValue[ I2C_3 ]          - iFirstArm;
		iChassisLeftRPM  =  SensorValue[ I2C_1 ]          - iFirstChassisLeftRPM;
		iChassisRightRPM = -SensorValue[ I2C_2 ]          - iFirstChassisRightRPM;
		iClawRPM         =  SensorValue[ intake_encoder ] - iFirstClawRPM;
	}
}

task
record( ) {
	startTask( calculateRPMs );
	int j = 20, s = 1;
	for( int i = 0; i < 400; i++ ) {
		if( j == 20 ) {
			writeDebugStream( "\n" );
			writeDebugStream( "\n	/* %i Second(s) */", s );
			s++;
			j = 0;
		}
		j++;

		writeDebugStream( "\n	auton( %i, %i, %i, %i, %i, %i, %i, %i);", iArmCurrentRPM, motor[ arm_l1 ], iChassisLeftRPM, motor[ chassis_l ], iChassisRightRPM, motor[ chassis_r ], iClawRPM, motor[ intake_l ] );

		delay( 50 );
	}
	stopTask( calculateRPMs );
	writeDebugStream( "\n\n	/* Stop the robot */\n	auton( 0, 0, 0, 0, 0, 0, %i, %i );", iClawRPM, motor[ intake_l ] );
}

void
auton( int iArmDes, int iArmSpeed, int iChassisLeftDes, int iChassisLeftSpeed, int iChassisRightDes, int iChassisRightSpeed, int iClawDes, int iClawSpeed ) {
	int iArmError = 0, iChassisLeftError = 0, iChassisRightError = 0, iClawError = 0;
	for( int i = 0; i < 50; i++ ) {
		iArmError          -= (iArmDes - iArmCurrentRPM) * 0.015;
		iChassisLeftError  -= (iChassisLeftDes - iChassisLeftRPM) * 0.01;
		iChassisRightError -= (iChassisRightDes - iChassisRightRPM) * 0.01;
		iClawError         -= (iClawDes - iClawRPM) * 0.01;

		arm( iArmError + iArmSpeed );
		motor[ intake_l ] =
		motor[ intake_r ] = (iClawError + iClawSpeed );

		if( !bSide )
			tankWithoutTrueSpeed( iChassisLeftError + iChassisLeftSpeed, iChassisRightError + iChassisRightSpeed );
		else
			tankWithoutTrueSpeed( iChassisRightError + iChassisRightSpeed, iChassisLeftError + iChassisLeftSpeed );

		delay( 1 );
	}
}
