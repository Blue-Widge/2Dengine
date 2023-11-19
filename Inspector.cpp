#include "Inspector.h"

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

    SDL_Surface* textSurface = TTF_RenderText_Solid_Wrapped(m_font, "Inspector", m_fontColor, INSPECTOR_WIDTH);
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
    if (m_entityChanged || m_entityPtr != m_lastEntityPtr)
    {
        infos = m_entityPtr->prepareEntityInfos();
        m_lastEntityPtr = m_entityPtr == nullptr ? m_lastEntityPtr : m_entityPtr;
    }
    displayEntityInfos(infos, m_entityPtr->getId());
    SDL_RenderCopy(m_renderer, m_selectionTexture, nullptr, &m_selectionRect);
    m_entityChanged = false;
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
    if (!m_entityChanged && lastEntityId == p_entityId)
    {
        for (const auto& entityInfo : m_entityInfos)
        {
            SDL_Rect currRect = entityInfo.m_textRect;
            SDL_RenderCopy(m_renderer, entityInfo.m_textTexture, nullptr, &currRect);
        }
        return;
    }

    for (const auto& entityInfo : m_entityInfos)
    {
        SDL_DestroyTexture(entityInfo.m_textTexture);
    }
    m_entityInfos.clear();
    
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
        SDL_Surface* infosSurface = TTF_RenderText_Solid_Wrapped(m_font, currLine.c_str(), m_fontColor, INSPECTOR_WIDTH);
        SDL_Texture* infosTexture = SDL_CreateTextureFromSurface(m_renderer, infosSurface);
        const SDL_Rect infosRect = {
            m_rect.x, m_rect.y + m_titleRect.h + nbLine * infosSurface->h,
            infosSurface->w, infosSurface->h };
        SDL_RenderCopy(m_renderer, infosTexture, nullptr, &infosRect);
        m_entityInfos.emplace_back(infosTexture, infosRect, currLine.substr(0, currLine.find(" :")));
        SDL_FreeSurface(infosSurface);
    }
    TTF_SetFontSize(m_font, 20);
    lastEntityId = p_entityId;
}

void Inspector::modifyInfoValue(const int p_x, const int p_y)
{
    bool clickedOnInfo = false;
    std::string infoName;
    SDL_Rect infoRect = {p_x,p_y, 100, 20};
    for(const Inspector::EntityInfo& entityInfo : m_entityInfos)
    {
        if (detectButtonClicked(p_x, p_y, entityInfo.m_textRect))
        {
            infoName = entityInfo.m_infoName;
            infoRect = entityInfo.m_textRect;
            clickedOnInfo = true;
            break;
        }
    }
    if (!clickedOnInfo || infoName.find("ID") != std::string::npos ||
        infoName.find("velocity") != std::string::npos || infoName.find("ground") != std::string::npos ||
        infoName.find("Collectible") != std::string::npos)
        return;
    
    SDL_Window* inputWindow = SDL_CreateWindow("Input", p_x, p_y + infoRect.h, 100, infoRect.h,
        SDL_WINDOW_POPUP_MENU | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
    SDL_Renderer* inputRenderer = SDL_CreateRenderer(inputWindow, -1, SDL_RENDERER_SOFTWARE);
    SDL_Rect inputRect = infoRect;
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
                userInput = "";
                done = true;
                break;
            }
            if (e.type == SDL_TEXTINPUT)
            {
                userInput += e.text.text;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_RETURN)
                    done = true;
                else if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    userInput.clear();
                    done = true;
                }
            }
        }
        TTF_SetFontSize(m_font, 12);
        SDL_RenderClear(inputRenderer);
        SDL_SetRenderDrawColor(inputRenderer, 255, 255, 255, 255);
        SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, userInput.c_str(), {0, 0, 0, 255});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(inputRenderer, textSurface);
        SDL_FreeSurface(textSurface);

        SDL_QueryTexture(texture, nullptr, nullptr, &inputRect.w, &inputRect.h);
        SDL_Rect textRect = {0, 0, inputRect.w, inputRect.h};
        SDL_RenderCopy(inputRenderer, texture, nullptr, &textRect);
        SDL_DestroyTexture(texture);
        SDL_RenderPresent(inputRenderer);
    }
    TTF_SetFontSize(m_font, 20);
    SDL_StopTextInput();
    SDL_HideWindow(inputWindow);
    SDL_DestroyRenderer(inputRenderer);
    SDL_DestroyWindow(inputWindow);

    assignModifiedValue(infoName, userInput);
    m_entityChanged = true;
}

void Inspector::assignModifiedValue(const std::string& p_infoName, const std::string& p_value)
{
    if (p_infoName == "Entity's X position")
    {
        try
        {
            m_entityPtr->setPosition(std::stof(p_value), m_entityPtr->getPosition().y);
            m_selectionRect.x = HIERARCHY_WIDTH + std::stof(p_value);
        }
        catch (...)
        {
            std::cerr << "User didn't enter a floating value" << std::endl;
        }
    }
    else if (p_infoName == "Entity's Y position")
    {
        try
        {
            m_entityPtr->setPosition(m_entityPtr->getPosition().x, std::stof(p_value));
            m_selectionRect.y = std::stof(p_value);
        }
        catch (...)
        {
            std::cerr << "User didn't enter a floating value" << std::endl;
        }        
    }
    else if (p_infoName == "Entity's rotation")
    {
        try
        {
            m_entityPtr->setRotation(std::stof(p_value));
        }
        catch (...)
        {
            std::cerr << "User didn't enter a floating value" << std::endl;
        }   
    }
    else if (p_infoName == "Entity's X size")
    {
        try
        {
            m_entityPtr->setSize(std::stof(p_value), m_entityPtr->getSize().y);
            m_selectionRect.w = std::stof(p_value);
        }
        catch (...)
        {
            std::cerr << "User didn't enter a floating value" << std::endl;
        }   
    }
    else if (p_infoName == "Entity's Y size")
    {
        try
        {
            m_entityPtr->setSize(m_entityPtr->getSize().x, std::stof(p_value));
            m_selectionRect.h = std::stof(p_value);
        }
        catch (...)
        {
            std::cerr << "User didn't enter a floating value" << std::endl;
        }   
    }
    else if (p_infoName == "Collider's X position")
    {
        try
        {
            Collider* collider = m_entityPtr->getCollider();
            collider->setPosition(collider->getColliderRect().x, std::stof(p_value));
        }
        catch (...)
        {
            std::cerr << "User didn't enter a floating value" << std::endl;
        }   
    }
    else if (p_infoName == "Collider's Y position")
    {
        try
        {
            Collider* collider = m_entityPtr->getCollider();
            collider->setPosition(std::stof(p_value), collider->getColliderRect().y);
        }
        catch (...)
        {
            std::cerr << "User didn't enter a floating value" << std::endl;
        }   
    }
    else if (p_infoName == "Collider's X size")
    {
        try
        {
            Collider* collider = m_entityPtr->getCollider();
            collider->setDimensions(collider->getColliderRect().w, std::stof(p_value));
        }
        catch (...)
        {
            std::cerr << "User didn't enter a floating value" << std::endl;
        } 
    }
    else if (p_infoName == "Collider's Y size")
    {
        try
        {
            Collider* collider = m_entityPtr->getCollider();
            collider->setDimensions(std::stof(p_value), collider->getColliderRect().h);
        }
        catch (...)
        {
            std::cerr << "User didn't enter a floating value" << std::endl;
        } 
    }
    else if (p_infoName == "Is kinematic")
    {
        if (p_value == "true" || p_value == "1")
            m_entityPtr->setKinematic(true);
        else if (p_value == "false" || p_value == "0")
            m_entityPtr->setKinematic(false);
    }
    else if (p_infoName == "Entity's texture")
    {
        m_entityPtr->setTexture(p_value.c_str());
    }
    else if (p_infoName == "Entity's mass")
    {
        try
        {
            reinterpret_cast<MoveableEntity*>(m_entityPtr)->setMass(std::stof(p_value));
        }
        catch (...)
        {
            std::cerr << "User didn't enter a floating value" << std::endl;
        } 
    }
    else if (p_infoName == "Entity's viscosity")
    {
        try
        {
            reinterpret_cast<MoveableEntity*>(m_entityPtr)->setViscosity(std::stof(p_value));
        }
        catch (...)
        {
            std::cerr << "User didn't enter a floating value" << std::endl;
        } 
    }
    else if (p_infoName == "Gravity reactive")
    {
        if (p_value == "true" || p_value == "1")
            reinterpret_cast<MoveableEntity*>(m_entityPtr)->setGravityReactive(true);
        else if (p_value == "false" || p_value == "0")
            reinterpret_cast<MoveableEntity*>(m_entityPtr)->setGravityReactive(false);
    }
}
