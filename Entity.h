#pragma once
#include <SDL.h>
#include <string>
#include <chrono>
#include <mutex>
#include <SDL_mixer.h>

#include "utils.h"
#include "Collider.h"

using std::to_string;
class EntityManager;

class Entity
{
public:
    Entity(EntityManager* p_entityManager, Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path,
           const FRect& p_rect);
    virtual ~Entity();

    virtual void setPosition(float p_x, float p_y);
    Vec2<float> getPosition() const { return {m_rect.x, m_rect.y}; }
    void setRotation(float p_rotationAngle);
    float getRotation() const { return m_rotationAngle; }
    void setSize(float p_w, float p_h);
    Vec2<float> getSize() const { return {m_rect.w, m_rect.h}; }
    FRect getEntityRect() const { return m_rect; }
    bool operator==(const Entity& p_entity) const;
    Collider* getCollider() const { return m_collider; }
    SDL_Texture* getTexture() const { return m_texture; }
    inline void setTexture(const char* p_path);
    Uint16 getId() const { return m_id; }
    virtual std::string prepareEntityInfos() const;
    bool getIsKinematic() const { return m_isKinematic; }
    void setKinematic(const bool p_kinematic) { m_isKinematic = p_kinematic; }
    std::string getName() const { return m_name; }
    void setName(const std::string& p_name) { m_name = p_name; }
    virtual void updateBeforeDelete() const;
protected:
    Uint16 m_id = 0;
    std::string m_name;

    SDL_Renderer* m_renderer = nullptr;
    FRect m_rect;
    SDL_Texture* m_texture = nullptr;
    EntityManager* m_entityManager = nullptr;

    float m_rotationAngle = 0.f;
    Collider* m_collider;
    bool m_isKinematic = false;
    Vec2<float> m_velocity;
};

class MoveableEntity : public Entity
{
public:
    MoveableEntity(EntityManager* p_entityManager, Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path,
                   const FRect& p_rect, float p_mass);
    MoveableEntity(EntityManager* p_entityManager, Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path,
                   const FRect& p_rect, float p_mass, float p_viscosity);
    ~MoveableEntity() override;
    void setPosition(float p_x, float p_y) override;
    void setVelocity(const Vec2<float> p_velocity) { m_velocity = p_velocity; }
    void setPositionKeepingInitialPos(float p_x, float p_y);
    void move(Axis_e p_axis, float p_moveSpeed, float p_deltaTime);
    void move(float p_deltaTime);
    void rotate(float p_rotationSpeed, float p_deltaTime);
    void applyForces(const float& p_deltaTime);
    void setMass(const float p_mass) { m_mass = p_mass; }
    float getMass() const { return m_mass; }
    void setGravityReactive(const bool p_gravityReactive) { m_gravityReactive = p_gravityReactive; }
    bool getGravityReactive() const { return m_gravityReactive; }
    Vec2<float> getVelocity() const { return m_velocity; }
    virtual void resetEntity();
    void applyGravity(const float& p_deltaTime);
    std::string prepareEntityInfos() const override;
    void setViscosity(const float p_viscosity) { m_viscosity = p_viscosity; }
    float getViscosity() const { return m_viscosity; }
    std::mutex& getMutex() { return m_entityMutex; }
    void updateBeforeDelete() const override;
protected:
    static void applyForceTo(MoveableEntity* p_entity, Vec2<float> p_velocity);
    bool m_gravityReactive = true;
    Vec2<float> m_initialPos;
    constexpr static float gravity = 9.81f;
    float m_mass = 0.f;
    float m_viscosity = 1.f;
    std::mutex m_entityMutex;
};

class Player : public MoveableEntity
{
public:
    static Player* getPlayerInstance(EntityManager* p_entityManager, Uint16 p_id,
                                     SDL_Renderer* p_renderer, const char* p_path, const FRect& p_rect, float p_mass,
                                     float p_viscosity);
    ~Player() override;
    void setOnGround(const bool p_onGround) { m_onGround = p_onGround; }
    bool getOnGround() const { return m_onGround; }
    void setXVelocity(const float p_x) { m_velocity.x = p_x; }
    void setYVelocity(const float p_y) { m_velocity.y = p_y; }
    void applyMovements(float p_deltaTime);
    std::string prepareEntityInfos() const override;
    void setXCounterSpeed(const float& p_counterSpeed) { m_xCounterSpeed = p_counterSpeed; }
    void resetEntity() override;
    void playJumpSound() const { Mix_PlayChannel(2, m_jumpSoundEffect, 0); }
private:
    Player(EntityManager* p_entityManager, Uint16 p_id,
           SDL_Renderer* p_renderer, const char* p_path, const FRect& p_rect, float p_mass,
           float p_viscosity);
private:
    static Player* m_instance;
    bool m_onGround = false;
    float m_xCounterSpeed;
    Mix_Chunk* m_jumpSoundEffect;
};

class Collectible : public Entity
{
public:
    Collectible(EntityManager* p_entityManager, const Uint16 p_id, SDL_Renderer* p_renderer, const char* p_path,
                const FRect& p_rect) : Entity(p_entityManager, p_id, p_renderer, p_path, p_rect)
    {
        m_name = "Collectible " + to_string(m_id);
        m_isKinematic = true;
        m_textureSave = m_texture;
        m_coinSoundEffect = Mix_LoadWAV("./sounds/coin.mp3");
    }
    ~Collectible() override;
    std::string prepareEntityInfos() const override;
    void detectCollected(const FRect& p_playerRect);
    bool getIsCollected() const { return m_isCollected; }
    void resetEntity();
    void updateBeforeDelete() const override;
private:
    bool m_isCollected = false;
    SDL_Texture* m_textureSave;
    Mix_Chunk* m_coinSoundEffect;
};
