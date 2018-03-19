#include "stdafx.h"
#include "MachineDefinition.h"


MachineDefinition::MachineDefinition()
{
	init(); 
}


MachineDefinition::~MachineDefinition()
{
}

bool MachineDefinition::init()
{
	this->sut.sutPickX = 1; 
	this->sut.sutPickY = 2; 
	this->sut.sutPickZ = 3; 
	this->aahead.aaZScanStartZ = 11; 
	this->aahead.initTiltX = 12; 
	this->aahead.initTiltY = 13;
	this->aahead.pickLensZ = 14;
	return true; 
}

SensorUT MachineDefinition::getSensorUTPos()
{
	return this->sut;
}

void MachineDefinition::setSensorUTPos(SensorUT sut)
{
	this->sut = sut; 
}

void MachineDefinition::setSensorUTPickXPos(double v)
{
	this->sut.sutPickX = v;
}

void MachineDefinition::setSensorUTPickYPos(double v)
{
	this->sut.sutPickY = v;
}

void MachineDefinition::setSensorUTPickZPos(double v)
{
	this->sut.sutPickZ = v;
}


AAHead MachineDefinition::getAAHeadPos()
{
	return this->aahead;
}
