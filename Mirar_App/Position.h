#pragma once

#define SUT_POSITION_PICK_X "sut_pick_x"
#define SUT_POSITION_PICK_Y "sut_pick_y"
#define SUT_POSITION_PICK_Z "sut_pick_z"

struct SensorUT{
	double sutPickX; 
	double sutPickY;
	double sutPickZ; 
};

struct AAHead {
	double pickLensZ; 
	double aaZScanStartZ;
	double initTiltX;
	double initTiltY;
};