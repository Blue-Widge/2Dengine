#pragma once
#include <SDL_rect.h>

#include "utils.h"

class Entity;
class MoveableEntity;

class Collider
{
public:
    Collider() = default;
    virtual ~Collider() = default;
    virtual void changePosition(int p_x, int p_y) { }
    virtual void changeDimensions(int p_width, int p_height) { }
    virtual void changeRotation(float p_rotationAngle) { }
    virtual Vec2<float> checkCollision(const MoveableEntity& p_moveableEntity, const Entity& p_otherEntity) { return {};}
    SDL_Rect getColliderRect() const { return m_rect; }
protected:
    SDL_Rect m_rect = {0,0,0,0};
    float m_rotation = 0.f;
    
};

class BoxCollider : Collider
{
public:
    explicit BoxCollider(const SDL_Rect& p_rect);
    void changePosition(int p_x, int p_y) override;
    void changeDimensions(int p_width, int p_height) override;
    void changeRotation(float p_rotationAngle) override;
    Vec2<float> checkCollision(const MoveableEntity& p_moveableEntity, const Entity& p_otherEntity) override;
};
