#pragma once
#include <SDL_render.h>
#include <SDL_image.h>

#include "Gameloop.h"
#include "utils.h"

//TODO : implement start/pause/stop buttons
class GameStateButtons
{
public:
    GameStateButtons(SDL_Renderer* p_renderer, Gameloop* p_gameloop);
    ~GameStateButtons();
    void displayGameStateButtons() const;
    void detectPressedButtons(int p_x, int p_y) const;
private:
    SDL_Renderer* m_renderer;
    Gameloop* m_gameLoop;
    
    SDL_Texture* m_playTexture;
    SDL_Texture* m_pauseTexture;
    SDL_Texture* m_stopTexture;
    SDL_Rect m_playRect;
    SDL_Rect m_pauseRect;
    SDL_Rect m_stopRect;
};
