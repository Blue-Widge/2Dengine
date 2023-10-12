#include "Entity.h"
#include <cmath>

Entity::Entity(EntityManager* p_entityManager, Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path,
               const FRect& p_rect) : m_id(p_id), m_renderer(p_renderer), m_rect(p_rect),
                                         m_entityManager(p_entityManager)
{
    const auto surface = SDL_LoadBMP(p_path);
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    m_collider = reinterpret_cast<Collider*>(new BoxCollider(this, m_rect));
}

Entity::~Entity()
{
    SDL_DestroyTexture(m_texture);
    delete m_collider;
}

void Entity::setPosition(const float p_x, const float p_y)
{
    m_rect.x = p_x > SCREEN_WIDTH ? SCREEN_WIDTH : p_x;
    m_rect.y = p_y > SCREEN_HEIGHT ? SCREEN_HEIGHT : p_y;
}

Vec2<float> Entity::getPosition() const
{
    return {m_rect.x, m_rect.y};
}

void Entity::setSize(const float p_w, const float p_h)
{
    m_rect.w = p_w;
    m_rect.h = p_h;
}

bool Entity::operator==(const Entity& p_entity) const
{
    return this->m_id == p_entity.m_id;
}

MoveableEntity::MoveableEntity(EntityManager* p_entityManager, const Uint16 p_id, SDL_Renderer* p_renderer,
                               const char* p_path, const FRect& p_rect,
                               const float p_mass) : MoveableEntity(p_entityManager, p_id, p_renderer, p_path, p_rect,
    p_mass, 0.31f)
{
}

MoveableEntity::MoveableEntity(EntityManager* p_entityManager, const Uint16 p_id, SDL_Renderer* p_renderer,
                               const char* p_path, const FRect& p_rect, float p_mass,
                               float p_viscosity) : Entity(p_entityManager, p_id, p_renderer, p_path, p_rect),
                                                    m_initialPos({0, 0}),
                                                    m_mass(p_mass), m_viscosity(p_viscosity),
                                                    m_velocity({0.f, 0.f})
{
    m_initialPos.x = p_rect.x;
    m_initialPos.y = p_rect.y;
}

void MoveableEntity::setPosition(const float p_x, const float p_y)
{
    m_initialPos.x = p_x;
    m_initialPos.y = p_y;
    Entity::setPosition(p_x, p_y);
    m_collider->updatePosition();
}

void MoveableEntity::setRotation(const float p_rotationAngle)
{
    m_collider->setRotation(p_rotationAngle);
    Entity::setRotation(p_rotationAngle);
}

void MoveableEntity::setSize(float p_w, float p_h)
{
    const FRect colliderRect = m_collider->getColliderRect();
    m_collider->setDimensions(colliderRect.w - m_rect.w + p_w, colliderRect.h - m_rect.h + p_h);
    Entity::setSize(p_w, p_h);
}

void MoveableEntity::move(Axis_e p_axis, float p_moveSpeed, float p_deltaTime)
{
    float deltaPos = p_deltaTime * p_moveSpeed;
    const FRect colliderRect = m_collider->getColliderRect();

    if (p_axis == x)
    {
        if (colliderRect.x + colliderRect.w > SCREEN_WIDTH)
            deltaPos = SCREEN_WIDTH - colliderRect.w;
        m_rect.x += deltaPos;
        return;
    }
    if (colliderRect.y + colliderRect.h > SCREEN_HEIGHT)
        deltaPos = SCREEN_HEIGHT - colliderRect.h;
    m_rect.y += deltaPos;
    
    m_collider->updatePosition();
}

void MoveableEntity::rotate(float p_rotationSpeed, float p_deltaTime)
{
    m_rotationAngle += p_rotationSpeed * p_deltaTime;

    m_rotationAngle = fmod(m_rotationAngle, 360.f);
}

void MoveableEntity::applyForces(const std::chrono::milliseconds p_fixedUpdateTime)
{
    //no fixedUpdateTime means that the thread hasn't started
    if (p_fixedUpdateTime.count() == 0 || m_isKinematic)
        return;
    
    const float updateTime = static_cast<float>(p_fixedUpdateTime.count()) / 1000.0f;
    m_velocity += m_entityManager->getEntityAppliedVelocity(*this, updateTime);
    float axisDeltaVelocity = m_velocity.x * m_mass * m_viscosity;
    move(Axis_e::x, axisDeltaVelocity, updateTime);

    axisDeltaVelocity = m_velocity.y * m_mass * m_viscosity;
    move(Axis_e::y, axisDeltaVelocity, updateTime);
    m_collider->updatePosition();
}

void MoveableEntity::resetEntity()
{
    setPosition(m_initialPos.x, m_initialPos.y);
    setRotation(0.f);
}

void MoveableEntity::applyGravity(float p_deltaTime)
{
    if (m_gravityReactive)
    {
        const float gravityDeltaVelocity = gravity * m_viscosity;
        const auto entities = m_entityManager->getEntities();
        const float gravityMovementThreshold = m_viscosity * gravity;
        for (const auto entity : entities)
        {
            if (entity == this)
                continue;
            if (m_collider->checkGroundCollision(*entity, p_deltaTime))
            {
                if (m_velocity.y > gravityMovementThreshold || m_velocity.y < -gravityMovementThreshold)
                    m_velocity.y *= -m_viscosity;
                else
                {
                    m_velocity.y = 0.f;
                    return;
                }
                break;
            }
        }
        m_velocity.y += gravityDeltaVelocity;
        move(Axis_e::y, m_velocity.y, p_deltaTime);
        m_collider->updatePosition();
    }
}
