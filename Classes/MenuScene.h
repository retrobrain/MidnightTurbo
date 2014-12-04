#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "cocos2d.h"
#include "Configs.h"
#include "AppDelegate.h"
#include "GameScene.h"
#include <string>

using namespace CocosDenshion;

class MenuScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // because of use of some shaders was better to override this, avoiding the double visit thingy
    virtual void visit(Renderer* renderer, const Mat4 &parentTransform, uint32_t parentFlags);
    
    // start the game used as callback
    void menuStartCallback(Ref *Sender);
    
    // show the scores screen used as callback
    void menuScoresCallback(Ref *Sender);
    
    // back to main menu from the scores screen. It is used as a callback
    void menuBackToMenu(Ref *Sender);
    
    // sound on/off toggle. It is used as a callback
    void soundButtonTaped(Ref *Sender);

    // implement the "static create()" method manually
    CREATE_FUNC(MenuScene);
private:

    Layer                   *m_pTitleMenu;
    Layer                   *m_pSettingsMenu;

    MenuItemLabel           *m_startButton;
    MenuItemToggle          *m_soundButton;

    RenderTexture           *m_pRenderTexture;
    FiniteTimeAction        *m_BgrScale;

    Label                   *m_pMidnightTitle;
    Label                   *m_pTurboTitle;
    Menu                    *m_pMenu;
    float                    m_totalFps;
    int                      m_fpsCounter;

    GLProgramState          *m_pCurrentGLState;
    CocosDenshion::SimpleAudioEngine *m_pSlideSfx;
};

#endif // __MENU_SCENE_H__
