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
	this->aahead.aaZScanX = 4;
	this->aahead.aaZScanY = 5;
	this->aahead.aaZScanZ = 6;
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

//AA Head


AAHead MachineDefinition::getAAHeadPos()
{
	return this->aahead;
}

void MachineDefinition::setAAHeadPos(AAHead aahead)
{
	this->aahead = aahead;
}

void MachineDefinition::setAAZscanPosX(double v)
{
	aahead.aaZScanX = v;
}

void MachineDefinition::setAAZscanPosY(double v)
{
	aahead.aaZScanY = v;
}

void MachineDefinition::setAAZscanPosZ(double v)
{
	aahead.aaZScanZ = v;
}