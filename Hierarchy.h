#pragma once
#include <SDL_render.h>
#include <SDL_ttf.h>

#include "EntityManager.h"

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
    
    SDL_Texture* m_infosTexture;
    Vec2<int> m_infosPos;
    
    EntityManager* m_entityManager;
    std::string m_hierarchyInfos;
    std::vector<std::tuple<SDL_Texture*, SDL_Rect>> m_infosTextureAndRect;
};
