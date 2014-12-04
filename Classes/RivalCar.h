#ifndef __RIVAL_CAR_H__
#define __RIVAL_CAR_H__

#include "cocos2d.h"
#include "Configs.h"

using namespace CocosDenshion;

class RivalCar : public Sprite
{
public:
    //no need to release, it's autoreleasing
    static RivalCar* CreateCar(Layer *parentLayer, const char *carName);

    // create backlights for each single car. This will have the same position, scale & visibility as the car itself
    // backlights object is attached to the parent layer
    void CreateLights(const char *carName);

    //launch the car
    void Run(float offset, bool *lane, float carSpeed);

    //Is used to move the car, update on every tick. The movement is FPS independant
    void MoveCar(float dt);

    //Turn action. There is some target position correction is happenning there
    void Turn(int turnDirection);

    //check if the car is running
    bool IsLaunched(){return m_isLaunched;}

    //Calculates objects x-coordinates based on it's y position
    float GetXOffset();

    //Returns the updated rect. Is used for intersect
    Rect GetRect();

private:
    RivalCar(const char *carName, Layer *parentLayer);

    Layer  *m_pParentLayer;
    Sprite *m_lightTexture;

    Vec2    m_startPosition;
    Vec2    m_currentPosition;
    Vec2    m_targetPosition;
    float   m_targetCorrection;
    float   m_currentCorrection;
    float   m_offsetFactor;
    float   m_currentScale;
    float   m_carSpeed;
    bool   *m_pLaneInUse;
    int     m_currentLane;
    bool    m_isLaunched;
};

#endif // __RIVAL_CAR_H__
