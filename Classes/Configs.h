#ifndef ___CONFIGS_H___
#define ___CONFIGS_H___

#include "cocos2d.h"
#include <algorithm>

USING_NS_CC;

//Some consts used in the game

//MAIN_SETTINGSz
//#define DESKTOP
extern Size designResolutionSize;

extern float screenScale;
extern float guiScale;
extern Vec2  screenCenter;
extern int   screenHorizont;
extern Vec2  carStartPosition;
extern float startOffset;
extern float endOffset;
extern float scaleDt;
extern float carHeight;
extern float carWidth;
extern int   turnWorldOffset;
extern float rivalMaxZorder;

const float modelTurnSpeed    = 10;
const float gameAcceleration  = 0.007;
const int   skewWorldOffset   = 62;
const float turnSpeed         = 0.1;
const int   playerSkewY       = 5;
const int   playerSkewX       = 2;

//Z_ORDER
const int   parallaxZ         = -2;
const int   roadZ             = -1;
const int   maskZ             =  1;
const int   rivalLightsZ      =  2;
const int   playerZ           =  3;
const int   scanLinesZ        =  4;

#endif //___CONFIGS_H___
