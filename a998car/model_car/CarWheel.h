#pragma once
#include "SceneSubj.h"

class CarWheel : public SceneSubj
{
public:
	float wheelDiameter = 20;
public:
	virtual SceneSubj* clone() { return new CarWheel(*this); };
	virtual int moveSubj() { return moveSubj(this); };
	static int moveSubj(CarWheel* pGS);
};