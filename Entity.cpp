#include "Entity.h"

#include <SDL_image.h>

#include "EntityManager.h"

Entity::Entity(EntityManager* p_entityManager, const Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path,
               const FRect& p_rect) : m_id(p_id), m_renderer(p_renderer), m_rect(p_rect),
                                      m_entityManager(p_entityManager)
{
    setTexture(p_path);
}

Entity::~Entity()
{
    SDL_DestroyTexture(m_texture);
    delete m_collider;
}

void Entity::setPosition(const float p_x, const float p_y)
{
    m_rect.x = (p_x + m_rect.w) > SCENE_WIDTH ? (SCENE_WIDTH - m_rect.w) : p_x;
    m_rect.y = (p_y + m_rect.h) > SCENE_HEIGHT ? (SCENE_HEIGHT - m_rect.h) : p_y;
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
    m_collider = reinterpret_cast<Collider*>(new BoxCollider(this, m_rect));
}

std::string Entity::prepareEntityInfos() const
{
    const SDL_Rect entityRect = convertFRect(getEntityRect());
    const SDL_Rect colliderRect = convertFRect(getCollider()->getColliderRect());

    //no need to recreate these variables at runtime but I don't see them as object's attribute
    const std::string entityInfosString = "Entity's ID : " + to_string(m_id) + "\n"
        "Entity's position : " + "x : " + to_string(entityRect.x) + " y : " + to_string(entityRect.y) + "\n"
        "Entity's rotation : " + to_string(m_rotationAngle) + "\n"
        "Entity's size : " + "x : " + to_string(entityRect.w) + " y : " + to_string(entityRect.h) + "\n"
        "Collider's position : " + "x : " + to_string(colliderRect.x) + " y : " + to_string(colliderRect.y) +
        "\n"
        "Collider's size : " + "w : " + to_string(colliderRect.w) + " h : " + to_string(colliderRect.h) + "\n"
        "Entity's texture : \n";
    return entityInfosString;
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
    m_velocity = {0.f, 0.f};
    m_initialPos.x = m_rect.x = p_rect.x;
    m_initialPos.y = m_rect.y = p_rect.y;
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
        if (colliderRect.x + colliderRect.w > SCENE_WIDTH)
            deltaPos = SCENE_WIDTH - colliderRect.w;
        m_rect.x += deltaPos;
        return;
    }
    if (colliderRect.y + colliderRect.h > SCENE_HEIGHT)
        deltaPos = SCENE_HEIGHT - colliderRect.h;
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
        const auto entities = m_entityManager->getStaticEntities();
        const float gravityMovementThreshold = m_viscosity * gravity;
        
        //CHECK COLLISION WITH OTHER ENTITIES
        for (const auto entity : entities)
        {
            if (entity == this)
                continue;
            const auto player = m_entityManager->getPlayer();

            const std::lock_guard<std::mutex> collisionMutex(this->m_entityMutex);
            
            if (!m_isKinematic && m_collider->checkGroundCollision(entity, p_deltaTime))
            {
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
                break;
            }
            
            if (this == player)
            {
                player->setOnGround(false);
            }
        }
        
        const std::lock_guard<std::mutex> collisionMutex(this->m_entityMutex);
        m_velocity.y += gravityDeltaVelocity;
        move(Axis_e::y, m_velocity.y, p_deltaTime);
        m_collider->updatePosition();
    }
}

std::string MoveableEntity::prepareEntityInfos() const
{
    const std::string moveableEntityInfosString = Entity::prepareEntityInfos() +
        "Entity's mass : " + to_string(m_mass) + "kg\n"
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
        return new Player(p_entityManager, p_id, p_renderer, p_path, p_rect, p_mass, p_viscosity);
    m_instance->m_mass = p_mass;
    m_instance->m_viscosity = p_viscosity;
    return m_instance;
}

void Player::applyMovements(const float p_deltaTime)
{
    move(Axis_e::x, m_velocity.x - m_xCounterSpeed, p_deltaTime);
    m_collider->updatePosition();
}

std::string Player::prepareEntityInfos() const
{
    return MoveableEntity::prepareEntityInfos();
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
}

void Collectible::detectCollected(const FRect& p_playerRect)
{
    if (m_isCollected)
        return;

    m_isCollected = (m_rect.x >= p_playerRect.x && m_rect.x <= p_playerRect.x + p_playerRect.w &&
        m_rect.y >= p_playerRect.y && m_rect.y <= p_playerRect.y + p_playerRect.h);
    if (m_isCollected)
        m_texture = SDL_CreateTexture(m_renderer, SDL_TEXTUREACCESS_STATIC, 0, 0, 0);
}

void Collectible::resetEntity()
{
    m_texture = m_textureSave;
}
