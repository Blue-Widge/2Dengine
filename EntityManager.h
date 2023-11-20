#pragma once
#include <SDL_mixer.h>
#include <vector>
#include "Entity.h"

class EntityManager
{
public:
    explicit EntityManager(SDL_Renderer* p_renderer) : m_renderer(p_renderer), m_nbEntities(0), m_entities(0),
                                                       m_moveableEntities(0), m_player(nullptr)
    {
    }

    ~EntityManager();
    Entity* addEntity(const char* p_texturePath, const FRect& p_rect);
    inline std::vector<Entity*>& getEntities() { return m_entities; }
    inline std::vector<MoveableEntity*>& getMoveableEntities() { return m_moveableEntities; }
    inline std::vector<Entity*>& getStaticEntities() { return m_staticEntities; }
    inline Player* getPlayer() const { return m_player; }
    inline std::vector<Collectible*>& getCollectibles() { return m_collectibles; }
    inline void setEntities(const std::vector<Entity*>&& p_entities) { m_entities = p_entities; }
    inline void setMoveableEntities(const std::vector<MoveableEntity*>&& p_moveableEntities)
    {
        m_moveableEntities = p_moveableEntities;
    }
    inline void setPlayer(Player* p_player) { m_player = p_player; }
    inline void setCollectibles(const std::vector<Collectible*>&& p_collectibles) { m_collectibles = p_collectibles; }
    inline void setStaticEntities(const std::vector<Entity*>&& p_staticEntities) { m_staticEntities = p_staticEntities; }
    static void deleteEntityUpdate(const Entity* p_entity);
    MoveableEntity* addMoveableEntity(const char* p_texturePath, const FRect& p_rect, float p_mass);
    Player* addPlayer(const char* p_texturePath, const FRect& p_rect, float p_mass);
    Collectible* addCollectible(const char* p_texturePath, const FRect& p_rect);
    void resetEntities() const;
    void deleteEntities();
    void solveInsidersEntities(const float& p_deltaTime) const;
private:
    SDL_Renderer* m_renderer;
    Uint16 m_nbEntities;
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_staticEntities;
    std::vector<MoveableEntity*> m_moveableEntities;
    std::vector<Collectible*> m_collectibles;
    Player* m_player;
};
