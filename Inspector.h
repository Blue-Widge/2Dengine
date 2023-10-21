#pragma once
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

class Player;
class MoveableEntity;
class Entity;

class Inspector
{
public:
    Inspector(SDL_Renderer* p_renderer, TTF_Font* p_font);
    void displayInspector();
    bool selectEntity(Entity* p_entity);
    void displayEntityInfos(const std::string& p_string, int p_entityId);
    
    void modifyInfoValue(int p_x, int p_y) const;
    inline void setCurrentText(const std::string& p_text) { m_currentText = p_text; }
private:
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
    
    std::vector<std::tuple<SDL_Texture*, SDL_Rect>> m_infosTexturesAndRect;
};
