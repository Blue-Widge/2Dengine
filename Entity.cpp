#include "Entity.h"

#include <cmath>

Entity::Entity(SDL_Renderer* p_renderer, const char* p_path) : Entity(p_renderer, p_path, {0, 0, 0 , 0})
{
    
}

Entity::Entity(SDL_Renderer* p_renderer, const char* p_path, const SDL_Rect& p_rect) : m_renderer(p_renderer), m_rect(p_rect)
{
    const auto surface = static_cast<SDL_Surface*>(SDL_LoadFile(p_path, nullptr));
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    m_collider = new BoxCollider(m_rect);
}

Entity::~Entity()
{
    SDL_DestroyTexture(m_texture);
    delete m_collider;
}

MoveableEntity::MoveableEntity(SDL_Renderer* p_renderer, const char* p_path, const SDL_Rect& p_rect, const float p_mass)
    : MoveableEntity(p_renderer, p_path, p_rect, p_mass, 1.f)
{
    
}

MoveableEntity::MoveableEntity(SDL_Renderer* p_renderer, const char* p_path, const SDL_Rect& p_rect, float p_mass,
    float p_viscosity) : Entity(p_renderer, p_path, p_rect), m_mass(p_mass), m_viscosity(p_viscosity)
{
    
}

void MoveableEntity::move(Axis_e p_axis, float p_moveSpeed, float p_deltaTime)
{
    const int deltaPos = static_cast<int>(p_deltaTime * p_moveSpeed);
    SDL_Rect colliderRect = m_collider->getColliderRect();
    
    if (p_axis == Axis_e::x)
    {
        m_rect.x += deltaPos;
        m_collider->changeBoxPosition(colliderRect.x + deltaPos, colliderRect.y);
        return;
    }
    m_rect.y += deltaPos;
    m_collider->changeBoxPosition(colliderRect.x, colliderRect.y + deltaPos);
}

void MoveableEntity::rotate(float p_rotationSpeed, float p_deltaTime)
{
    m_rotationAngle += p_rotationSpeed * p_deltaTime;

    m_rotationAngle = fmod(m_rotationAngle, 360.f);
}

void MoveableEntity::applyForces(Vec2<float> p_forces)
{
// TODO : for the moment it's only gravity
    if (m_isKinematic)
        return;
    SDL_Rect colliderRect = m_collider->getColliderRect();
    if (m_gravityReactive)
    {
        int gravityDeltaPos = static_cast<int>(m_mass * gravity * m_viscosity);
        m_rect.y -= gravityDeltaPos;
        m_collider->changeBoxPosition(colliderRect.x, colliderRect.y - gravityDeltaPos);
    }
}
