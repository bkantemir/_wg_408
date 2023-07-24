#include "LineXY.h"
#include "utils.h"

void LineXY::initDirXY(float* p01) {
	float p00[3] = { 0,0 };
	initLineXY(p00, p01);
}
void LineXY::initLineXY(float* p00, float* p01) {
	v2copy(p0, p00);
	v2copy(p1, p01);
	if (v2match(p0, p1))
		isDot = true;
	else {
		isDot = false;
		//find line equation
		if (p0[0] == this->p1[0]) {
			isVertical = true;
			x_vertical = p0[0];
		}
		else if (p0[1] == p1[1]) {
			isHorizontal = true;
			a_slope = 0;
			b_intercept = p0[1];
		}
		else {
			a_slope = (p1[1] - p0[1]) / (p1[0] - p0[0]);
			b_intercept = p0[1] - a_slope * p0[0];
		}
	}
	return;
}
bool LineXY::matchingLines(LineXY* pLine0, LineXY* pLine1) {
	if (!parallelLines(pLine0, pLine1))
		return false;
	if (pLine0->b_intercept != pLine1->b_intercept)
		return false;
	if (pLine0->x_vertical != pLine1->x_vertical)
		return false;
	return true;
}
bool LineXY::parallelLines(LineXY* pLine0, LineXY* pLine1) {
	if (pLine0->isVertical != pLine1->isVertical)
		return false;
	if (pLine0->a_slope != pLine1->a_slope)
		return false;
	return true;
}
