#include "EntityManager.h"

EntityManager::~EntityManager()
{
    for (auto entity : m_entities)
    {
        delete entity;
        entity = nullptr;
    }
    m_entities.clear();
    m_moveableEntities.clear();
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
    m_staticEntities.push_back(entity);

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
        p_mass, 0.3f);
    ++m_nbEntities;
    m_entities.push_back(entity);
    m_moveableEntities.push_back(entity);
    m_player = entity;
    return entity;
}

Collectible* EntityManager::addCollectible(const char* p_texturePath, const FRect& p_rect)
{
    auto* collectible = new Collectible(this, m_nbEntities, m_renderer, p_texturePath, p_rect);
    ++m_nbEntities;
    m_entities.push_back(collectible);
    m_collectibles.push_back(collectible);
    return collectible;
}

void EntityManager::resetEntities() const
{
    for (const auto entity : m_moveableEntities)
        entity->resetEntity();
    
    for (const auto collectible : m_collectibles)
        collectible->resetEntity();
}

void EntityManager::deleteEntities() const
{
    for (const Entity* entity : m_entities)
    {
        delete entity;
        entity = nullptr;
    }
}
void EntityManager::solveInsidersEntities(const float& p_deltaTime) const
{
    for (MoveableEntity* moveableEntity : m_moveableEntities)
    {
        if (moveableEntity->getIsKinematic())
            continue;

        Collider* moveableEntityCollider = moveableEntity->getCollider();
        const FRect& moveableEntityColliderRect = moveableEntityCollider->getColliderRect();
        const Vec2<float> moveableEntityPosition = moveableEntity->getPosition();
        std::mutex& moveableEntityMutex = moveableEntity->getMutex();

        for (const Entity* entity : m_entities)
        {
            if (moveableEntity == entity || entity == m_player || entity->getIsKinematic())
                continue;

            Collider* entityCollider = entity->getCollider();
            const FRect& entityColliderRect = entityCollider->getColliderRect();

            std::lock_guard<std::mutex> insidersLock(moveableEntityMutex);

            const float yOverlap = std::min(entityColliderRect.y + entityColliderRect.h - moveableEntityColliderRect.y,
                                             moveableEntityColliderRect.y + moveableEntityColliderRect.h - entityColliderRect.y);

            const float xOverlap = std::min(entityColliderRect.x + entityColliderRect.w - moveableEntityColliderRect.x,
                                             moveableEntityColliderRect.x + moveableEntityColliderRect.w - entityColliderRect.x);

            if (xOverlap + g_epsilonValue <= 0  || yOverlap + g_epsilonValue <= 0)
                continue;
            
            const Vec2<float> moveableEntityVelocity = moveableEntity->getVelocity();

            if (std::abs(xOverlap) < std::abs(yOverlap))
            {
                // Horizontal collision
                if (moveableEntityVelocity.x < -g_epsilonValue &&
                    xOverlap > -g_epsilonValue &&
                    moveableEntityCollider->checkLeftCollisions(entity, p_deltaTime))
                {
                    moveableEntity->setPositionKeepingInitialPos(entityColliderRect.x + entityColliderRect.w + g_epsilonValue,
                        moveableEntityPosition.y);
                }
                else if (moveableEntityVelocity.x > g_epsilonValue &&
                    xOverlap > -g_epsilonValue &&
                    moveableEntityCollider->checkRightCollisions(entity, p_deltaTime))
                {
                    moveableEntity->setPositionKeepingInitialPos(entityColliderRect.x - moveableEntityColliderRect.w - g_epsilonValue,
                        moveableEntityPosition.y);
                }
            }
            else
            {
                // Vertical collision
                if (moveableEntityVelocity.y < -g_epsilonValue &&
                    yOverlap > -g_epsilonValue &&
                    moveableEntityCollider->checkUpperCollisions(entity, p_deltaTime))
                {
                    moveableEntity->setPositionKeepingInitialPos(moveableEntityPosition.x,
                        entityColliderRect.y + entityColliderRect.h + g_epsilonValue);
                }
                else if (moveableEntityVelocity.y > g_epsilonValue &&
                    yOverlap > -g_epsilonValue &&
                    moveableEntityCollider->checkGroundCollision(entity, p_deltaTime))
                {
                    moveableEntity->setPositionKeepingInitialPos(moveableEntityPosition.x,
                        entityColliderRect.y - moveableEntityColliderRect.h - g_epsilonValue);
                }
            }
            moveableEntityCollider->updatePosition();
        }
    }
}
