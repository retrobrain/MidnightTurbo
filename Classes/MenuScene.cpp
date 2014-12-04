#include "MenuScene.h"

Scene* MenuScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = MenuScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //----------------------------------------------------------------------------------------

    m_pTitleMenu = Layer::create();
    addChild(m_pTitleMenu);

    //============RenderTexture/Shader================
    m_pRenderTexture = RenderTexture::create(designResolutionSize.width, designResolutionSize.height, Texture2D::PixelFormat::RGBA8888);
    m_pRenderTexture->setPosition(screenCenter);
    m_pRenderTexture->getSprite()->getTexture()->setAliasTexParameters();
    auto vhsEffect = ShaderCache::getInstance()->getGLProgram("VHS");
    m_pCurrentGLState = GLProgramState::getOrCreateWithGLProgram(vhsEffect);
    m_pCurrentGLState->setUniformFloat("u_time", AppDelegate::GetShaderTime());
    m_pRenderTexture->getSprite()->setGLProgram(vhsEffect);
    m_pRenderTexture->retain();

    //============MainMenuBgr======================
    Sprite *turbo = Sprite::createWithSpriteFrameName("turbo.png");
    turbo->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    turbo->setPosition(screenCenter.x, screenCenter.y*2);
    turbo->setScale(guiScale);
    m_pTitleMenu->addChild(turbo);
    auto menuCar = Sprite::createWithSpriteFrameName("player_testa.png");
    menuCar->setScale(0.01);
    menuCar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    menuCar->setPosition(screenCenter);
    menuCar->runAction(ScaleBy::create(1.0, screenScale / 0.01));
    m_pTitleMenu->addChild(menuCar, 0);

    //=============ScoresBgr===========================
    auto scoresBgr = Sprite::createWithSpriteFrameName("score_bgr.png");
    scoresBgr->setPosition(screenCenter.x, -screenCenter.y);
    scoresBgr->setOpacity(50.0);
    scoresBgr->setScale(guiScale);
    m_pTitleMenu->addChild(scoresBgr);

    //    //=============StartButton=========================
    auto startLabel = Label::createWithTTF("Start", "fonts/SPEERG.ttf", 15*screenScale);
    startLabel->runAction(RepeatForever::create(Blink::create(1.0, 1)));
    auto startButton = MenuItemLabel::create(startLabel, CC_CALLBACK_1(MenuScene::menuStartCallback,this));
    startButton->setPosition(screenCenter.x, screenCenter.y * 0.45);

    //=============ScoresButton========================
    auto scoresSprite = Sprite::createWithSpriteFrameName("world_score.png");
    scoresSprite->setOpacity(100.0);
    auto scoresSpriteAct = Sprite::createWithSpriteFrameName("world_score.png");
    scoresSpriteAct->setOpacity(200.0);
    auto scoresButton = MenuItemSprite::create(scoresSprite, scoresSpriteAct, CC_CALLBACK_1(MenuScene::menuScoresCallback,this));
    scoresButton->setScale(guiScale);
    scoresButton->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    scoresButton->setPosition(0.0 + scoresSprite->getContentSize().width/2, 0.0 + scoresSprite->getContentSize().height*1.5);

    //=============BackFromScoresButton================
    auto backToMenuLabel = Label::createWithTTF("back", "fonts/Pixel.ttf", 10*screenScale);
    backToMenuLabel->setOpacity(100.0);
    auto backToMenuButton = MenuItemLabel::create(backToMenuLabel, CC_CALLBACK_1(MenuScene::menuBackToMenu,this));
    backToMenuButton->setPosition(0.0 + backToMenuLabel->getContentSize().width/1.5, 0.0 - backToMenuLabel->getContentSize().height);

    //=============SoundButton=========================
    auto soundSpriteOn = Sprite::createWithSpriteFrameName("sound.png");
    soundSpriteOn->setOpacity(100.0);
    auto soundSpriteOff = Sprite::createWithSpriteFrameName("sound.png");
    soundSpriteOff->setOpacity(50.0);
    MenuItem *soundToggleOn = MenuItemSprite::create(soundSpriteOn, soundSpriteOn);
    MenuItem *soundToggleOff = MenuItemSprite::create(soundSpriteOff, soundSpriteOff);
    auto soundButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(MenuScene::soundButtonTaped, this), soundToggleOn, soundToggleOff, nullptr);
    soundButton->setScale(guiScale);
    soundButton->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    soundButton->setPosition(designResolutionSize.width - soundSpriteOff->getContentSize().width/2, 0.0 + soundSpriteOff->getContentSize().height*1.5);
    auto state = AppDelegate::isSoundEnabled;
    soundButton->setSelectedIndex(!state);

    //=================MainMenu============================
    m_pMenu = Menu::create(startButton, soundButton, scoresButton, backToMenuButton, NULL);
    m_pMenu->setPosition(0.0, 0.0);
    m_pTitleMenu->addChild(m_pMenu, 1);

//    m_pSlideSfx = CocosDenshion::SimpleAudioEngine::getInstance();
//    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("audio/music.ogg");

    return true;
}

void MenuScene::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    //use this to get avverage fps and turn on/off the main game shader
    auto uTime = AppDelegate::GetShaderTime();
    m_pCurrentGLState->setUniformFloat("u_time", uTime);
    m_totalFps += Director::getInstance()->getFrameRate();
    m_fpsCounter++;

    m_pRenderTexture->beginWithClear(0.09, 0.094, 0.176, 0.0);
    m_pTitleMenu->visit(renderer, parentTransform, true);
    m_pRenderTexture->end();

    m_pRenderTexture->visit(renderer, parentTransform, true);
}

void MenuScene::menuStartCallback(Ref *Sender)
{
    auto fpsOnMenu = m_totalFps/m_fpsCounter;
    if(fpsOnMenu < 30.0)
        AppDelegate::isShaderEnabled = false;

    m_pRenderTexture->release();

    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(scene);
}

void MenuScene::menuScoresCallback(Ref *Sender)
{
//    m_pSlideSfx->playEffect("audio/passingBy_sfx.ogg");

    m_pTitleMenu->runAction(MoveBy::create(0.5, Vec2(0.0, screenCenter.y * 2.0)));
}

void MenuScene::menuBackToMenu(Ref *Sender)
{
    m_pTitleMenu->runAction(MoveBy::create(0.5, Vec2(0.0, -screenCenter.y * 2.0)));
}

void MenuScene::soundButtonTaped(Ref *Sender)
{
    AppDelegate::isSoundEnabled = !AppDelegate::isSoundEnabled;
}

