#pragma once
#include "utils.h"

class Entity;

class Collider
{
public:
    Collider(Entity* p_parent, const FRect& p_rect) : m_parent(p_parent), m_rect(p_rect) { }
    virtual ~Collider() = default;
    virtual void setPosition(const float p_x, const float p_y) { }
    virtual void updatePosition() { }
    virtual void updateDeltaPos() { }
    virtual void setDimensions(const float p_width, const float p_height) { }
    virtual void setRotation(float p_rotationAngle) { }
    virtual bool checkXCollisions(const Entity* p_otherEntity, float p_deltaTime) { return {};}
    virtual bool checkUpperCollisions(const Entity* p_otherEntity, float p_deltaTime) { return {};}
    virtual bool checkGroundCollision(const Entity& p_otherEntity, float p_deltaTime) { return {};}
    FRect& getColliderRect() { return m_rect; }
protected:
    Entity* m_parent;
    FRect m_rect = {0,0,0,0};
    Vec2<float> m_parentDeltaPos = {0, 0};
    float m_rotation = 0.f;
    
};

class BoxCollider : public Collider
{
public:
    BoxCollider(Entity* p_parent, const FRect& p_rect);
    void setPosition(float p_x, float p_y) override;
    void updatePosition() override;
    void updateDeltaPos() override;
    void setDimensions(float p_width, float p_height) override;
    void setRotation(float p_rotationAngle) override;
    bool checkXCollisions(const Entity* p_otherEntity, float p_deltaTime) override;
    bool checkGroundCollision(const Entity& p_otherEntity, float p_deltaTime) override;
    bool checkUpperCollisions(const Entity* p_otherEntity, float p_deltaTime) override;
};
