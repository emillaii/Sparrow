#pragma once

#define SUT_POSITION_PICK_X "sut_pick_x"
#define SUT_POSITION_PICK_Y "sut_pick_y"
#define SUT_POSITION_PICK_Z "sut_pick_z"

#define AA_POSITION_ZSCAN_X "aa_zscan_x"
#define AA_POSITION_ZSCAN_Y "aa_zscan_y"
#define AA_POSITION_ZSCAN_Z "aa_zscan_z"

struct SensorUT{
	double sutPickX; 
	double sutPickY;
	double sutPickZ; 
};

struct AAHead {
	double aaZScanX; 
	double aaZScanY; 
	double aaZScanZ;
	double pickLensZ; 
	double aaZScanStartZ;
	double initTiltX;
	double initTiltY;
};