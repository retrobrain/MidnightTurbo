#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer, 0, "game");

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    m_carSpeed    = 1.0;
    m_updateDt    = 0.3f;
    m_updateIncr  = 1.0f;

    auto keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    //----------------------------------------------------------------------------------------

    //all the game objects are attached to this layer
    m_pEffectLayer = Layer::create();
    m_pEffectLayer->setVisible(false);
    addChild(m_pEffectLayer);

    //all the menu objects are attached to this layer
    m_pMenuLayer = Layer::create();
    m_pMenuLayer->setVisible(false);
    addChild(m_pMenuLayer);

    //================Shaders===================
    m_pZoomShader = ShaderCache::getInstance()->getGLProgram("ZOOM");
    m_pCurrentGlState[ZOOM_SH] = GLProgramState::getOrCreateWithGLProgram(m_pZoomShader);
    m_pCurrentGlState[ZOOM_SH]->setUniformFloat("u_blurSize", m_score);
    m_pCurrentGlState[ZOOM_SH]->setUniformFloat("u_time", AppDelegate::GetShaderTime());

    auto vhsShader = ShaderCache::getInstance()->getGLProgram("VHS_INGAME");
    m_pCurrentGlState[VHS_SH] = GLProgramState::getOrCreateWithGLProgram(vhsShader);
    m_pCurrentGlState[VHS_SH]->setUniformFloat("u_time", AppDelegate::GetShaderTime());


    //============RenderTextureGame==============
    m_pRenderTex = RenderTexture::create(designResolutionSize.width, designResolutionSize.height, Texture2D::PixelFormat::RGBA8888);
    m_pRenderTex->setPosition(screenCenter);
    m_pRenderTex->getSprite()->getTexture()->setAliasTexParameters();
    if(AppDelegate::isShaderEnabled)
    {
        m_pRenderTex->getSprite()->setGLProgram(m_pZoomShader);
    }
    else
    {
        m_scanLines = Sprite::createWithSpriteFrameName("scan_lines.png");
        m_scanLines->setScale(guiScale);
        m_scanLines->setPosition(screenCenter);
        m_scanLines->setOpacity(50);
        m_pEffectLayer->addChild(m_scanLines, scanLinesZ + rivalMaxZorder);
    }
    m_pRenderTex->retain();

    //============RenderTextureMenu==============
    m_pRenderTexMenu = RenderTexture::create(designResolutionSize.width, designResolutionSize.height, Texture2D::PixelFormat::RGBA8888);
    m_pRenderTexMenu->setPosition(screenCenter);
    m_pRenderTexMenu->getSprite()->getTexture()->setAliasTexParameters();
    m_pRenderTexMenu->getSprite()->setGLProgram(vhsShader);
    m_pRenderTexMenu->retain();

    //============ParallaxNode===============
    m_pParallaxCity = ParallaxNode::create();
    char parallaxPath[100] = {0};
    for(int i = 1; i < 4; i++)
    {
        float offset = 1.0 / i;
        sprintf(parallaxPath, "parallax_%d.png",i);
        auto layer = Sprite::createWithSpriteFrameName(parallaxPath);
        layer->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        layer->setScale(screenScale);
        m_pParallaxCity->addChild(layer, 3 - i, Vec2(offset,offset), Vec2(screenCenter.x, screenCenter.y * 2.0));
    }
    m_pParallaxCity->setVisible(false);
    m_pEffectLayer->addChild(m_pParallaxCity, parallaxZ);
    m_pParallaxTurnLeft = MoveBy::create(0.1, Vec2(turnWorldOffset, 0.0));
    m_pParallaxTurnLeft->retain();
    m_pParallaxTurnRight = MoveBy::create(0.1, Vec2(-turnWorldOffset, 0.0));
    m_pParallaxTurnRight->retain();

    //============Road&Grass==================
    m_pRoad = Sprite::create();
    Vector<SpriteFrame*> animFrames;
    char roadPath[100] = {0};
    for(int i = 1; i <= 4; i++)
    {
        sprintf(roadPath, "road_%d.png",i);
        auto frame = Sprite::createWithSpriteFrameName(roadPath);
        frame->getTexture()->setAliasTexParameters();
        animFrames.pushBack(frame->getSpriteFrame());
    }
    auto animation = Animation::createWithSpriteFrames(animFrames, 0.05f, -1);
    auto animate = Animate::create(animation);
    m_pRoadAnimation = Speed::create(animate, 1.0);
    m_pRoad->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    m_pRoad->setPosition(screenCenter.x, screenHorizont);
    m_pRoad->setScale(screenScale);
    m_pEffectLayer->addChild(m_pRoad, roadZ);
    for(auto laneIter : m_bLaneInUse)
        laneIter = false;

    //============NightMask===================
    m_pNightMask = Sprite::createWithSpriteFrameName("world_mask.png");
    m_pNightMask->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    m_pNightMask->setPosition(carStartPosition);
    m_pNightMask->setScale(screenScale + 0.5);
    m_pEffectLayer->addChild(m_pNightMask, maskZ + rivalMaxZorder);

    //============PlayerCar===================
    m_pPlayerCar = Sprite::createWithSpriteFrameName("player_testa.png");
    m_pPlayerCar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    m_pPlayerCar->getTexture()->setAliasTexParameters();
    auto playerPosition = Vec2(screenCenter.x, NULL);
    m_pPlayerCar->setPosition(playerPosition);
    m_pPlayerCar->setScale(screenScale);

    auto rectWidth  = carWidth * screenScale * 0.5;
    auto rectHeight = carHeight / 2 * screenScale * 0.8;
    auto minX = screenCenter.x - rectWidth / 4;
    auto minY = NULL;
    auto maxX = screenCenter.x + rectWidth;
    auto maxY = rectHeight;
    m_playerRect.setRect(minX, minY, rectWidth / 2, rectHeight);

    m_pEffectLayer->addChild(m_pPlayerCar, playerZ + rivalMaxZorder);

    //============OpponentCars================
    CreateCar("lambo", 8);
    CreateCar("testa", 8);
    CreateCar("f40", 8);
    CreateCar("lambo", 2);
    CreateCar("testa", 2);
    CreateCar("f40", 2);

    //==================InGameMenu=======================
    CreateInGameMenu();

    //==================StartTrafficLight================
    auto tLight = Sprite::create();
    Vector<SpriteFrame*> trFrames;
    char trPath[100] = {0};
    for(int i = 1; i <= 4; i++)
    {
        sprintf(trPath, "trl_%d.png",i);
        auto trframe = Sprite::createWithSpriteFrameName(trPath);
        trFrames.pushBack(trframe->getSpriteFrame());
    }
    auto trAnimation = Animation::createWithSpriteFrames(trFrames, 0.5f, 1);
    auto trAnimate = Animate::create(trAnimation);
    tLight->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    tLight->setPosition(screenCenter.x, screenCenter.y*2);
    tLight->setScale(guiScale);
    tLight->setOpacity(200);
    auto trMove = MoveBy::create(0.2, Vec2(0.0, trFrames.at(0)->getOriginalSize().height));
    auto showMenuLayerCallback = CallFunc::create(std::bind(&Layer::setVisible, m_pMenuLayer, true));
    auto removeTrafficLight = CallFunc::create(std::bind(&Layer::removeChildByName, m_pEffectLayer, "trafficLight", true));
    auto seq = Sequence::create(trAnimate, trMove, showMenuLayerCallback, removeTrafficLight, nullptr);
    tLight->runAction(seq);
    m_pEffectLayer->addChild(tLight, rivalLightsZ, "trafficLight");

    RunGame();
    srand(time(NULL));
    return true;
}

void GameScene::CreateInGameMenu()
{
    //=================ScoresBgr================
    auto scoresBgr = Sprite::createWithSpriteFrameName("score_bgr.png");
    scoresBgr->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    scoresBgr->setPosition(screenCenter.x, screenCenter.y * 4);
    scoresBgr->setOpacity(50.0);
    scoresBgr->setScale(guiScale);
    m_pMenuLayer->addChild(scoresBgr);

    //=================Scores===================
    m_pScoreTextW = LabelTTF::create("000", "fonts/Pixel.ttf", 10 * screenScale);
    m_pScoreTextW->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    m_pScoreTextW->setPosition(screenCenter.x, (designResolutionSize.height - m_pScoreTextW->getContentSize().height));
    m_pScoreTextR = LabelTTF::create("000", "fonts/Pixel.ttf", 10.88 * screenScale);
    m_pScoreTextR->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    m_pScoreTextR->setPosition(screenCenter.x, (designResolutionSize.height - m_pScoreTextW->getContentSize().height));
    m_pScoreTextR->setColor(Color3B::RED);
    m_pMenuLayer->addChild(m_pScoreTextR);
    m_pMenuLayer->addChild(m_pScoreTextW);

    //=================PauseButton==============
    auto pauseSprite = Sprite::createWithSpriteFrameName("pause.png");
    pauseSprite->setOpacity(100.0);
    auto pauseSpriteAct = Sprite::createWithSpriteFrameName("pause.png");
    pauseSpriteAct->setOpacity(200.0);
    auto pauseButton = MenuItemSprite::create(pauseSprite, pauseSpriteAct, CC_CALLBACK_1(GameScene::PauseGame, this));
    pauseButton->setScale(guiScale);
    pauseButton->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    pauseButton->setPosition(screenCenter * 1.9);

    //================BackToMainMenu=============
    auto stopSprite = Sprite::createWithSpriteFrameName("stop.png");
    stopSprite->setOpacity(100.0);
    auto stopSpriteAct = Sprite::createWithSpriteFrameName("stop.png");
    stopSpriteAct->setOpacity(200.0);
    auto stopButton = MenuItemSprite::create(stopSprite, stopSpriteAct, CC_CALLBACK_1(GameScene::GoMainMenu, this));
    stopButton->setScale(guiScale);
    stopButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    stopButton->setPosition(0.0 + stopSprite->getContentSize().width, screenCenter.y * 2 + stopSprite->getContentSize().height);

    //================ReplayButton================
    auto replaySprite = Sprite::createWithSpriteFrameName("replay.png");
    replaySprite->setOpacity(100.0);
    auto replaySpriteAct = Sprite::createWithSpriteFrameName("replay.png");
    replaySpriteAct->setOpacity(200.0);
    auto replayButton = MenuItemSprite::create(replaySprite, replaySpriteAct, CC_CALLBACK_1(GameScene::ReplayGame, this));
    replayButton->setScale(guiScale);
    replayButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    replayButton->setPosition(screenCenter.x*2 - replaySprite->getContentSize().width, screenCenter.y * 2 + replaySprite->getContentSize().height);

    //================ResumeButton================
    auto resumeSprite = Sprite::createWithSpriteFrameName("resume.png");
    resumeSprite->setOpacity(100.0);
    auto resumeSpriteAct = Sprite::createWithSpriteFrameName("resume.png");
    resumeSpriteAct->setOpacity(200.0);
    auto resumeButton = MenuItemSprite::create(resumeSprite, resumeSpriteAct, CC_CALLBACK_1(GameScene::ResumeGame, this));
    resumeButton->setScale(guiScale);
    resumeButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    resumeButton->setPosition(screenCenter.x*2 - resumeSprite->getContentSize().width, screenCenter.y * 3 + resumeSprite->getContentSize().height);
    resumeButton->setName("resume_button");

    m_pInGameMenu = Menu::create(pauseButton, stopButton, replayButton, resumeButton, NULL);
    m_pInGameMenu->setPosition(Vec2::ZERO);
    m_pMenuAction = MoveBy::create(0.2, Vec2(0.0, -screenCenter.y*2));
    m_pMenuAction->retain();
    m_pMenuLayer->addChild(m_pInGameMenu);
}

void GameScene::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    if(m_isRunning)
    {
        auto delta = Director::getInstance()->getDeltaTime();

        delta*=m_carSpeed; // need this for FPS independant movement of the rival cars
        m_updateIncr += delta;

        if(m_updateIncr >= m_updateDt)
            m_updateIncr = LaunchCar(); //if success returns 0, if not 1

        for(auto iter : m_vDisponibleCars)
        {
            if(iter->IsLaunched())
            {
                iter->MoveCar(delta);
                auto rRect = iter->GetRect();
                if(m_playerRect.intersectsRect(rRect))
                {
                    StopMovement();
                }
                else if(iter->getPositionY() < NULL)
                {
                    UpdateScore();

                    if(AppDelegate::isShaderEnabled)
                        m_pCurrentGlState[ZOOM_SH]->setUniformFloat("u_blurSize",(m_score));

                    m_carSpeed += gameAcceleration;
                    m_pRoadAnimation->setSpeed(m_carSpeed);
                }
            }
        }
        if(AppDelegate::isShaderEnabled)
            m_pCurrentGlState[ZOOM_SH]->setUniformFloat("u_time", AppDelegate::GetShaderTime());

        m_pRenderTex->beginWithClear(0, 0, 0, 0);
        m_pEffectLayer->visit();
        m_pRenderTex->end();
        m_pRenderTex->visit(renderer, parentTransform, true);
        m_pMenuLayer->visit();
    }
    else
    {
        m_pCurrentGlState[VHS_SH]->setUniformFloat("u_time", AppDelegate::GetShaderTime());
        m_pRenderTexMenu->beginWithClear(0, 0, 0, 0);
        m_pCrashScene->visit();
        m_pMenuLayer->visit();
        m_pRenderTexMenu->end();
        m_pRenderTexMenu->visit(renderer, parentTransform, true);
    }
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event *event)
{

    switch ( keyCode )
    {
    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        Turn(LEFT);

        break;

    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        Turn(RIGHT);
        break;

    case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        Director::getInstance()->resume();
        break;

    case EventKeyboard::KeyCode::KEY_UP_ARROW:
        Director::getInstance()->pause();
        break;

    case EventKeyboard::KeyCode::KEY_SPACE:
        ReplayGame(this);
        break;
    }
}

bool GameScene::onTouchBegan(Touch *touch, Event *unused_event)
{
    if(touch->getLocation().x < screenCenter.x)
        Turn(LEFT);
    else
        Turn(RIGHT);
}

void GameScene::CreateCar(const char *carName, int units)
{
    char fullName[100] = {0};
    int id = 0;
    for(int i = 1; i < units + 1; i++)
    {
        id = i;
        if(id > 4)
            id -= 4;

        sprintf(fullName, "%s_%d.png", carName,id);
        auto rivalCar = RivalCar::CreateCar(m_pEffectLayer, fullName);
        sprintf(fullName, "%s_light.png", carName);
        rivalCar->CreateLights(fullName);
        m_vDisponibleCars.pushBack(rivalCar);
    }
}

void GameScene::RunGame()
{
    m_pParallaxCity->setVisible(true);
    m_pRoad->runAction(m_pRoadAnimation);

    auto playerAnimation = MoveBy::create(0.1, Vec2(0.0, -1.0 * screenScale));
    auto seq = Sequence::createWithTwoActions(playerAnimation, playerAnimation->reverse());
    auto parallaxAnimation = MoveBy::create(0.05, Vec2(0.0, 0.5 * screenScale));
    auto seq2 = Sequence::createWithTwoActions(parallaxAnimation, parallaxAnimation->reverse());

    m_pPlayerAnimate = Speed::create(RepeatForever::create(seq), 1.0);
    m_pParallaxAnimate = Speed::create(RepeatForever::create(seq2), 1.0);

    m_pPlayerCar->runAction(m_pPlayerAnimate);
    m_pParallaxCity->runAction(m_pParallaxAnimate);
    m_isRunning = true;
    m_pEffectLayer->setVisible(true);
}

int GameScene::LaunchCar()
{
    auto *lane = &m_bLaneInUse[1]; //get the ptr to the middle element
    int laneRandomize = rand()%3 + (-1); //get random lane -1/0/1

    if(!lane[laneRandomize])
    {
        auto randomObject = m_vDisponibleCars.getRandomObject();
        if(!randomObject->IsLaunched())
        {
            float startLane = laneRandomize * startOffset;
            randomObject->Run(startLane, &lane[laneRandomize], m_carSpeed); //a car takes a ptr to the lane used blocking the lane
        }                                                                   //for other cars. Later its released by the car.
        return 0;
    }
    else
        return 1; //
}

void GameScene::Turn(int turnDirection)
{
    if(m_currentLane != turnDirection && m_pRoad->numberOfRunningActions() == 1)
    {
        m_currentLane += turnDirection;

        auto parallaxTurn = MoveBy::create(turnSpeed, Vec2(turnWorldOffset * 0.2 * -turnDirection, 0.0));
        auto roadTurn     = Spawn::create(SkewBy::create(turnSpeed * 0.1, 0, skewWorldOffset * turnDirection),
                                          MoveBy::create(turnSpeed * 0.05, Vec2(turnWorldOffset * -turnDirection, 0.0)),
                                          nullptr);
        roadTurn->setDuration(turnSpeed);

        auto  playerSkew   = SkewTo::create(turnSpeed, playerSkewX * -turnDirection, playerSkewY * turnDirection);
        auto  playerSkewR  = SkewTo::create(turnSpeed, 0.0, 0.0);
        auto  playerTurn   = Sequence::createWithTwoActions(playerSkew, playerSkewR);

        m_pParallaxCity->runAction(parallaxTurn);
        m_pRoad->runAction(roadTurn);
        m_pPlayerCar->runAction(playerTurn);

        for(auto iter : m_vDisponibleCars)
            iter->Turn(turnDirection);
    }
}

void GameScene::UpdateScore()
{
    char tmp[10];

    m_score++;

    auto third  = m_score % 10;
    auto second = (m_score / 10) % 10;
    auto first  = (m_score / 100) % 10;
    sprintf(tmp, "%d%d%d", first, second, third);

    m_pScoreTextW->setString(tmp);
    m_pScoreTextR->setString(tmp);

    auto scoreTextAnimation = ScaleBy::create(0.1, 1.1);
    auto seqAnimation = Sequence::createWithTwoActions(scoreTextAnimation, scoreTextAnimation->reverse());

    if(!m_pScoreTextW->getNumberOfRunningActions())
        m_pScoreTextW->runAction(seqAnimation);
}

void GameScene::EnterBackground()
{
    PauseGame(this);
}

void GameScene::PauseGame(Ref *Sender)
{
    m_paused = true;
    StopMovement();
}

void GameScene::ResumeGame(Ref *Sender)
{
    //use this in AppDelegate in applicationDidEnterBackground() func
    UserDefault::getInstance()->setBoolForKey("App_Paused", false);

    m_isRunning = true;
    m_paused = false;
    m_pMenuAction->setDuration(0.2);
    m_pMenuLayer->runAction(m_pMenuAction->reverse());
    m_pScoreTextR->setVisible(true);

    m_pScoreTextW->setFontSize(10 * screenScale);
    m_pScoreTextW->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    m_pScoreTextW->setPosition(screenCenter.x, (designResolutionSize.height - m_pScoreTextW->getContentSize().height));

    m_pRoad->resume();
    m_pParallaxCity->resume();
    m_pPlayerCar->resume();
}

void GameScene::GoMainMenu(Ref *Sender)
{
    //use this in AppDelegate in applicationDidEnterBackground() func
    UserDefault::getInstance()->setBoolForKey("App_Paused", false);

    auto director = Director::getInstance();

    m_pRenderTexMenu->release();
    m_pParallaxTurnLeft->release();
    m_pParallaxTurnRight->release();
    m_pMenuAction->release();
    m_pRenderTex->release();

    if(!m_isRunning)
        m_pCrashScene->release();

    auto menu = MenuScene::createScene();
    director->replaceScene(menu);
}


void GameScene::ReplayGame(Ref *Sender)
{
    //use this in AppDelegate in applicationDidEnterBackground() func
    UserDefault::getInstance()->setBoolForKey("App_Paused", false);

    auto director = Director::getInstance();

    m_pRenderTexMenu->release();
    m_pParallaxTurnLeft->release();
    m_pParallaxTurnRight->release();
    m_pMenuAction->release();
    m_pRenderTex->release();

    if(!m_isRunning)
        m_pCrashScene->release();

    auto game = GameScene::createScene();
    director->replaceScene(game);
}

void GameScene::StopMovement()
{
    //use this in AppDelegate in applicationDidEnterBackground() func
    UserDefault::getInstance()->setBoolForKey("App_Paused", true);
    m_isRunning = false;

    if(!m_paused)
    {
        m_pInGameMenu->getChildByName("resume_button")->setVisible(false);
    }

    m_pRoad->pause();
    m_pParallaxCity->pause();
    m_pPlayerCar->pause();

    m_pScoreTextR->setVisible(false);
    m_pScoreTextW->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    m_pScoreTextW->setPosition(screenCenter.x, screenCenter.y * 2.1);
    m_pScoreTextW->setFontSize(20 * screenScale);


    m_pCrashScene = Sprite::createWithTexture(m_pRenderTex->getSprite()->getTexture());
    m_pCrashScene->setPosition(screenCenter);
    m_pCrashScene->setFlippedY(true);

    m_pCurrentGlState[ZOOM_SH]->setUniformFloat("u_blurSize",m_score);
    m_pCrashScene->setGLProgram(m_pZoomShader);

    m_pCrashScene->retain();
    m_pMenuAction->setDuration(0.2);
    m_pMenuLayer->runAction(m_pMenuAction);
}
