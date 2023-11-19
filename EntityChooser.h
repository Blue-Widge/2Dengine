#pragma once
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <vector>

#include "Entity.h"
#include "EntityManager.h"
#include "Hierarchy.h"

struct Choice
{
    std::string m_name;
    SDL_Texture* m_nameTexture;
    SDL_Texture* m_shownTexture;
    SDL_Rect m_nameRect;
    SDL_Rect m_shownTextureRect;
};

class EntityChooser
{
public:
    EntityChooser(SDL_Renderer* p_renderer, EntityManager* p_entityManager, Hierarchy* p_hierarchy);
    ~EntityChooser();
    void displayEntityChooser() const;
    void addChoice(const char* p_path, const std::string& p_entityName, unsigned short int p_spaceBetween);
    bool detectChosenEntity(int p_x, int p_y) const;
private:
    SDL_Renderer* m_renderer;
    EntityManager* m_entityManager;
    SDL_Texture* m_texture;
    TTF_Font* m_font;
    SDL_Rect m_rect;

    std::vector<Choice> m_choices;
    SDL_Color m_fontColor = {100,  100, 100, 255};
    SDL_Rect m_choiceRect;
    Hierarchy* m_hierarchy;
};
