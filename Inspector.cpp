#include "Inspector.h"

#include <iostream>
#include <ostream>

#include "Entity.h"

Inspector::Inspector(const int p_posX, const int p_width, SDL_Renderer* p_renderer, TTF_Font* p_font) :
m_renderer(p_renderer), m_rect({0}), m_titleRect({0}), m_font(p_font), m_selectionRect({0})
{
    m_rect = {p_posX, 0, p_width, SCREEN_HEIGHT};
    const SDL_Rect temp = {0, 0, m_rect.w, m_rect.h};
    SDL_Surface* surface = SDL_CreateRGBSurface(0, temp.w, temp.h, 32, 0, 0, 0, 0);
    SDL_FillRect(surface, &temp, SDL_MapRGBA(surface->format, 255, 0, 0, 255));
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, "Inspector", m_fontColor);
    m_titleTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    m_titleRect = {m_rect.x + textSurface->w, m_rect.y, textSurface->w, textSurface->h};

    SDL_Surface* selectionSurface = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
    SDL_FillRect(selectionSurface, nullptr, SDL_MapRGBA(surface->format, 0, 255, 0, 10));
    m_selectionTexture = SDL_CreateTextureFromSurface(m_renderer, selectionSurface);
    SDL_SetTextureBlendMode(m_selectionTexture, SDL_BLENDMODE_ADD);

    SDL_FreeSurface(surface);
    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(selectionSurface);
}

void Inspector::displayInspector() const
{
    SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_rect);
    SDL_RenderCopy(m_renderer, m_titleTexture, nullptr, &m_titleRect);
    if (m_entityPtr == nullptr)
        return;
    SDL_RenderCopy(m_renderer, m_selectionTexture, nullptr, &m_selectionRect);
}

void Inspector::selectEntity(Entity* p_entity)
{
    m_entityPtr = p_entity;
    if (m_entityPtr == nullptr)
        return;
    m_selectionRect = ConvertFRect(m_entityPtr->getEntityRect());
    m_selectionRect.x += (SCREEN_WIDTH - SCENE_WIDTH) / 2;

    //TODO display entities info
    int id = p_entity->getId();
    FRect entityRect = p_entity->getEntityRect();
    FRect colliderRect = p_entity->getCollider()->getColliderRect();
    SDL_Texture* texture = p_entity->getTexture();
    //TODO display texture with possibility to change it 
}

void Inspector::selectEntity(MoveableEntity* p_entity)
{
    Inspector::selectEntity(dynamic_cast<Entity*>(p_entity));
    bool isKinematic = p_entity->getIsKinematic();
    float viscosity = p_entity->getViscosity();
}

void Inspector::selectEntity(Player* p_entity)
{
    Inspector::selectEntity(dynamic_cast<MoveableEntity*>(p_entity));
    //ANY PLAYER ONLY INFOS    
}

void Inspector::modifyInfoValue(const int p_x, const int p_y) const
{
    //TODO detect if a pos has been selected
    
    SDL_Window* inputWindow = SDL_CreateWindow("Input", p_x, p_y, 100, 30, 0);
    SDL_Renderer* inputRenderer = SDL_CreateRenderer(inputWindow, -1, SDL_RENDERER_SOFTWARE);
    SDL_Rect inputRect = {p_x, p_y, 100, 30};
    SDL_StartTextInput();

    std::string userInput = "";
    bool done = false;
    while(!done)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                done = true;
                break;
            }
            if (e.type == SDL_TEXTINPUT)
            {
                userInput += e.text.text;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_ESCAPE)
                {
                    try
                    {
                        if (userInput.length() == 0)
                        {
                            userInput = "0";
                            done = true;
                            break;
                        }
                        int test = std::stoi(userInput);
                        done = true;
                    }
                    catch(std::invalid_argument const& ex)
                    {
                        std::cerr << "Please only enter numbers : " << ex.what() << std::endl;
                        userInput = "";
                    }
                }
            }
        }
        SDL_RenderClear(inputRenderer);
        SDL_SetRenderDrawColor(inputRenderer, 100, 100, 100, 255);
        SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, userInput.c_str(), m_fontColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(inputRenderer, textSurface);
        SDL_FreeSurface(textSurface);

        SDL_QueryTexture(texture, nullptr, nullptr, &inputRect.w, &inputRect.h);
        SDL_Rect textRect = {0, 0, inputRect.w, inputRect.h};
        SDL_RenderCopy(inputRenderer, texture, nullptr, &textRect);
        SDL_DestroyTexture(texture);
        SDL_RenderPresent(inputRenderer);
    }
    SDL_StopTextInput();
    SDL_HideWindow(inputWindow);
    SDL_DestroyRenderer(inputRenderer);
    SDL_DestroyWindow(inputWindow);
}
