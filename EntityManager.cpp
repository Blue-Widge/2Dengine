#include "EntityManager.h"

EntityManager::~EntityManager()
{
    for (auto entity : m_entities)
    {
        delete entity;
        entity = nullptr;
    }
}

Entity* EntityManager::addEntity()
{
    return addEntity(BASE_TEXTURE);
}

Entity* EntityManager::addEntity(const char* p_texturePath)
{
    return addEntity(p_texturePath, {0, 0, 50, 50});
}

Entity* EntityManager::addEntity(const char* p_texturePath, const FRect& p_rect)
{
    auto* entity = new Entity(this, m_nbEntities, m_renderer, p_texturePath, p_rect);
    ++m_nbEntities;
    m_entities.push_back(entity);

    return entity;
}

MoveableEntity* EntityManager::addMoveableEntity(const char* p_texturePath, const FRect& p_rect, const float p_mass)
{
    auto* entity = new MoveableEntity(this, m_nbEntities, m_renderer, p_texturePath, p_rect, p_mass);
    ++m_nbEntities;
    m_entities.push_back(entity);
    m_moveableEntities.push_back(entity);

    return entity;
}

Player* EntityManager::addPlayer(const char* p_texturePath, const FRect& p_rect, const float p_mass)
{
    auto* entity = Player::getPlayerInstance(this, m_nbEntities, m_renderer, p_texturePath, p_rect,
        p_mass, 0.81f);
    ++m_nbEntities;
    m_entities.push_back(entity);
    m_player = entity;
    return entity;
}


Vec2<float> EntityManager::getEntityAppliedVelocity(const MoveableEntity& p_moveableEntity,
                                                    const float p_deltaTime) const
{
    Vec2<float> appliedVelocity = {0.f, 0.f};
    if (p_moveableEntity.getIsKinematic())
        return appliedVelocity;
    for (const auto entity : m_entities)
    {
        if (*entity == p_moveableEntity)
            continue;
        const Vec2<float> currVelocity = p_moveableEntity.getCollider()->checkCollisions(*entity, p_deltaTime);
        appliedVelocity.x += currVelocity.x;
        appliedVelocity.y += currVelocity.y;
    }
    return appliedVelocity;
}

void EntityManager::resetEntities() const
{
    for (const auto entity : m_moveableEntities)
    {
        entity->resetEntity();
    }
}

void EntityManager::deleteEntities() const
{
    for (auto entity : m_entities)
    {
        delete entity;
        entity = nullptr;
    }
}