#include "Camera.h"
#include "TheApp.h"
#include "utils.h"

//extern TheGame theGame;
extern float degrees2radians;

void Camera::setCameraPosition(Camera* pCam) {
	v3set(pCam->ownCoords.pos, 0, 0, -pCam->focusDistance);
	mat4x4_mul_vec4plus(pCam->ownCoords.pos, pCam->ownCoords.rotationMatrix, pCam->ownCoords.pos, 1);
	for (int i = 0; i < 3; i++)
		pCam->ownCoords.pos[i] += pCam->lookAtPoint[i];
}
void Camera::buildLookAtMatrix(Camera* pCam) {
	float cameraUp[4] = { 0,1,0,0 }; //y - up
	mat4x4_mul_vec4plus(cameraUp, pCam->ownCoords.rotationMatrix, cameraUp, 0);
	mat4x4_look_at(pCam->lookAtMatrix, pCam->ownCoords.pos, pCam->lookAtPoint, cameraUp);
}
void Camera::onTargetResize(Camera* pCam, int width, int height) {
	pCam->targetSize[0] = (float)width;
	pCam->targetSize[1] = (float)height;
	pCam->targetAspectRatio = (float)width / (float)height;
	//glViewport(0, 0, width, height);
	pCam->reset(pCam);
}
void Camera::reset(Camera* pCam) {
	pickDistance(pCam);
	setCameraPosition(pCam);
	buildLookAtMatrix(pCam);
	buildViewProjection(pCam);
}
float Camera::pickDistance(Camera* pCam) {
	float viewRangeDg = pCam->viewRangeDg;
	if (viewRangeDg == 0)
		viewRangeDg = 30;
	float cotangentA = 1.0f / tanf(degrees2radians * viewRangeDg / 2);
	float cameraDistanceV = pCam->stageSize[1] / 2 * cotangentA;
	float cameraDistanceH = pCam->stageSize[0] / 2 * cotangentA / pCam->targetAspectRatio;
	pCam->focusDistance = (float)fmax(cameraDistanceV, cameraDistanceH);
	return pCam->focusDistance;
}
void Camera::buildViewProjection(Camera* pCam) {
	mat4x4 mProjection;
	float r = pCam->stageSize[0] / 2 + 5.0f;
	float nearClip = pCam->focusDistance - r;
	float farClip = pCam->focusDistance + r;
	if (nearClip < 0) nearClip = 0;
	if (pCam->viewRangeDg == 0) {
		float w = pCam->stageSize[0] / 2 + 10.0f;
		float h = w / pCam->targetAspectRatio;
		mat4x4_ortho(mProjection, -w, w, -h, h, nearClip, farClip);
		mat4x4_mul(pCam->mViewProjection, mProjection, pCam->lookAtMatrix);
	}
	else {
		mat4x4_perspective(mProjection, pCam->viewRangeDg * degrees2radians, pCam->targetAspectRatio, nearClip, farClip);
		mat4x4_mul(pCam->mViewProjection, mProjection, pCam->lookAtMatrix);
		pCam->mViewProjection[1][3] = 0; //keystone effect
	}
}
