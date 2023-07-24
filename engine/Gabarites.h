#pragma once
#include <vector>
//#include "Vertex01.h"
#include "LineXY.h"

class Gabarites
{
public:
	//bounding box
	float bbMin[3];
	float bbMax[3];

	float bbMid[4];
	float bbRad[3];

	//chord
	float chordR;
	LineXY chord;

public:
	static void adjustMidRad(Gabarites* pGB);
};

