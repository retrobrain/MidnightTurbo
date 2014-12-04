#include "AppDelegate.h"
#include "MenuScene.h"
#include "GameScene.h"
#include "PreloadScene.h"
#include "Configs.h"

USING_NS_CC;

Size designResolutionSize = Size(800, 480);

float screenScale;
float guiScale;
Vec2  screenCenter;
int   screenHorizont;
Vec2  carStartPosition;
float startOffset;
float endOffset;
float scaleDt;
float carHeight;
float carWidth;
int   turnWorldOffset;
float rivalMaxZorder;

//use this to initialize some consts used based on the screen size of each single device
void initConfigByResolution()
{
    guiScale          = designResolutionSize.height / 640;
    screenScale       = designResolutionSize.height / 160;
    screenCenter      = Vec2(designResolutionSize.width * 0.5, designResolutionSize.height * 0.5);
    screenHorizont    = designResolutionSize.height * 0.60625;
    carStartPosition  = Vec2(designResolutionSize.width*0.5, designResolutionSize.height * 0.65);
    startOffset       =  designResolutionSize.width * 0.04;
    endOffset         =  designResolutionSize.width * 0.7;
    scaleDt           = 1.6 / designResolutionSize.height;
    carHeight         = designResolutionSize.height * 0.4375 / screenScale;
    carWidth          = carHeight * 1.77142857;
    turnWorldOffset   = 11 * screenScale;
    rivalMaxZorder    = designResolutionSize.height * 0.65;
}
\
//static variables
bool AppDelegate::isSoundEnabled;
bool AppDelegate::isShaderEnabled;
float AppDelegate::shaderTime;

//will use this to load some configs from file
static void initConfigsFromFile()
{
    AppDelegate::isShaderEnabled = true;
    AppDelegate::isSoundEnabled = true;
}

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
#ifdef DESKTOP
        glview->setFrameSize(designResolutionSize.width, designResolutionSize.height);
#endif
    }

#ifndef DESKTOP
    designResolutionSize = glview->getFrameSize();
#endif
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_HEIGHT);
    initConfigByResolution();

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    initConfigsFromFile();
    UserDefault::getInstance()->setBoolForKey("App_Paused", false);

    // create a scene. it's an autorelease object
    auto scene = PreloadScene::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    auto game = Director::getInstance()->getRunningScene()->getChildByName("game");
    auto isPaused = UserDefault::getInstance()->getBoolForKey("App_Paused"); //set this in GameScene
    if(game && !isPaused)
        dynamic_cast<GameScene*>(game)->EnterBackground();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

GLProgram* AppDelegate::ShaderEffect(const char *frag)
{
    GLchar * fragSource = (GLchar*) String::createWithContentsOfFile(
                FileUtils::getInstance()->fullPathForFilename(frag).c_str())->getCString();
    GLchar * vertexSource = (GLchar*) String::createWithContentsOfFile(
                FileUtils::getInstance()->fullPathForFilename("shaders/vertex.vsh").c_str())->getCString();

    auto program = GLProgram::createWithByteArrays(vertexSource, fragSource);

    return program;
}

float AppDelegate::GetShaderTime()
{
    auto fps = Director::getInstance()->getAnimationInterval();
    shaderTime += fps;
    return (shaderTime > 2.0) ? shaderTime -= shaderTime*2.0 : shaderTime;
}
