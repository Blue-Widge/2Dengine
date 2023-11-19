#include "Collider.h"
#include "Entity.h"

BoxCollider::BoxCollider(Entity* p_parent, const FRect& p_rect) : Collider(p_parent, p_rect)
{
    BoxCollider::updateDeltaPos();
}

void BoxCollider::setPosition(const float p_x, const float p_y)
{
    m_rect.x = p_x;
    m_rect.y = p_y;
    updateDeltaPos();
}

void BoxCollider::updatePosition()
{
    const Vec2<float> parentPos = m_parent->getPosition();
    m_rect.x = parentPos.x + m_parentDeltaPos.x;
    m_rect.y = parentPos.y + m_parentDeltaPos.y;
}

void BoxCollider::updateDeltaPos()
{
    const Vec2<float> posParent = m_parent->getPosition();
    m_parentDeltaPos.x = m_rect.x - posParent.x;
    m_parentDeltaPos.y = m_rect.y - posParent.y;
}

void BoxCollider::setDimensions(const float p_width, const float p_height)
{
    m_rect.w = p_width;
    m_rect.h = p_height;
}

void BoxCollider::setRotation(const float p_rotationAngle)
{
    m_rotation = p_rotationAngle;
}

bool BoxCollider::checkGroundCollision(const Entity* p_otherEntity, const float p_deltaTime)
{
    const FRect& otherColliderRect = p_otherEntity->getCollider()->getColliderRect();
    const float nextYMove = reinterpret_cast<MoveableEntity*>(m_parent)->getVelocity().y * p_deltaTime;
    if (m_rect.y + m_rect.h + nextYMove + g_epsilonValue >= otherColliderRect.y &&
        m_rect.y + m_rect.h - g_epsilonValue <= otherColliderRect.y + otherColliderRect.h &&
        m_rect.x + m_rect.w - g_epsilonValue > otherColliderRect.x + g_epsilonValue &&
        m_rect.x + g_epsilonValue < otherColliderRect.x + otherColliderRect.w - g_epsilonValue)
        return true;
    return false;
}

bool BoxCollider::checkUpperCollisions(const Entity* p_otherEntity, const float p_deltaTime)
{
    const FRect& otherColliderRect = p_otherEntity->getCollider()->getColliderRect();
    const float nextYMove = reinterpret_cast<MoveableEntity*>(m_parent)->getVelocity().y * p_deltaTime;
    
    //from down to up
    if (m_rect.y + nextYMove - g_epsilonValue <= otherColliderRect.y + otherColliderRect.h &&
        m_rect.y + g_epsilonValue >= otherColliderRect.y &&
        m_rect.x + m_rect.w - g_epsilonValue > otherColliderRect.x + g_epsilonValue &&
        m_rect.x + g_epsilonValue < otherColliderRect.x + otherColliderRect.w - g_epsilonValue)
            return true;
    return false;
}

bool BoxCollider::checkLeftCollisions(const Entity* p_otherEntity, const float p_deltaTime)
{
    const FRect& otherColliderRect = p_otherEntity->getCollider()->getColliderRect();
    const float nextXMove = reinterpret_cast<MoveableEntity*>(m_parent)->getVelocity().x * p_deltaTime;
    
    //from the right to the left
    if (m_rect.x + nextXMove - g_epsilonValue <= otherColliderRect.x + otherColliderRect.w  &&
        m_rect.x + g_epsilonValue >= otherColliderRect.x &&
        m_rect.y + m_rect.h - g_epsilonValue > otherColliderRect.y + g_epsilonValue &&
        m_rect.y + g_epsilonValue < otherColliderRect.y + otherColliderRect.h - g_epsilonValue)
            return true;
    return false;
}

bool BoxCollider::checkRightCollisions(const Entity* p_otherEntity, const float p_deltaTime)
{
    const FRect& otherColliderRect = p_otherEntity->getCollider()->getColliderRect();
    const float nextXMove = reinterpret_cast<MoveableEntity*>(m_parent)->getVelocity().x * p_deltaTime;
    
    //From the left to the right
    if (m_rect.x + m_rect.w + nextXMove + g_epsilonValue >= otherColliderRect.x &&
        m_rect.x + m_rect.w - g_epsilonValue <= otherColliderRect.x + otherColliderRect.w &&
        m_rect.y + m_rect.h - g_epsilonValue >= otherColliderRect.y + g_epsilonValue &&
        m_rect.y + g_epsilonValue <= otherColliderRect.y + otherColliderRect.h - g_epsilonValue)
            return true;
    return false;
}
