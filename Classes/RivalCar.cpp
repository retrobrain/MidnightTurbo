#include "RivalCar.h"


RivalCar::RivalCar(const char *carName, Layer *parentLayer)
    : m_pParentLayer(parentLayer),
      m_targetCorrection(0.0),
      m_currentCorrection(0.0),
      m_offsetFactor(0.0),
      m_currentScale(0.0),
      m_carSpeed(0.0),
      m_currentLane(0),
      m_isLaunched(false)
{
    initWithSpriteFrameName(carName);
    getTexture()->setAliasTexParameters();
    m_startPosition = m_currentPosition = carStartPosition;
    setPosition(m_startPosition);
    setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    setScale(m_currentScale);
    autorelease();

    parentLayer->addChild(this);
}

RivalCar *RivalCar::CreateCar(Layer *parentLayer, const char *carName)
{
    RivalCar *carToCreate = new RivalCar(carName, parentLayer);
    return carToCreate;
}

void RivalCar::CreateLights(const char *carName)
{
    m_lightTexture = Sprite::createWithSpriteFrameName(carName);
    m_lightTexture->getTexture()->generateMipmap();
    Texture2D::TexParams texParams = { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
    m_lightTexture->getTexture()->setTexParameters(texParams);
    m_lightTexture->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    m_lightTexture->setPosition(m_currentPosition);
    m_lightTexture->setScale(m_currentScale);
    m_pParentLayer->addChild(m_lightTexture, rivalLightsZ + rivalMaxZorder);
}

void RivalCar::Run(float offset, bool *lane, float carSpeed)
{
    m_pLaneInUse = lane;
    *m_pLaneInUse = true;

    m_currentCorrection = NULL;
    m_targetCorrection = NULL;

    m_currentPosition = m_startPosition = carStartPosition;
    setPosition(m_startPosition);
    auto startToTargetFactor = endOffset/startOffset;
    auto targetCorrection = -m_currentLane * endOffset;

    m_targetPosition.x = screenCenter.x + targetCorrection + (offset * startToTargetFactor);
    m_targetPosition.y = NULL;

    m_carSpeed = (carSpeed + (rand()%2 * 0.1)) * 2.0;

    m_isLaunched = true;
}

void RivalCar::MoveCar(float dt)
{
    //Y-coord acceleration delta. More close to target more acceleration applied
    auto yVelocity = m_startPosition.y + 0.1 - m_currentPosition.y;
    m_currentPosition.y -= m_carSpeed*yVelocity*dt;

    //set normal current X position
    m_currentPosition.x = (m_currentPosition.y * GetXOffset()) + m_targetPosition.x;

    m_currentScale = (m_startPosition.y-m_currentPosition.y) * scaleDt * screenScale;
    if(m_currentScale > screenScale)
        m_currentScale = screenScale;
    setScale(m_currentScale);

    //apply post-turn correction on X-coord
    m_currentPosition.x += m_currentCorrection * m_currentScale / screenScale;
    m_currentCorrection += m_targetCorrection * dt;
    if(fabs(m_currentCorrection) > fabs(m_targetCorrection/modelTurnSpeed))
    {
        m_currentCorrection = m_targetCorrection/modelTurnSpeed;
        m_targetPosition.x += m_currentCorrection;
        m_targetCorrection = 0.0f;
        m_currentCorrection = 0.0f;
    }

    setPosition(m_currentPosition);

    //as close to player higher Z-order. Use this to render the closest car the last
    setZOrder(m_startPosition.y - m_currentPosition.y);

    //release the lane so it can be used by other car
    if(m_currentPosition.y + 0.3f < m_startPosition.y && m_pLaneInUse && m_isLaunched)
    {
        *m_pLaneInUse = false;
        m_pLaneInUse = nullptr;
    }
    //hide the car after get to target pos
    else if(m_currentPosition.y <= m_targetPosition.y)
    {
        m_currentScale = 0.0f;
        m_currentPosition.y = 0.0f - getContentSize().height;
        setScale(m_currentScale);
        setPosition(m_currentPosition);
        m_isLaunched = false;
    }

    //set backlights position and scale based on the cars' values
    m_lightTexture->setScale(m_currentScale);
    m_lightTexture->setPosition(m_currentPosition);
}

void RivalCar::Turn(int turnDirection)
{
    m_currentLane += turnDirection;

    m_targetCorrection = -turnDirection * endOffset * modelTurnSpeed;
}

float RivalCar::GetXOffset()
{
    auto xFactor = fabs(m_targetPosition.x - m_startPosition.x) /
            fabs(m_targetPosition.y - m_startPosition.y);
    if(m_targetPosition.x > screenCenter.x)
        xFactor *= -1;

    return xFactor;
}

Rect RivalCar::GetRect()
{
    auto rectWidth  = carWidth  * getScale();
    auto rectHeight = carHeight / 2 * getScale();

    auto minX = m_currentPosition.x - rectWidth / 2;
    auto minY = m_currentPosition.y;

    auto maxX = m_currentPosition.x + rectWidth;
    auto maxY = m_currentPosition.y + rectHeight;


    return Rect(minX, minY, rectWidth, rectHeight);
}
