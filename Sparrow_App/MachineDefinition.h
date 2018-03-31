#pragma once
#include "Position.h"
class MachineDefinition
{
public:
	MachineDefinition();
	~MachineDefinition();

//ToDo: Change the machine module class later after motion integration
private: 
	SensorUT sut; 
	AAHead aahead; 

public:
	bool init();
	void setSensorUTPos(SensorUT sut);
	void setSensorUTPickXPos(double v);
	void setSensorUTPickYPos(double v);
	void setSensorUTPickZPos(double v);

	void setAAZscanPosX(double v);
	void setAAZscanPosY(double v);
	void setAAZscanPosZ(double v);
	SensorUT getSensorUTPos();
	void setAAHeadPos(AAHead aahead);
	AAHead getAAHeadPos();


};

