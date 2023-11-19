#pragma once
#include <SDL_render.h>
#include <SDL_ttf.h>

#include "EntityManager.h"

struct EntityInfo
{
    EntityInfo(SDL_Texture* p_textTexture, SDL_Rect p_textRect, const Entity* p_entityPtr) :
        m_textTexture(p_textTexture), m_textRect(p_textRect), m_entityPtr(p_entityPtr) {}
    SDL_Texture* m_textTexture;
    SDL_Rect m_textRect;
    const Entity* m_entityPtr;
};

class Hierarchy
{
public:
    Hierarchy(SDL_Renderer* p_renderer, TTF_Font* p_font, EntityManager* p_entityManager);
    ~Hierarchy();
    void updateHierarchy();
    void displayHierarchy() const;
private:
    SDL_Renderer* m_renderer;
    TTF_Font* m_font;
    SDL_Color m_fontColor = {200, 200, 200, 255};
    SDL_Texture* m_texture;
    SDL_Rect m_rect;
    
    SDL_Texture* m_titleTexture;
    SDL_Rect m_titleRect;
    
    Vec2<int> m_infosPos;
    
    EntityManager* m_entityManager;
    std::string m_hierarchyInfos;
    std::vector<EntityInfo> m_entityInfos;
};
