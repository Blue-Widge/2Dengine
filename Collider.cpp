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

Vec2<float> BoxCollider::checkCollisions(const Entity& p_otherEntity, const float p_deltaTime)
{
    Vec2<float> velocity = {0, 0};
    const FRect otherColliderRect = p_otherEntity.getCollider()->getColliderRect();
    const MoveableEntity* parent = reinterpret_cast<MoveableEntity*>(m_parent);
    const Vec2<float> currVelocity = parent->getVelocity();
    const float parentViscosity = parent->getViscosity();

    const float nextXMove = currVelocity.x * p_deltaTime;

    //From the left to the right
    if (m_rect.x + m_rect.w + nextXMove >= otherColliderRect.x &&
        m_rect.x + m_rect.w < otherColliderRect.x &&
        m_rect.y >= otherColliderRect.y && m_rect.y <= otherColliderRect.y + otherColliderRect.h )
            velocity.x -= currVelocity.x * parentViscosity;

    //from the right to the left
    if (otherColliderRect.x + otherColliderRect.w <= m_rect.x + nextXMove &&
        otherColliderRect.x + otherColliderRect.w > m_rect.x &&
        m_rect.y >= otherColliderRect.y && m_rect.y <= otherColliderRect.y + otherColliderRect.h )
            velocity.y -=  currVelocity.y * parentViscosity;

    //from down to up
    const float nextYMove = currVelocity.y * p_deltaTime;
    if (m_rect.y + nextYMove <= otherColliderRect.y + otherColliderRect.h &&
        m_rect.y > otherColliderRect.y + otherColliderRect.h &&
        m_rect.x > otherColliderRect.x && m_rect.x <= otherColliderRect.x + otherColliderRect.w)
            velocity.y -= currVelocity.y * parentViscosity;

    return velocity;
}

bool BoxCollider::checkGroundCollision(const Entity& p_otherEntity, const float p_deltaTime)
{
    const FRect otherColliderRect = p_otherEntity.getCollider()->getColliderRect();
    const float nextYMove = reinterpret_cast<MoveableEntity*>(m_parent)->getVelocity().y * p_deltaTime;
    if (m_rect.y + m_rect.h + nextYMove >= otherColliderRect.y &&
            m_rect.y < otherColliderRect.y + otherColliderRect.h &&
            m_rect.x >= otherColliderRect.x && m_rect.x + m_rect.w <= otherColliderRect.x + otherColliderRect.w) 
        return true;
    return false;
}
