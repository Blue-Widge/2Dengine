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
    m_unmoveableEntities.push_back(entity);

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
    m_moveableEntities.push_back(entity);
    m_player = entity;
    return entity;
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

void EntityManager::solveInsidersEntities() const
{
    for(MoveableEntity* moveableEntity : m_moveableEntities)
    {
        if (moveableEntity->getIsKinematic())
            continue;
        
        Collider* moveableEntityCollider = moveableEntity->getCollider();
        const FRect& moveableEntityColliderRect = moveableEntityCollider->getColliderRect();
        const Vec2<float> moveableEntityPosition = moveableEntity->getPosition();
        std::mutex& moveableEntityMutex = moveableEntity->getMutex();
        for(const Entity* entity : m_entities)
        {
            if (moveableEntity == entity || entity == m_player)
                continue;
            Collider* entityCollider = entity->getCollider();
            const FRect& entityColliderRect = entityCollider->getColliderRect();

            std::lock_guard<std::mutex> insidersLock(moveableEntityMutex);
            constexpr float epsilonValue = 1.f;
            const float halfX = 3.f * moveableEntityColliderRect.w / 4.f;
            const float halfY = 3.f * moveableEntityColliderRect.h / 4.f;
            if (moveableEntityColliderRect.x + halfX + epsilonValue < entityColliderRect.x ||
                moveableEntityColliderRect.x + moveableEntityColliderRect.w - (halfX + epsilonValue) > entityColliderRect.x + entityColliderRect.w ||
                moveableEntityColliderRect.y + halfY + epsilonValue < entityColliderRect.y ||
                moveableEntityColliderRect.y + moveableEntityColliderRect.h - (halfY + epsilonValue) > entityColliderRect.y + entityColliderRect.h)
                    continue;
            const Vec2<float> moveableEntityVelocity = moveableEntity->getVelocity();
            if (moveableEntityVelocity.x <= -epsilonValue)
            {
                moveableEntity->setPosition(entityColliderRect.x + entityColliderRect.w + epsilonValue,
                    moveableEntityPosition.y);
                moveableEntityCollider->updatePosition();
            }
            else if (moveableEntityVelocity.x >= epsilonValue)
            {
                moveableEntity->setPosition(entityColliderRect.x - moveableEntityColliderRect.w - epsilonValue,
                    moveableEntityPosition.y);
                moveableEntityCollider->updatePosition();
            }
            if (moveableEntityVelocity.y <= -epsilonValue)
            {
                moveableEntity->setPosition(moveableEntityPosition.x, entityColliderRect.y + entityColliderRect.h
                    + epsilonValue);
                moveableEntityCollider->updatePosition();
            }
            else if (moveableEntityVelocity.y >= epsilonValue)
            {
                moveableEntity->setPosition(moveableEntityPosition.x, entityColliderRect.y - moveableEntityColliderRect.h - epsilonValue);
                moveableEntityCollider->updatePosition();
            }
        }
    }
}
