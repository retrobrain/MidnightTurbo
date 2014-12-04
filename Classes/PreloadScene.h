#ifndef __PRELOAD_SCENE_H__
#define __PRELOAD_SCENE_H__

#include "cocos2d.h"
#include "Configs.h"

struct Text
{
    Text(char*_path, int _frames, bool POT = false) : path(_path), frames(_frames), pot(POT){}
    char *path;
    int frames;
    bool pot;
};

class PreloadScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
//    virtual void onEnter();

    // implement the "static create()" method manually
    CREATE_FUNC(PreloadScene);
    
    //is planned to be assync
    void PreloadData();

private:
    virtual void update(float dt);
    bool m_dataStartLoading = false;
    bool m_dataFinishLoading = false;
    Label *_labelLoading;
};

#endif // __PRELOAD_SCENE_H__
