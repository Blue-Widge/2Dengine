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
    Entity(EntityManager* p_entityManager, Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path, const SDL_Rect& p_rect);
    ~Entity();
    
    void setPosition(const int p_x, const int p_y)
    {
        m_rect.x = p_x > SCREEN_WIDTH ? SCREEN_WIDTH : p_x;
        m_rect.y = p_y > SCREEN_HEIGHT ? SCREEN_HEIGHT : p_y;
    }
    
    void setRotation(const float p_rotationAngle) { m_rotationAngle = p_rotationAngle; }
    void setSize(int p_w, int p_h);
    SDL_Rect getEntityRect() const { return m_rect; }
    bool operator==(const Entity& p_entity) const;
    inline Collider* getCollider() const { return m_collider; }
    inline SDL_Texture* getTexture() const { return m_texture; }

protected:
    Uint16 m_id = 0;
    
    SDL_Renderer* m_renderer = nullptr;
    SDL_Rect m_rect;
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
                   const SDL_Rect& p_rect, float p_mass);
    MoveableEntity(EntityManager* p_entityManager, Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path,
                   const SDL_Rect& p_rect, float p_mass, float p_viscosity);
    
    void setPosition(int p_x, int p_y);
    void setRotation(float p_rotationAngle);
    void setSize(int p_w, int p_h);
    void move(Axis_e p_axis, float p_moveSpeed, float p_deltaTime);
    void rotate(float p_rotationSpeed, float p_deltaTime);
    void applyForces(std::chrono::milliseconds p_fixedUpdateTime);
    inline Vec2<float> getVelocity() const { return m_velocity; }
    void resetEntity();

    bool m_gravityReactive = true;
    Vec2<int> m_initialPos;
    constexpr static float gravity = 9.81f;
    bool m_isKinematic = false;
    
private:
    float m_mass = 0.f;
    float m_viscosity = 1.f;
    Vec2<float> m_velocity;
};