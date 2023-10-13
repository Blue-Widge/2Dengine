#pragma once
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>

class Player;
class MoveableEntity;
class Entity;

class Inspector
{
public:
    Inspector(int p_posX, int p_width, SDL_Renderer* p_renderer, TTF_Font* p_font);
    void displayInspector() const;
    void selectEntity(Entity* p_entity);
    void selectEntity(MoveableEntity* p_entity);
    void selectEntity(Player* p_entity);
    void modifyInfoValue(int p_x, int p_y) const;
    inline void setCurrentText(const std::string& p_text) { m_currentText = p_text; }
private:
    SDL_Renderer* m_renderer;
    SDL_Rect m_rect;
    Entity* m_entityPtr = nullptr;
    SDL_Texture* m_texture;
    SDL_Texture* m_titleTexture;
    SDL_Rect m_titleRect;
    TTF_Font* m_font;
    SDL_Color m_fontColor = {200, 200, 200, 255};
    SDL_Rect m_selectionRect;
    SDL_Texture* m_selectionTexture;
    std::string m_currentText;
};
