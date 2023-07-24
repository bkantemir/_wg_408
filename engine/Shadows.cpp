#include "Shadows.h"
#include "TheApp.h"
#include "utils.h"

float Shadows::shadowLight = 0.5f;
int Shadows::depthMapTexN = -1;
Texture* Shadows::pDepthMap = NULL;
Camera Shadows::shadowCamera;
float Shadows::sizeUnitPixelsSize = 1;

extern TheApp theApp;

float Shadows::uDepthBias[16]; //z-value shifts depeding on normal
std::vector<SceneSubj*> Shadows::shadowsQueue;

int Shadows::init() {
	if (v3equals(theApp.dirToMainLight, 0)) {
		mylog("ERROR in Shadows::init(): theApp.dirToMainLight not set.\n");
		return -1;
	}

	depthMapTexN = Texture::textures.size();
	Texture* pTex = new Texture();
	Texture::textures.push_back(pTex);
	pTex->size[0] = 1024;
	pTex->size[1] = 1024;
	pTex->source.assign("depthMap");

	glGenTextures(1, (GLuint*)&pTex->GLid);
	glBindTexture(GL_TEXTURE_2D, pTex->GLid);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		//pTex->size[0], pTex->size[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16F, pTex->size[0], pTex->size[1]);
	checkGLerrors("Depth tex");

	glBindTexture(GL_TEXTURE_2D, 0);
	/////////////////

	Texture::attachRenderBuffer(depthMapTexN, true);
	pDepthMap = Texture::textures.at(depthMapTexN);
	float worldRadius = 45; //2;
	sizeUnitPixelsSize = pDepthMap->size[0] / (worldRadius * 2);

	//set up camera
	Camera* pCam = &shadowCamera;
	pCam->focusDistance = worldRadius;

	float vDir[4];
	for (int i = 0; i < 3; i++) {
		pCam->ownCoords.pos[i] = theApp.dirToMainLight[i] * worldRadius;
		vDir[i] = -pCam->ownCoords.pos[i];
	}
	//build pCam->ownCoords.eulerDg[]
	pCam->ownCoords.setEulerDg(v3pitchDg(vDir), v3yawDg(vDir), 0);
	//build pCam->ownCoords.rotationMatrix
	mat4x4_from_quat(pCam->ownCoords.rotationMatrix, pCam->ownCoords.getRotationQuat());
	pCam->buildLookAtMatrix(pCam);

	//set up mViewProjection matrix
	mat4x4 mProjection;
	float nearClip = 0;
	float farClip = worldRadius * 2;
	mat4x4_ortho(mProjection, -worldRadius, worldRadius, -worldRadius, worldRadius, nearClip, farClip);
	mat4x4_mul(shadowCamera.mViewProjection, mProjection, shadowCamera.lookAtMatrix);

	//calculate uBias (for depth map)
	int totalN = 16;
	float normalStep = 1.0f / (totalN + 1);
	for (int i = 0; i < totalN; i++) {
		float normalZ = normalStep * (i + 1);
		float bias = sqrtf(1.0f - normalZ * normalZ) / normalZ;
		uDepthBias[i] = bias / (float)pDepthMap->size[0] * 3.0f;
	}
	return 1;
}

int Shadows::renderDepthMap() {
	Texture::setRenderToTexture(depthMapTexN);
	//Texture* pT = Texture::textures.at(depthMapTexN);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::vector<SceneSubj*>* pSubjs = &shadowsQueue;
	//RenderScene
	int subjsN = pSubjs->size();
	for (int subjN = 0; subjN < subjsN; subjN++) {
		SceneSubj* pSS = pSubjs->at(subjN);
		pSS->renderDepthMap(&shadowCamera);
		//checkGLerrors("pGS->renderDepthMap");
	}
	shadowsQueue.clear();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return 1;
}
int Shadows::addToShadowsQueue(std::vector<SceneSubj*>* pSubjs) {
	int subjsN = pSubjs->size();
	int added = 0;
	for (int sN = 0; sN < subjsN; sN++) {
		SceneSubj* pSS = pSubjs->at(sN);
		if (pSS == NULL)
			continue;
		if (pSS->hide > 0)
			continue;
		if (pSS->djTotalN < 1)
			continue;
		if (pSS->dropsShadow == 0)
			continue;
		shadowsQueue.push_back(pSS);
		added++;
	}
	return added;
}
