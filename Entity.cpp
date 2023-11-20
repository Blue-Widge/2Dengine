#include "Entity.h"
#include <SDL_image.h>
#include "EntityManager.h"


//to handle fullscreen when playing
int g_scenePosX = HIERARCHY_WIDTH;
int g_scenePosY = 0;
int g_sceneWidth = SCENE_WIDTH;
int g_sceneHeight = SCENE_HEIGHT;

Entity::Entity(EntityManager* p_entityManager, const Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path,
               const FRect& p_rect) : m_id(p_id), m_name("Entity " + to_string(m_id)), m_renderer(p_renderer),
                                      m_rect(p_rect), m_entityManager(p_entityManager)
{
    setTexture(p_path);
    m_collider = new BoxCollider(this, m_rect);
}

Entity::~Entity()
{
    SDL_DestroyTexture(m_texture);
    m_texture = nullptr;
    delete m_collider;
    m_collider = nullptr;
}

void Entity::setPosition(const float p_x, const float p_y)
{
    m_rect.x = (p_x + m_rect.w) > g_sceneWidth ? (g_sceneWidth - m_rect.w) : p_x;
    m_rect.y = (p_y + m_rect.h) > g_sceneHeight ? (g_sceneHeight - m_rect.h) : p_y;
    m_collider->updatePosition();
}

void Entity::setRotation(const float p_rotationAngle)
{
    m_rotationAngle = p_rotationAngle;
    m_collider->setRotation(p_rotationAngle);
}

void Entity::setSize(const float p_w, const float p_h)
{
    const FRect colliderRect = m_collider->getColliderRect();
    m_collider->setDimensions(colliderRect.w - m_rect.w + p_w, colliderRect.h - m_rect.h + p_h);
    m_rect.w = p_w;
    m_rect.h = p_h;
}

bool Entity::operator==(const Entity& p_entity) const
{
    return this->m_id == p_entity.m_id;
}

void Entity::setTexture(const char* p_path)
{
    const auto surface = IMG_Load(p_path);
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
}

std::string Entity::prepareEntityInfos() const
{
    const SDL_Rect entityRect = convertFRect(getEntityRect());
    const SDL_Rect colliderRect = convertFRect(getCollider()->getColliderRect());

    //no need to recreate these variables at runtime but I don't see them as object's attribute
    std::string entityInfosString = "Entity's name : " + m_name + "\n"
        "Entity's ID : " + to_string(m_id) + "\n"
        "Entity's X position : " + to_string(entityRect.x) + "\n"
        "Entity's Y position : " + to_string(entityRect.y) + "\n"
        "Entity's rotation : " + to_string(m_rotationAngle) + "\n"
        "Entity's X size : " + to_string(entityRect.w) + "\n"
        "Entity's Y size : " + to_string(entityRect.h) + "\n"
        "Collider's X position : " + to_string(colliderRect.x) + "\n"
        "Collider's Y position : " + to_string(colliderRect.y) + "\n"
        "Collider's X size : " + to_string(colliderRect.w) + "\n"
        "Collider's Y size : " + to_string(colliderRect.h) + "\n"
        "Is kinematic : " + to_string(m_isKinematic) + "\n"
        "Entity's texture : \n";
    return entityInfosString;
}
void Entity::updateBeforeDelete() const
{
    const std::vector<Entity*> entities = m_entityManager->getEntities();
    const std::vector<Entity*> staticEntities = m_entityManager->getStaticEntities();
    std::vector<Entity*> remainingEntities = {};
    std::vector<Entity*> remainingStaticEntities = {};
    for(Entity* currEntity : entities)
    {
        if (currEntity == this)
            continue;
        remainingEntities.emplace_back(currEntity);
    }
    for(Entity* currStaticEntity : staticEntities)
    {
        if (currStaticEntity == this)
            continue;
        remainingStaticEntities.emplace_back(currStaticEntity);
    }
    m_entityManager->setEntities(std::move(remainingEntities));
    m_entityManager->setStaticEntities(std::move(remainingStaticEntities));
}

MoveableEntity::MoveableEntity(EntityManager* p_entityManager, const Uint16 p_id, SDL_Renderer* p_renderer,
                               const char* p_path, const FRect& p_rect,
                               const float p_mass) : MoveableEntity(p_entityManager, p_id, p_renderer, p_path, p_rect,
    p_mass, 0.31f)
{
}

MoveableEntity::MoveableEntity(EntityManager* p_entityManager, const Uint16 p_id, SDL_Renderer* p_renderer,
                               const char* p_path, const FRect& p_rect, const float p_mass,
                               const float p_viscosity) : Entity(p_entityManager, p_id, p_renderer, p_path, p_rect),
                                                          m_initialPos({0, 0}), m_mass(p_mass),
                                                          m_viscosity(p_viscosity)
{
    m_name = "MoveableEntity " + to_string(m_id);
    m_velocity = {0.f, 0.f};
    m_initialPos.x = m_rect.x = p_rect.x;
    m_initialPos.y = m_rect.y = p_rect.y;
}

MoveableEntity::~MoveableEntity()
{
    Entity::~Entity();
}

void MoveableEntity::setPosition(const float p_x, const float p_y)
{
    m_initialPos.x = p_x;
    m_initialPos.y = p_y;
    Entity::setPosition(p_x, p_y);
}

void MoveableEntity::setPositionKeepingInitialPos(const float p_x, const float p_y)
{
    Entity::setPosition(p_x, p_y);
}

void MoveableEntity::move(const Axis_e p_axis, const float p_moveSpeed, const float p_deltaTime)
{
    float deltaPos = p_deltaTime * p_moveSpeed;
    const FRect colliderRect = m_collider->getColliderRect();

    if (p_axis == x)
    {
        if (colliderRect.x + colliderRect.w > g_sceneWidth)
            deltaPos = g_sceneWidth - colliderRect.w;
        m_rect.x += deltaPos;
        return;
    }
    if (colliderRect.y + colliderRect.h > g_sceneHeight)
        deltaPos = g_sceneHeight - colliderRect.h;
    m_rect.y += deltaPos;

    m_collider->updatePosition();
}

void MoveableEntity::move(const float p_deltaTime)
{
    move(Axis_e::x, m_velocity.x, p_deltaTime);
    move(Axis_e::y, m_velocity.y, p_deltaTime);
}

void MoveableEntity::rotate(const float p_rotationSpeed, const float p_deltaTime)
{
    const std::lock_guard<std::mutex> rotateGuard(this->m_entityMutex);
    m_rotationAngle += p_rotationSpeed * p_deltaTime;
    m_rotationAngle = fmod(m_rotationAngle, 360.f);
    m_collider->setRotation(m_rotationAngle);
}

void MoveableEntity::applyForces(const float& p_deltaTime)
{
    if (m_isKinematic)
        return;

    Player* player = m_entityManager->getPlayer();
    Collider* collider = this->getCollider();
    const std::lock_guard<std::mutex> forceGuard(this->m_entityMutex);

    player->setXCounterSpeed(0.f);
    for (MoveableEntity* otherEntity : m_entityManager->getMoveableEntities() )
    {
        if (otherEntity == this || otherEntity == player)
            continue;
        
        if (!collider->checkLeftCollisions(otherEntity, p_deltaTime) &&
            !collider->checkRightCollisions(otherEntity, p_deltaTime) &&
            !collider->checkUpperCollisions(otherEntity, p_deltaTime))
            continue;
        
        const float otherEntityMass = otherEntity->getMass();
        Vec2<float> otherEntityVelocity = otherEntity->getVelocity();
        Vec2<float> relativeVelocity = otherEntityVelocity - m_velocity;
        if (std::abs(relativeVelocity.x) < 0.1f && std::abs(relativeVelocity.y) < 0.1f)
            continue;
        
        Vec2<float> impulse = (m_mass * relativeVelocity + otherEntityMass * otherEntityVelocity) /
                (m_mass + otherEntityMass);
        if (this == player)
            player->setXCounterSpeed(impulse.x - 50.f);
        else
            applyForceTo(this, impulse);
        
        applyForceTo(otherEntity, -1.f * impulse);
        move(p_deltaTime);
        otherEntity->move(p_deltaTime);
        applyForceTo(otherEntity, -1.f * otherEntity->m_velocity * otherEntity->m_viscosity);
    }
}

void MoveableEntity::updateBeforeDelete() const
{
    Entity::updateBeforeDelete();
    const std::vector<MoveableEntity*> moveableEntities = m_entityManager->getMoveableEntities();
    std::vector<MoveableEntity*> remainingMoveableEntities = {};
    for(MoveableEntity* currMoveableEntity : moveableEntities)
    {
        if (currMoveableEntity == this)
            continue;
        remainingMoveableEntities.emplace_back(currMoveableEntity);
    }
    m_entityManager->setMoveableEntities(std::move(remainingMoveableEntities));
}
void MoveableEntity::applyForceTo(MoveableEntity* p_entity, const Vec2<float> p_velocity)
{
    p_entity->m_velocity.x += p_velocity.x;
    p_entity->m_velocity.y += p_velocity.y;
}

void MoveableEntity::resetEntity()
{
    setPositionKeepingInitialPos(m_initialPos.x, m_initialPos.y);
    m_collider->updatePosition();
    m_velocity = {0.f, 0.f};
    setRotation(0.f);
}

void MoveableEntity::applyGravity(const float& p_deltaTime)
{
    if (m_gravityReactive)
    {
        const float gravityDeltaVelocity = gravity * m_viscosity;
        const auto entities = m_entityManager->getEntities();
        const float gravityMovementThreshold = m_viscosity * gravity;
        const auto player = m_entityManager->getPlayer();
        
        //CHECK COLLISION WITH OTHER ENTITIES
        for (const auto entity : entities)
        {
            if (entity == this)
                continue;

            const std::lock_guard<std::mutex> collisionMutex(this->m_entityMutex);
            
            if (m_isKinematic || entity->getIsKinematic() || !m_collider->checkGroundCollision(entity, p_deltaTime))
                continue;

            if (this == player)
            {
                m_velocity.y = 0.f;
                player->setOnGround(true);
                return;
            }
            
            if (m_velocity.y > gravityMovementThreshold || m_velocity.y < -gravityMovementThreshold)
            {
                if (typeid(*entity) == typeid(MoveableEntity))
                    applyForceTo(reinterpret_cast<MoveableEntity*>(entity), {0.f, m_viscosity * m_mass});
                m_velocity.y *= -m_viscosity;
            }
            else
            {
                m_velocity.y = 0.f;
                return;
            }
        }
        const std::lock_guard<std::mutex> collisionMutex(this->m_entityMutex);
        if (this == player)
            player->setOnGround(false);
        m_velocity.y += gravityDeltaVelocity;
        move(Axis_e::y, m_velocity.y, p_deltaTime);
        m_collider->updatePosition();
    }
}

std::string MoveableEntity::prepareEntityInfos() const
{
    std::string moveableEntityInfosString = Entity::prepareEntityInfos() +
        "Entity's velocity : x : " + to_string(m_velocity.x) + " y : " + to_string(m_velocity.y) + "\n"
        "Entity's mass : " + to_string(m_mass) + " kg\n"
        "Entity's viscosity : " + to_string(m_viscosity) + "\n"
        "Gravity reactive : " + to_string(m_gravityReactive) + "\n"
        "Is kinematic : " + to_string(m_isKinematic) + "\n";
    return moveableEntityInfosString;
}

Player* Player::m_instance = nullptr;

Player* Player::getPlayerInstance(EntityManager* p_entityManager, const Uint16 p_id,
                                  SDL_Renderer* p_renderer, const char* p_path, const FRect& p_rect, const float p_mass,
                                  const float p_viscosity)
{
    if (m_instance == nullptr)
    {
        m_instance = new Player(p_entityManager, p_id, p_renderer, p_path, p_rect, p_mass, p_viscosity);
        return m_instance;
    }
    
    return m_instance;
}

Player::~Player()
{
    MoveableEntity::~MoveableEntity();
    m_entityManager->setPlayer(nullptr);
    m_instance = nullptr;
    Mix_FreeChunk(m_jumpSoundEffect);
}

void Player::applyMovements(const float p_deltaTime)
{
    move(Axis_e::x, m_velocity.x - m_xCounterSpeed, p_deltaTime);
    m_collider->updatePosition();
}

std::string Player::prepareEntityInfos() const
{
    return MoveableEntity::prepareEntityInfos() +
        "On ground : " + to_string(m_onGround) + '\n';
}

void Player::resetEntity()
{
    MoveableEntity::resetEntity();
    m_xCounterSpeed = 0;
}

Player::Player(EntityManager* p_entityManager, Uint16 p_id, SDL_Renderer* p_renderer,
               const char* p_path, const FRect& p_rect, const float p_mass, const float p_viscosity) : MoveableEntity(
                                                                                                           p_entityManager, p_id, p_renderer, p_path, p_rect, p_mass, p_viscosity), m_xCounterSpeed(0.f)
{
    m_name = "Player";
    m_jumpSoundEffect = Mix_LoadWAV("./sounds/jump.mp3");
}

Collectible::~Collectible()
{
    Entity::~Entity();
    Mix_FreeChunk(m_coinSoundEffect);
}

std::string Collectible::prepareEntityInfos() const
{
    const std::vector<Collectible*> collectibles = m_entityManager->getCollectibles();
    unsigned short int count = 1;
    for(const Collectible* collectible : collectibles)
    {
        if (collectible == this)
            break;
        count++;
    }
    std::string collectibleInfos = Entity::prepareEntityInfos() +
            "Collectible N°" + to_string(count) + "/" + to_string(collectibles.size()) + "\n";
    return collectibleInfos;
}

void Collectible::detectCollected(const FRect& p_playerRect)
{
    if (m_isCollected)
        return;

    m_isCollected = (m_rect.x + m_rect.w >= p_playerRect.x &&
        m_rect.x <= p_playerRect.x + p_playerRect.w &&
        m_rect.y + m_rect.h >= p_playerRect.y &&
        m_rect.y <= p_playerRect.y + p_playerRect.h);
    if (m_isCollected)
    {
        Mix_PlayChannel(2, m_coinSoundEffect, 0);
        m_texture = SDL_CreateTexture(m_renderer, SDL_TEXTUREACCESS_STATIC, 0, 0, 0);
    }
}

void Collectible::resetEntity()
{
    m_texture = m_textureSave;
    m_isCollected = false;
}
void Collectible::updateBeforeDelete() const
{
    Entity::updateBeforeDelete();
    
    const std::vector<Collectible*> collectibles = m_entityManager->getCollectibles();
    std::vector<Collectible*> remainingCollectibles = {};
    for (Collectible* currCollectible : collectibles)
    {
        if (currCollectible == this)
            continue;
        remainingCollectibles.emplace_back(currCollectible);
    }
    m_entityManager->setCollectibles(std::move(remainingCollectibles));
}
