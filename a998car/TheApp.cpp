#include "TheApp.h"
#include "platform.h"
#include "utils.h"
#include "linmath.h"
#include "Texture.h"
//#include "gltf/GLTFskinShader.h"
#include "DrawJob.h"
#include "ModelBuilder.h"
#include "TexCoords.h"
#include "ModelLoader.h"
#include "model_car/CarWheel.h"
#include "Shadows.h"
#include "ProgressBar.h"


extern std::string filesRoot;
extern float degrees2radians;

std::vector<SceneSubj*> TheApp::sceneSubjs;

int TheApp::getReady() {

    Shader::loadBasicShaders();
    UISubj::init();

    ProgressBar* pPBar = new ProgressBar(filesRoot + "/dt/cfg/progressbar00.bin");

    bExitApp = false;
    frameN = 0;
    Shader::loadShaders(pPBar);
    //GLTFskinShader::loadGLTFshaders(pPBar);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);//default
    glFrontFace(GL_CCW);//default

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);

    
    //int subjN = ModelLoader::loadModel(&sceneSubjs, NULL, NULL, "/dt/test01.txt", "", pPBar);
    //int subjN = ModelLoader::loadModel(&sceneSubjs, NULL, NULL, "/dt/models/cars/999_1935_deusenberg_ssj/root01.txt", "",pPBar);
    int subjN = ModelLoader::loadModel(&sceneSubjs, NULL, NULL, "/dt/models/cars/998_1929_deusenberg_j/root01_gatsby.txt", "",pPBar);
    SceneSubj* pSS0 = sceneSubjs.at(subjN);
    pSS0->ownSpeed.setEulerDg(0,1,0);
    pSS0->ownCoords.setEulerDg(0,-90,0);
    mat4x4_from_quat(pSS0->ownCoords.rotationMatrix, pSS0->ownCoords.getRotationQuat());
    pSS0->setRoot(pSS0);
    pSS0->onDeploy();
    /*

    int subjN = ModelLoader::loadModel(&sceneSubjs, NULL,NULL,"/dt/models/misc/999_marlboro01red/root01.txt", "", pPBar);
    //int subjN = ModelLoader::loadModel(&gameSubjs, "/dt/models/misc/999_marlboro01red/root03blank.txt", "", pPBar);
    SceneSubj* pSS0 = sceneSubjs.at(subjN);
    pSS0->ownSpeed.setEulerDg(0,1,0);
    pSS0->ownCoords.setEulerDg(0,-90,0);
    mat4x4_from_quat(pSS0->ownCoords.rotationMatrix, pSS0->ownCoords.getRotationQuat());
    pSS0->setRoot(pSS0);
    
    for (int carN = 0; carN < 4; carN++) {
        float scale = 64.0f / 150;
        if (getRandom(0, 1) == 0) {
            subjN = ModelLoader::loadModel(&sceneSubjs, NULL, NULL, "/dt/models/cars/999_1935_deusenberg_ssj/root01.txt", "", pPBar);
        }
        else {
            subjN = ModelLoader::loadModel(&sceneSubjs, NULL, NULL, "/dt/models/cars/998_1929_deusenberg_j/root01.txt", "", pPBar);
            scale *= 1.05;
        }
        SceneSubj* pSS = sceneSubjs.at(subjN);
        pSS->scaleMe(scale);
        pSS->ownCoords.pos[1] = 0;// 0.4267 * 6;
        pSS->ownCoords.pos[2] = (82.0f / 4.5f) * ((float)carN - 1.5f);
        pSS->ownCoords.setEulerDg(0,60,0);
        mat4x4_from_quat(pSS->ownCoords.rotationMatrix, pSS->ownCoords.getRotationQuat());
        pSS->pStickTo = pSS0;
        pSS->setRoot(pSS);
        pSS->onDeploy();
    }
    */

    //===== set up camera
    mainCamera.ownCoords.setEulerDg(30, 180, 0); //set camera angles/orientation
    mat4x4_from_quat(mainCamera.ownCoords.rotationMatrix, mainCamera.ownCoords.getRotationQuat());

    mainCamera.viewRangeDg = 20;// 20;
    mainCamera.stageSize[0] = 85;
    mainCamera.stageSize[1] = 45;
    //memcpy(mainCamera.lookAtPoint, pGS->ownCoords.pos, sizeof(float) * 3);
    mainCamera.lookAtPoint[1] = 4;// 4;
    mainCamera.reset();

    //===== set up light
    v3set(dirToMainLight, -1, 1, 0.5);
    v3norm(dirToMainLight);

    Shadows::init();

    pPBar->nextStep(pPBar);
    UISubj::cleanUp();

    //======= set UI
    //UISubj::addDepthMapSubj("test", 20, 10, 800, 800, "top left", Shadows::depthMapTexN);

    return 1;
}
int TheApp::drawFrame() {
    myPollEvents(); 

    //scan subjects
    int subjsN = sceneSubjs.size();
    for (int subjN = 0; subjN < subjsN; subjN++) {
        SceneSubj* pSS = sceneSubjs.at(subjN);
        if (pSS == NULL)
            continue;
        if (pSS->hide > 0)
            continue;

        pSS->processSubj();
    }

    //shadows
    Shadows::addToShadowsQueue(&sceneSubjs);
    Shadows::renderDepthMap();

    //set render to screen
    glViewport(0, 0, mainCamera.targetSize[0], mainCamera.targetSize[1]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDepthMask(GL_TRUE);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //translucent light
    float vecToLightInEyeSpace[4];
    mat4x4_mul_vec4plus(vecToLightInEyeSpace, (vec4*)mainCamera.mViewProjection, dirToMainLight, 0);
    float dirToTranslucent[3];
    if (vecToLightInEyeSpace[2] <= 0) { //light from behind
        for (int i = 0; i < 3; i++) {
            if (i == 2)
                dirToTranslucent[i] = dirToMainLight[i];
            else
                dirToTranslucent[i] = -dirToMainLight[i];
        }
    }
    else { //light to eyes
        for (int i = 0; i < 3; i++) {
            dirToTranslucent[i] = -dirToMainLight[i];
        }
    }
    dirToTranslucent[2] *= 0.5;
    v3norm(dirToTranslucent);

    //calculate screen coords/gabarites
    for (int subjN = 0; subjN < subjsN; subjN++) {
        SceneSubj* pSS = sceneSubjs.at(subjN);
        if (pSS == NULL)
            continue;
        if (pSS->hide > 0)
            continue;
        pSS->getScreenPosition(pSS, mainCamera.mViewProjection);// , & mainCamera, dirToMainLight);
    }


    //sort for rendering
    std::vector<SceneSubj*> renderQueueOpaque;
    std::vector<SceneSubj*> renderQueueTransparent;
    SceneSubj::addToRenderQueue(&renderQueueOpaque, &renderQueueTransparent, &sceneSubjs);

    //draw opaque subjects
    SceneSubj::sortRenderQueue(&renderQueueOpaque, 0); //0-closer 1st
    subjsN = renderQueueOpaque.size();
    for (int subjN = 0; subjN < subjsN; subjN++) {
        SceneSubj* pSS = renderQueueOpaque.at(subjN);
        pSS->render(&mainCamera, dirToMainLight, dirToTranslucent, 0); //0 - render opaque only
    }
    renderQueueOpaque.clear();

    //draw transparent subjects
    SceneSubj::sortRenderQueue(&renderQueueTransparent, 1); //1-farther 1st
    subjsN = renderQueueTransparent.size();
    for (int subjN = 0; subjN < subjsN; subjN++) {
        SceneSubj* pSS = renderQueueTransparent.at(subjN);
        pSS->render(&mainCamera, dirToMainLight, dirToTranslucent, 1); //1 - render transparent only
    }
    renderQueueTransparent.clear();

    UISubj::renderAll();

    //synchronization
    while (1) {
        uint64_t currentMillis = getSystemMillis();
        uint64_t millisSinceLastFrame = currentMillis - lastFrameMillis;
        if (millisSinceLastFrame >= millisPerFrame) {
            lastFrameMillis = currentMillis;
            break;
        }
    }
    mySwapBuffers();
    frameN++;
    return 1;
}

int TheApp::cleanUp() {
    int itemsN = sceneSubjs.size();
    //delete all UISubjs
    for (int i = 0; i < itemsN; i++)
        delete sceneSubjs.at(i);
    sceneSubjs.clear();
    //clear all other classes
    MaterialAdjust::cleanUp();
    Texture::cleanUp();
    Shader::cleanUp();
    DrawJob::cleanUp();
    MyColor::cleanUp();
    UISubj::cleanUp();
    return 1;
}
int TheApp::onScreenResize(int width, int height) {
    if (mainCamera.targetSize[0] == width && mainCamera.targetSize[1] == height)
        return 0;
    mainCamera.onTargetResize(width, height);
    UISubj::onScreenResize(width, height);
    mylog(" screen size %d x %d\n", width, height);
    return 1;
}

SceneSubj* TheApp::newSceneSubj(std::string subjClass, std::string sourceFile,
    std::vector<SceneSubj*>* pSubjsSet0, std::vector<DrawJob*>* pDrawJobs0) {
    SceneSubj* pSS = NULL;
    if (subjClass.compare("") == 0)
        pSS = (new SceneSubj());
    else if (subjClass.find("Car") == 0) {
        if (subjClass.compare("CarWheel") == 0)
            pSS = (new CarWheel());
    }
    if (pSS == NULL) {
        mylog("ERROR in TheProject::newSceneSubj. %s class not found\n", subjClass.c_str());
        return NULL;
    }
    strcpy_s(pSS->className, 32, subjClass.c_str());
    strcpy_s(pSS->source, 256, sourceFile.c_str());
    if (pDrawJobs0 != NULL)
        pSS->pDrawJobs = pDrawJobs0;
    if (pSubjsSet0 != NULL) {
        pSS->nInSubjsSet = pSubjsSet0->size();
        pSS->pSubjsSet = pSubjsSet0;
        pSubjsSet0->push_back(pSS);
    }
    return pSS;
}

int TheApp::run() {
    getReady();
    while (!bExitApp) {
        drawFrame();
    }
    cleanUp();
    return 1;
}
