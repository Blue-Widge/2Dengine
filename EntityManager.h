#pragma once
#include <vector>

#include "Entity.h"

class EntityManager
{
public:
    EntityManager(SDL_Renderer* p_renderer) : m_renderer(p_renderer), m_nbEntities(0), m_entities(0),
                                              m_moveableEntities(0), m_player(nullptr)
    {
    }

    ~EntityManager();
    Entity* addEntity();
    Entity* addEntity(const char* p_texturePath);
    Entity* addEntity(const char* p_texturePath, const FRect& p_rect);
    MoveableEntity* addMoveableEntity(const char* p_texturePath, const FRect& p_rect, float p_mass);
    Player* addPlayer(const char* p_texturePath, const FRect& p_rect, float p_mass);
    inline std::vector<MoveableEntity*> getMoveableEntities() { return m_moveableEntities; }
    inline std::vector<Entity*> getUnmoveableEntities() { return m_unmoveableEntities; }
    void resetEntities() const;
    void deleteEntities() const;
    inline std::vector<Entity*> getEntities() const { return m_entities; }
    inline Player* getPlayer() const { return m_player; }
    void solveInsidersEntities() const;
private:
    SDL_Renderer* m_renderer;
    Uint16 m_nbEntities;
    std::vector<Entity*> m_entities;
    std::vector<Entity*> m_unmoveableEntities;
    std::vector<MoveableEntity*> m_moveableEntities;
    Player* m_player;
};
