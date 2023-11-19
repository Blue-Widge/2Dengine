#include "EntityChooser.h"

#include <algorithm>
#include <SDL_image.h>

#include "Hierarchy.h"

EntityChooser::EntityChooser(SDL_Renderer* p_renderer, EntityManager* p_entityManager, Hierarchy* p_hierarchy) :
    m_renderer(p_renderer), m_entityManager(p_entityManager), m_font(TTF_OpenFont(BASE_FONT, 12)),
    m_hierarchy(p_hierarchy)
{
    m_rect = {HIERARCHY_WIDTH, SCENE_HEIGHT, SCENE_WIDTH, SCREEN_HEIGHT - SCENE_HEIGHT};
    SDL_Surface* surface = SDL_CreateRGBSurface(0, m_rect.w, m_rect.h, 32, 0, 0, 0, 0);
    const SDL_Rect temp = {0, 0, m_rect.w, m_rect.h};
    SDL_FillRect(surface, &temp, SDL_MapRGBA(surface->format, 0, 255, 0, 255));
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    SDL_FreeSurface(surface);
    m_choiceRect = {HIERARCHY_WIDTH + 10, (SCREEN_HEIGHT - SCENE_HEIGHT) / 2 + SCENE_HEIGHT, 80, 80};
    m_choiceRect.y -= m_choiceRect.h / 2;
    
    constexpr unsigned short int spaceBetween = 20;
    addChoice(BASE_TEXTURE, "Entity", spaceBetween);
    addChoice(BASE_MOVEABLE_TEXTURE, "Moveable entity", spaceBetween);
    addChoice(BASE_COLLECTIBLE_TEXTURE, "Collectible", spaceBetween);    
    addChoice(BASE_PLAYER_TEXTURE, "Player", spaceBetween);    
    
    //AFTER ADDING CHOICES
    unsigned short int count = 0;
    for(Choice& choice : m_choices)
    {
        choice.m_nameRect.x += (spaceBetween + m_choiceRect.w ) * count + (choice.m_shownTextureRect.w - choice.m_nameRect.w) / 2;
        choice.m_shownTextureRect.x += (spaceBetween + m_choiceRect.w) * count;
        count++;
    }

    //TODO: add delete Entity button
}


EntityChooser::~EntityChooser()
{
    for(const Choice& choice : m_choices)
    {
        SDL_DestroyTexture(choice.m_nameTexture);
        SDL_DestroyTexture(choice.m_shownTexture);
    }
}

void EntityChooser::displayEntityChooser() const
{
    SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_rect);
    for (const Choice& choice : m_choices)
    {
        SDL_RenderCopy(m_renderer, choice.m_nameTexture, nullptr, &choice.m_nameRect);
        SDL_RenderCopy(m_renderer, choice.m_shownTexture, nullptr, &choice.m_shownTextureRect);
    }
}

void EntityChooser::addChoice(const char* p_path, const std::string& p_entityName, const unsigned short int p_spaceBetween)
{
    SDL_Surface* surface = IMG_Load(p_path);
    SDL_Surface* textSurface = TTF_RenderText_Solid_Wrapped(m_font, p_entityName.c_str(), m_fontColor, m_choiceRect.w + p_spaceBetween / 2);
    const SDL_Rect nameRect = {m_choiceRect.x, m_choiceRect.y - textSurface->h, textSurface->w, textSurface->h};
    m_choices.push_back({p_entityName, SDL_CreateTextureFromSurface(m_renderer, textSurface),
        SDL_CreateTextureFromSurface(m_renderer, surface), nameRect, m_choiceRect});
    SDL_FreeSurface(surface);
    SDL_FreeSurface(textSurface);
}

bool EntityChooser::detectChosenEntity(const int p_x, const int p_y) const
{
    for(const Choice& choice : m_choices)
    {
        if (detectButtonClicked(p_x, p_y, choice.m_shownTextureRect) || detectButtonClicked(p_x, p_y, choice.m_nameRect))
        {
            if (choice.m_name == "Entity")
                    m_entityManager->addEntity(BASE_TEXTURE, {SCENE_WIDTH / 2, SCENE_HEIGHT / 2, 50.f, 50.f});
            else if (choice.m_name == "Moveable entity")
                    m_entityManager->addMoveableEntity(BASE_MOVEABLE_TEXTURE, {SCENE_WIDTH / 2, SCENE_HEIGHT / 2, 50.f, 50.f}, 10.f);
            else if (choice.m_name == "Collectible")
                    m_entityManager->addCollectible(BASE_COLLECTIBLE_TEXTURE, {SCENE_WIDTH / 2, SCENE_HEIGHT / 2, 50.f, 50.f});
            else if (choice.m_name == "Player")
                m_entityManager->addPlayer(BASE_PLAYER_TEXTURE, {SCENE_WIDTH / 2, SCENE_HEIGHT / 2, 30, 70}, 80);
            m_hierarchy->updateHierarchy();
            return true;
        }
    }
    return false;
}
