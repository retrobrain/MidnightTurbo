#include "AppMacros.h"
#include "MenuScene.h"
#include "GameScene.h"
#include "PreloadScene.h"

USING_NS_CC;

Scene* PreloadScene::createScene()
{
    auto scene = Scene::create();
    auto layer = PreloadScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool PreloadScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    _labelLoading = Label::createWithTTF("loading...", "fonts/Pixel.ttf", 10 * screenScale);
    _labelLoading->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _labelLoading->setPosition(screenCenter);

    addChild(_labelLoading);
    scheduleUpdate();
    return true;
}

void PreloadScene::update(float dt)
{
    if(!m_dataStartLoading)
        PreloadData();

    if(m_dataFinishLoading)
    {
        auto director = Director::getInstance();
        auto menu = MenuScene::createScene();
        director->replaceScene(menu);
    }
}

void PreloadScene::PreloadData()
{
    m_dataStartLoading = true;
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("audio/player_sfx.ogg");
//    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("audio/explode_sfx.ogg");
//    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("audio/breaks_sfx.ogg");
//    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("audio/passingBy_sfx.ogg");
//    CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("audio/music.ogg");

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("textures/gui_objects.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("textures/world_objects.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("textures/light_objects.plist");

    auto vhsProgram = AppDelegate::ShaderEffect("shaders/vhs.fsh");
    auto vhsInGame = AppDelegate::ShaderEffect("shaders/vhs_ingame.fsh");
    auto zoomShader = AppDelegate::ShaderEffect("shaders/zoom_blur.fsh");
    auto zoomGlState = GLProgramState::getOrCreateWithGLProgram(zoomShader);
    zoomGlState->setUniformVec2("u_blurCenter", Vec2(0.5,0.67));


    ShaderCache::getInstance()->addGLProgram(vhsProgram, "VHS");
    ShaderCache::getInstance()->addGLProgram(zoomShader, "ZOOM");
    ShaderCache::getInstance()->addGLProgram(vhsInGame, "VHS_INGAME");
    m_dataFinishLoading = true;
}
