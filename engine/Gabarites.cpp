#include "Gabarites.h"
//#include "utils.h"

void Gabarites::adjustMidRad(Gabarites* pGB) {
	for (int i = 0; i < 3; i++) {
		pGB->bbRad[i] = pGB->bbMax[i] - pGB->bbMin[i];
		pGB->bbMid[i] = (pGB->bbMax[i] + pGB->bbMin[i]) * 0.5f;
	}
}



