#pragma once
#include "SDL.h"
#include "utils.h"
#include "Inspector.h"
#include "InputManager.h"
#include "Gameloop.h"

class SDLHandler
{
public:
    ~SDLHandler();
    bool initSDL();
    bool loadFont();
    void loop() const;
    static SDLHandler* getHandlerInstance();
    bool getIsPlaying() const { return m_isPlaying; }
private:
    SDLHandler() : m_window(nullptr), m_renderer(nullptr), m_background(nullptr),
        m_isPlaying(true), m_inputManager(nullptr), m_gameloop(nullptr), m_inspector(nullptr), m_font(nullptr)
    {
    }

    static SDLHandler* instance;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_background;
    
    bool m_isPlaying;

    InputManager* m_inputManager;
    Gameloop* m_gameloop;
    Inspector* m_inspector;
    const SDL_Rect m_sceneRect = {(SCREEN_WIDTH - SCENE_WIDTH) / 2, 0, SCENE_WIDTH, SCENE_HEIGHT};
    TTF_Font* m_font;
};
