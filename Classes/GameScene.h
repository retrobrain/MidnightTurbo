#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "MenuScene.h"
#include "AppDelegate.h"
#include "Configs.h"
#include "RivalCar.h"

//the three lanes values
enum lane {LEFT = -1, CENTER, RIGHT};

//used in array of GLProgramm ptr
enum GLPState{ZOOM_SH, VHS_SH};

class GameScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init(); 

    //create in-game menu object
    void CreateInGameMenu();
    
    // because of use of some shaders was better to override this, avoiding the double visit thingy
    virtual void visit(Renderer* renderer, const Mat4 &parentTransform, uint32_t parentFlags);
    
    //to be used only in the desktop ver.
    virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    
    //tap to turn
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);

    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

    //creates rival cars and pushes to a conteiner
    void CreateCar(const char *carName, int units);

    //runs the game. sets visible the layers, runs animations
    void RunGame();

    //launch a random rival car on a random lane
    int  LaunchCar();


    void Turn(int turnDirection);
    void UpdateScore();
    void EnterBackground();
    void PauseGame(Ref *Sender);
    void ResumeGame(Ref *Sender);
    void GoMainMenu(Ref *Sender);
    void ReplayGame(Ref *Sender);
    void StopMovement();

private:
    cocos2d::RenderTexture *m_pRenderTex;
    cocos2d::RenderTexture *m_pRenderTexMenu;
    Layer                  *m_pEffectLayer;
    Layer                  *m_pMenuLayer;
    Sprite                 *m_pCrashScene;
    Menu                   *m_pInGameMenu;
    Sprite                 *m_scanLines;
    GLProgram              *m_pZoomShader;
    GLProgramState         *m_pCurrentGlState[2];
    LabelTTF               *m_pScoreTextW;
    LabelTTF               *m_pScoreTextR;

    ParallaxNode           *m_pParallaxCity;
    Sprite                 *m_pRoad;
    Sprite                 *m_pPlayerCar;
    Speed                  *m_pRoadAnimation;
    Speed                  *m_pPlayerAnimate;
    Speed                  *m_pParallaxAnimate;
    Sprite                 *m_pNightMask;
    Vector<RivalCar*>       m_vDisponibleCars;

    FiniteTimeAction       *m_pRoadTurnLeft;
    FiniteTimeAction       *m_pParallaxTurnLeft;
    FiniteTimeAction       *m_pParallaxTurnRight;
    FiniteTimeAction       *m_pPlayerTurn;
    FiniteTimeAction       *m_pMenuAction;

    bool                    m_bLaneInUse[3];
    bool                    m_paused;
    int                     m_currentLane;
    int                     m_lastTurn;
    float                   m_carSpeed;
    float                   m_updateDt;
    float                   m_updateIncr;
    int                     m_blurDt;
    bool                    m_isRunning;
    Rect                    m_playerRect;

    int                     m_score;
};

#endif // __GAME_SCENE_H__
