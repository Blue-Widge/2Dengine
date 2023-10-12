#pragma once
#include "Collider.h"
#include "SDLHandler.h"

enum Axis_e
{
    x,
    y
};

#define BASE_TEXTURE "./images/baseTexture.bmp"

class Entity
{
public:
    Entity(EntityManager* p_entityManager, Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path, const FRect& p_rect);
    ~Entity();
    
    void setPosition(float p_x, float p_y);
    Vec2<float> getPosition() const;
    
    inline void setRotation(const float p_rotationAngle) { m_rotationAngle = p_rotationAngle; }
    void setSize(float p_w, float p_h);
    inline FRect getEntityRect() const { return m_rect; }
    bool operator==(const Entity& p_entity) const;
    inline Collider* getCollider() const { return m_collider; }
    inline SDL_Texture* getTexture() const { return m_texture; }

protected:
    Uint16 m_id = 0;
    
    SDL_Renderer* m_renderer = nullptr;
    FRect m_rect;
    SDL_Texture* m_texture = nullptr;
    EntityManager* m_entityManager = nullptr;
    
    float m_rotationAngle = 0.f;
    Collider* m_collider;
    Uint16 m_depth = 0;
};

class MoveableEntity : public Entity
{
public:
    MoveableEntity(EntityManager* p_entityManager, Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path,
                   const FRect& p_rect, float p_mass);
    MoveableEntity(EntityManager* p_entityManager, Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path,
                   const FRect& p_rect, float p_mass, float p_viscosity);
    
    void setPosition(float p_x, float p_y);
    void setRotation(float p_rotationAngle);
    void setSize(float p_w, float p_h);
    void move(Axis_e p_axis, float p_moveSpeed, float p_deltaTime);
    void rotate(float p_rotationSpeed, float p_deltaTime);
    void applyForces(std::chrono::milliseconds p_fixedUpdateTime);
    inline Vec2<float> getVelocity() const { return m_velocity; }
    void resetEntity();
    void applyGravity(float p_deltaTime);
    inline bool getIsKinematic() const { return m_isKinematic; }
    inline void setKinematic(bool p_kinematic) { m_isKinematic = p_kinematic; }
    inline float getViscosity() const { return m_viscosity; }
protected:
    bool m_gravityReactive = true;
    Vec2<float> m_initialPos;
    constexpr static float gravity = 9.81f;
    float m_mass = 0.f;
    float m_viscosity = 1.f;
    Vec2<float> m_velocity;
    bool m_isKinematic = false;
};