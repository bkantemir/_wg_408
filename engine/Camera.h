#pragma once
#include "Coords.h"
#include "linmath.h"

class Camera
{
public:
	Coords ownCoords;
	mat4x4 lookAtMatrix; //view?
	mat4x4 mViewProjection;
	float lookAtPoint[3] = { 0,0,0 };
	float focusDistance = 100;
	float viewRangeDg = 30;
	float stageSize[2] = { 500, 375 };

	float targetSize[2] = { 10,10 };
	float targetAspectRatio = 1;
public:
	float pickDistance() { return pickDistance(this); };
	static float pickDistance(Camera* pCam);
	void setCameraPosition() { setCameraPosition(this); };
	static void setCameraPosition(Camera* pCam);
	void buildLookAtMatrix() { buildLookAtMatrix(this); };
	static void buildLookAtMatrix(Camera* pCam);
	void onTargetResize(int width, int height) { onTargetResize(this, width, height); };
	static void onTargetResize(Camera* pCam, int width, int height);
	void buildViewProjection() { buildViewProjection(this); };
	static void buildViewProjection(Camera* pCam);
	void reset() { reset(this); };
	static void reset(Camera* pCam);
};

