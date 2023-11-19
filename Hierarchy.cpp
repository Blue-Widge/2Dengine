#include "Hierarchy.h"

#include "SDLHandler.h"

Hierarchy::Hierarchy(SDL_Renderer* p_renderer, TTF_Font* p_font, EntityManager* p_entityManager) : m_renderer(p_renderer),
    m_font(p_font), m_entityManager(p_entityManager)
{
    m_rect = {0, 0, HIERARCHY_WIDTH, HIERARCHY_HEIGHT};
    SDL_Surface* surface = SDL_CreateRGBSurface(0, m_rect.w, m_rect.h, 32, 0, 0, 0, 0);
    const SDL_Rect temp = {0, 0, m_rect.w, m_rect.h};
    SDL_FillRect(surface, &temp, SDL_MapRGBA(surface->format, 0, 0, 255, 255));
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    SDL_Surface* titleSurface = TTF_RenderText_Solid(m_font, "Hierarchy", m_fontColor);
    m_titleTexture = SDL_CreateTextureFromSurface(m_renderer, titleSurface);
    m_titleRect = {
        m_rect.x + HIERARCHY_WIDTH / 2 - titleSurface->w / 2, m_rect.y, titleSurface->w, titleSurface->h };

    m_infosPos = {HIERARCHY_WIDTH / 50, m_titleRect.y + m_titleRect.h};
    
    SDL_FreeSurface(surface);
    SDL_FreeSurface(titleSurface);
    updateHierarchy();
}

Hierarchy::~Hierarchy()
{
    SDL_DestroyTexture(m_texture);
}

void Hierarchy::updateHierarchy()
{
    m_hierarchyInfos.clear();
    for(const EntityInfo& entityInfo : m_entityInfos)
        SDL_DestroyTexture(entityInfo.m_textTexture);
    m_entityInfos.clear();
    
    const std::vector<Entity*> entities = m_entityManager->getEntities();
    unsigned short int count = 0;
    for (const Entity* entity : entities)
    {
        std::string currString = "Bloc " + std::to_string(entity->getId()) + '\n';
        m_hierarchyInfos.append(currString);
        SDL_Surface* currEntityInfoSurface = TTF_RenderText_Solid_Wrapped(m_font, currString.c_str(), m_fontColor,
            HIERARCHY_WIDTH);
        SDL_Rect currEntityRect = {m_infosPos.x, m_infosPos.y + count * currEntityInfoSurface->h, currEntityInfoSurface->w, currEntityInfoSurface->h};
        m_entityInfos.emplace_back(SDL_CreateTextureFromSurface(m_renderer, currEntityInfoSurface), currEntityRect, entity);
        SDL_FreeSurface(currEntityInfoSurface);
        ++count;
    }
}

void Hierarchy::displayHierarchy() const
{
    SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_rect);
    SDL_RenderCopy(m_renderer, m_titleTexture, nullptr, &m_titleRect);
    for(const EntityInfo& entityInfo : m_entityInfos)
    {
        const SDL_Rect& currRect = entityInfo.m_textRect;
        SDL_RenderCopy(m_renderer, entityInfo.m_textTexture, nullptr, &currRect);
    }
}
