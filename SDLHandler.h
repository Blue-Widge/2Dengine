#pragma once
#include "EntityChooser.h"
#include "SDL.h"
#include "utils.h"
#include "Inspector.h"
#include "InputManager.h"
#include "Gameloop.h"
#include "Hierarchy.h"

class SDLHandler
{
public:
    ~SDLHandler();
    bool initSDL();
    bool loadFont();
    void loop() const;
    static SDLHandler* getHandlerInstance();
    bool getIsActivated() const { return m_isActivated; }
private:
    SDLHandler() : m_window(nullptr), m_renderer(nullptr), m_background(nullptr), m_isActivated(true),
        m_inputManager(nullptr), m_gameloop(nullptr), m_inspector(nullptr), m_hierarchy(nullptr),
        m_gameStateButtons(nullptr), m_font(nullptr)
    {
    }

    static SDLHandler* instance;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_background;
    
    bool m_isActivated;

    InputManager* m_inputManager;
    Gameloop* m_gameloop;
    Inspector* m_inspector;
    Hierarchy* m_hierarchy;
    GameStateButtons* m_gameStateButtons;
    const SDL_Rect m_sceneRect = {(SCREEN_WIDTH - SCENE_WIDTH) / 2, 0, SCENE_WIDTH, SCENE_HEIGHT};
    TTF_Font* m_font;
    EntityChooser* m_entityChooser;
};
