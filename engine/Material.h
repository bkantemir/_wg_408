#pragma once
#include "MyColor.h"
#include <string>

class Material
{
public:
    char materialName[32] = "";
    char shaderType[32] = "";
    int shaderN = -1;

    int takesShadow = 1; //if accepts shadows
    int dropsShadow = 1;
    //int shadowShaderN = -1;
    //unsigned int shadowVAOid = 0; //will hold data stream attributes mapping/positions

    int primitiveType = GL_TRIANGLES; //GL_POINTS; GL_LINES; GL_LINE_STRIP; GL_LINE_LOOP; GL_TRIANGLE_STRIP; GL_TRIANGLE_FAN; GL_TRIANGLES;
    MyColor uColor;
    MyColor uColor1; //for 2-tone
    MyColor uColor2; //for 2-tone
    int uTex0 = -1;
    int uTex1mask = -1;
    int uTex2nm = -1;
    //int uBinormalMissed = 0;
    int uTex3 = -1;
    int uTex1alphaChannelN = 3; //default - alpha channel for mask
    int uTex1alphaNegative = 0; //default - alpha channel not negative
    int uTex0translateChannelN = -1; //translate tex0 to tex3 by channelN. Default -1 - don't translate, 4-7 - 2-tone

    float uTexMod[4] = { -1,1,0,0 }; //uTexN to adjust, scale, dx,dy

    int uAlphaBlending = 0; //for semi-transparency
    float uAlphaFactor = 1; //for semi-transparency
    float uAmbient = 0.4f; //ambient light
    //specular light parameters
    float uSpecularIntencity = 0.8f;
    float uSpecularMinDot[2] = { 0.8f,2.0f };//0.98};
    float uSpecularPowerOf = 20.0f;
    float uBleach = 0.0f;
    float uShadingK = 1.0;

    float lineWidth = 1;
    int zBuffer = 1;
    int zBufferUpdate = 1;

    int uDiscardNormalsOut = 0;
    float uTranslucency = 0; //cabochon?

    int dontRender = 0; //guideline?

    float uFogLevel = 0.0;
    MyColor uFogColor;

    char layer2as[32] = ""; //"clearcoat", etc

    //static std::vector<Material*> materialsList;

public:
    virtual ~Material(); //destructor
    //static int cleanUp();
    //int pickShaderNumber() { return pickShaderNumber(this); };
    static int pickShaderNumber(Material* pMT);
    //void setShaderType(std::string needType) { setShaderType(this, needType); };
    static void setShaderType(Material* pMT, std::string needType) { strcpy_s(pMT->shaderType, 32, (char*)needType.c_str()); };
    void clear() { clear(this); };
    static void clear(Material* pMT);
    int assignShader(std::string needType) { return assignShader(this, needType); };
    static int assignShader(Material* pMT, std::string needType);
};
