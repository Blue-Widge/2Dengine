#include "Collider.h"

#include "Entity.h"

BoxCollider::BoxCollider(const SDL_Rect& p_rect)
{
    m_rect = p_rect;
}

void BoxCollider::changePosition(const int p_x, const int p_y)
{
    m_rect.x = p_x;
    m_rect.y = p_y;
}

void BoxCollider::changeDimensions(const int p_width, const int p_height)
{
    m_rect.w = p_width;
    m_rect.h = p_height;
}

void BoxCollider::changeRotation(float p_rotationAngle)
{
    m_rotation = p_rotationAngle;
}


Vec2<float> BoxCollider::checkCollision(const MoveableEntity& p_moveableEntity, const Entity& p_otherEntity)
{
    Vec2<float> velocity = {0, 0};
    const SDL_Rect moveableEntityColliderRect = p_moveableEntity.getCollider()->getColliderRect();
    const SDL_Rect entityColliderRect = p_otherEntity.getCollider()->getColliderRect();
    const Vec2<float> currVelocity = p_moveableEntity.getVelocity();
        
    if (moveableEntityColliderRect.x + moveableEntityColliderRect.w >= entityColliderRect.x &&
        moveableEntityColliderRect.x <= entityColliderRect.x + entityColliderRect.w &&  
        moveableEntityColliderRect.y + moveableEntityColliderRect.h >= entityColliderRect.y &&
        moveableEntityColliderRect.y <=  entityColliderRect.y + entityColliderRect.h)
    {
        velocity.x = -currVelocity.x;
        velocity.y = -currVelocity.y;
    }
    return velocity;
}