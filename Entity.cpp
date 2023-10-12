#include "Entity.h"
#include <cmath>

Entity::Entity(EntityManager* p_entityManager, Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path,
               const SDL_Rect& p_rect) : m_id(p_id), m_renderer(p_renderer), m_rect(p_rect),
                                         m_entityManager(p_entityManager)
{
    const auto surface = SDL_LoadBMP(p_path);
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    m_collider = reinterpret_cast<Collider*>(new BoxCollider(m_rect));
}

Entity::~Entity()
{
    SDL_DestroyTexture(m_texture);
    delete m_collider;
}

void Entity::setSize(const int p_w, const int p_h)
{
    m_rect.w = p_w;
    m_rect.h = p_h;
}

bool Entity::operator==(const Entity& p_entity) const
{
    return this->m_id == p_entity.m_id;
}

MoveableEntity::MoveableEntity(EntityManager* p_entityManager, const Uint16 p_id, SDL_Renderer* p_renderer,
                               const char* p_path, const SDL_Rect& p_rect,
                               const float p_mass) : MoveableEntity(p_entityManager, p_id, p_renderer, p_path, p_rect,
    p_mass, 1.f)
{
}

MoveableEntity::MoveableEntity(EntityManager* p_entityManager, const Uint16 p_id, SDL_Renderer* p_renderer,
                               const char* p_path, const SDL_Rect& p_rect, float p_mass,
                               float p_viscosity) : Entity(p_entityManager, p_id, p_renderer, p_path, p_rect),
                                                    m_initialPos({0, 0}),
                                                    m_mass(p_mass), m_viscosity(p_viscosity),
                                                    m_velocity({0.f, 0.f})
{
    m_initialPos.x = p_rect.x;
    m_initialPos.y = p_rect.y;
}

void MoveableEntity::setPosition(const int p_x, const int p_y)
{
    const SDL_Rect colliderRect = m_collider->getColliderRect();
    m_collider->changePosition(colliderRect.x - m_rect.x + p_x, colliderRect.y - m_rect.y + p_y);
    m_initialPos.x = p_x;
    m_initialPos.y = p_y;
    Entity::setPosition(p_x, p_y);
}

void MoveableEntity::setRotation(const float p_rotationAngle)
{
    m_collider->changeRotation(p_rotationAngle);
    Entity::setRotation(p_rotationAngle);
}

void MoveableEntity::setSize(const int p_w, const int p_h)
{
    const SDL_Rect colliderRect = m_collider->getColliderRect();
    m_collider->changeDimensions(colliderRect.w - m_rect.w + p_w, colliderRect.h - m_rect.h + p_h);
    Entity::setSize(p_w, p_h);
}

void MoveableEntity::move(Axis_e p_axis, float p_moveSpeed, float p_deltaTime)
{
    float deltaPos = p_deltaTime * p_moveSpeed;
    const SDL_Rect colliderRect = m_collider->getColliderRect();

    if (p_axis == x)
    {
        if (m_rect.x + m_rect.w > SCREEN_WIDTH)
            deltaPos = static_cast<float>(SCREEN_WIDTH - m_rect.w);
        m_rect.x += static_cast<int>(deltaPos);
        m_collider->changePosition(colliderRect.x + deltaPos, colliderRect.y);
        return;
    }
    if (m_rect.y + m_rect.h > SCREEN_HEIGHT)
        deltaPos = static_cast<float>(SCREEN_HEIGHT - m_rect.h);
    m_rect.y += static_cast<int>(deltaPos);
    m_collider->changePosition(colliderRect.x, colliderRect.y + deltaPos);

    //TODO : don't stay inside the screen and collider is off screen (a lot)
}

void MoveableEntity::rotate(float p_rotationSpeed, float p_deltaTime)
{
    m_rotationAngle += p_rotationSpeed * p_deltaTime;

    m_rotationAngle = fmod(m_rotationAngle, 360.f);
}

void MoveableEntity::applyForces(const std::chrono::milliseconds p_fixedUpdateTime)
{
    //no fixedUpdateTime means that the game hasn't started
    if (p_fixedUpdateTime.count() == 0 || m_isKinematic)
        return;
    
    SDL_Rect colliderRect = m_collider->getColliderRect();
    const auto updateTime = static_cast<float>(p_fixedUpdateTime.count());
    
    if (m_gravityReactive)
    {
        const int gravityDeltaVelocity = gravity * m_viscosity;
        move(Axis_e::y, gravity * m_viscosity, updateTime);
        m_collider->changePosition(colliderRect.x, colliderRect.y + static_cast<int>(updateTime * gravityDeltaVelocity));
        colliderRect = m_collider->getColliderRect();
    }
    
    const Vec2<float> velocity = m_entityManager->getEntityAppliedVelocity(*this);
    float axisDeltaVelocity = velocity.x * m_mass * m_viscosity;
    move(Axis_e::x, axisDeltaVelocity, updateTime);
    m_collider->changePosition(colliderRect.x + static_cast<int>(axisDeltaVelocity * updateTime), colliderRect.y);

    axisDeltaVelocity = velocity.y * m_mass * m_viscosity;
    move(Axis_e::y, axisDeltaVelocity, updateTime);
    m_collider->changePosition(colliderRect.x, colliderRect.y + static_cast<int>(axisDeltaVelocity * updateTime));
}

void MoveableEntity::resetEntity()
{
    setPosition(m_initialPos.x, m_initialPos.y);
    setRotation(0.f);
}
