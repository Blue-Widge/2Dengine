#pragma once
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

#include "GameStateButtons.h"

class Entity;

class Inspector
{
public:
    Inspector(SDL_Renderer* p_renderer, TTF_Font* p_font);
    void displayInspector();
    bool selectEntity(Entity* p_entity);
    void displayEntityInfos(const std::string& p_string, int p_entityId);
    
    void modifyInfoValue(int p_x, int p_y);
    void assignModifiedValue(const std::string& p_infoName, const std::string& p_value);
    inline void setCurrentText(const std::string& p_text) { m_currentText = p_text; }
private:
    struct EntityInfo
    {
        EntityInfo(SDL_Texture* p_textTexture, SDL_Rect p_textRect, std::string p_infoName) :
            m_textTexture(p_textTexture), m_textRect(p_textRect), m_infoName(std::move(p_infoName)) {}
        SDL_Texture* m_textTexture;
        SDL_Rect m_textRect;
        std::string m_infoName;
    };
    
    SDL_Renderer* m_renderer;
    SDL_Rect m_rect;
    
    SDL_Texture* m_texture;
    SDL_Texture* m_titleTexture;
    SDL_Rect m_titleRect;
    
    TTF_Font* m_font;
    SDL_Color m_fontColor = {200, 200, 200, 255};
    SDL_Rect m_selectionRect;
    SDL_Texture* m_selectionTexture;
    
    Entity* m_entityPtr = nullptr;
    std::string m_currentText;
    
    std::vector<Inspector::EntityInfo> m_entityInfos;
    Entity* m_lastEntityPtr = nullptr;
    bool m_entityChanged;
};
