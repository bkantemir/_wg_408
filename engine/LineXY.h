#pragma once
//#include "Vertex01.h"
#include <vector>

class LineXY
{
public:
	float p0[4] = { 0,0,0,0 };
	float p1[4] = { 0,0,0,0 };
	//line equation
	float a_slope = 0; //a
	float b_intercept = 0; //b
	bool isVertical = false;
	float x_vertical = 0;
	bool isHorizontal = false;
	bool isDot = false;
public:
	LineXY(void) {};
	LineXY(float* p00, float* p01) { initLineXY(p00, p01); };
	void initLineXY(float* p00, float* p01);
	void initDirXY(float* p01);
	static bool matchingLines(LineXY* pLine0, LineXY* pLine1);
	static bool parallelLines(LineXY* pLine0, LineXY* pLine1);
};
