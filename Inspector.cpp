#include "Inspector.h"

#include <complex>
#include <iostream>
#include <ostream>
#include <sstream>

#include "Entity.h"

Inspector::Inspector(SDL_Renderer* p_renderer, TTF_Font* p_font) :
m_renderer(p_renderer), m_font(p_font), m_selectionRect({0})
{
    m_rect = {HIERARCHY_WIDTH + SCENE_WIDTH, 0, INSPECTOR_WIDTH, INSPECTOR_HEIGHT};
    SDL_Surface* surface = SDL_CreateRGBSurface(0, m_rect.w, m_rect.h, 32, 0, 0, 0, 0);
    const SDL_Rect temp = {0, 0, m_rect.w, m_rect.h};
    SDL_FillRect(surface, &temp, SDL_MapRGBA(surface->format, 255, 0, 0, 255));
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, "Inspector", m_fontColor);
    m_titleTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    m_titleRect = {m_rect.x + (SCREEN_WIDTH - m_rect.x) / 2 - textSurface->w / 2, m_rect.y, textSurface->w, textSurface->h};

    SDL_Surface* selectionSurface = SDL_CreateRGBSurface(0, 32, 32, 32, 0, 0, 0, 0);
    SDL_FillRect(selectionSurface, nullptr, SDL_MapRGBA(surface->format, 0, 255, 0, 10));
    m_selectionTexture = SDL_CreateTextureFromSurface(m_renderer, selectionSurface);
    SDL_SetTextureBlendMode(m_selectionTexture, SDL_BLENDMODE_ADD);

    SDL_FreeSurface(surface);
    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(selectionSurface);
}

void Inspector::displayInspector()
{
    SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_rect);
    SDL_RenderCopy(m_renderer, m_titleTexture, nullptr, &m_titleRect);
    if (m_entityPtr == nullptr)
        return;
    std::string infos;
    if (m_entityPtr != m_lastEntityPtr)
    {
        infos = m_entityPtr->prepareEntityInfos();
        m_lastEntityPtr = m_entityPtr == nullptr ? m_lastEntityPtr : m_entityPtr;
    }
    displayEntityInfos(infos, m_entityPtr->getId());
    SDL_RenderCopy(m_renderer, m_selectionTexture, nullptr, &m_selectionRect);
}

bool Inspector::selectEntity(Entity* p_entity)
{
    m_entityPtr = p_entity;
    if (!p_entity)
        return false;
    m_selectionRect = convertFRect(m_entityPtr->getEntityRect());
    m_selectionRect.x += HIERARCHY_WIDTH;
    return true;
}

void Inspector::displayEntityInfos(const std::string& p_string, const int p_entityId)
{
    static int lastEntityId = -1;
    if (lastEntityId == p_entityId)
    {
        for (const auto& infosTextureAndRect : m_infosTexturesAndRect)
        {
            SDL_Rect currRect = std::get<1>(infosTextureAndRect);
            SDL_RenderCopy(m_renderer, std::get<0>(infosTextureAndRect), nullptr, &currRect);
        }
        return;
    }

    for (auto infosTextureAndRect : m_infosTexturesAndRect)
    {
        SDL_DestroyTexture(std::get<0>(infosTextureAndRect));
    }
    m_infosTexturesAndRect.clear();
    
    std::vector<std::string> lines;
    std::istringstream iss(p_string);
    std::string line;
    int nbLines = 0;
    while(std::getline(iss, line))
    {
        lines.push_back(line);
        ++nbLines;
    }
    TTF_SetFontSize(m_font, 12);
    for(int nbLine = 0; nbLine < nbLines; ++nbLine)
    {
        std::string currLine = lines.at(nbLine);
        SDL_Surface* infosSurface = TTF_RenderText_Solid(m_font, currLine.c_str(), m_fontColor);
        SDL_Texture* infosTexture = SDL_CreateTextureFromSurface(m_renderer, infosSurface);
        const SDL_Rect infosRect = {
            m_rect.x, m_rect.y + m_titleRect.h + nbLine * infosSurface->h,
            infosSurface->w, infosSurface->h };
        SDL_RenderCopy(m_renderer, infosTexture, nullptr, &infosRect);
        m_infosTexturesAndRect.emplace_back(infosTexture, infosRect);
        SDL_FreeSurface(infosSurface);
    }
    lastEntityId = p_entityId;
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
