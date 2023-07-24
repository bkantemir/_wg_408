#pragma once
#include <vector>
#include "SceneSubj.h"
#include "UISubj.h"
#include "Camera.h"

class TheApp
{
public:
	uint32_t frameN = 0;
	//synchronization
	uint64_t lastFrameMillis = 0;
	int targetFPS = 30;
	int millisPerFrame = 1000 / targetFPS;

	bool bExitApp;
	Camera mainCamera;
	float dirToMainLight[4] = { 1,1,1,0 };

	//static arrays (vectors) of active SceneSubj
	static std::vector<SceneSubj*> sceneSubjs;
public:
	int run();
	int getReady();
	int drawFrame();
	int cleanUp();
	int onScreenResize(int width, int height);
	static SceneSubj* newSceneSubj(std::string subjClass, std::string sourceFile="",
		std::vector<SceneSubj*>* pSubjsSet0 = NULL, std::vector<DrawJob*>* pDrawJobs0 = NULL);
};
