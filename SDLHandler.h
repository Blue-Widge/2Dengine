#pragma once

#include "SDL.h"
#include "Entity.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
 // TODO : Change the single class to multiple ones : GameLoop, EntityManager, InputManager at least


class SDLHandler
{
public:
    ~SDLHandler();
    bool initSDL();
    void fixedUpdate();
    void loop();
    void updateDeltaTime();
    void checkInput();
    void draw() const;
    static SDLHandler* getHandlerInstance();
    bool getIsPlaying() const { return m_isPlaying; }
private:
    SDLHandler() : m_window(nullptr), m_renderer(nullptr), m_background(nullptr), m_isPlaying(true),
                   m_deltaTime(0.f), m_loopBeginTime(0), m_fixedUpdateAccumulator(0.f), m_fixedUpdateThreshold(0.01f)
    {
    }

    static SDLHandler* instance;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_background;
    SDL_Event m_event = {0};

    bool m_isPlaying;
    float m_deltaTime;
    Uint32 m_loopBeginTime;
    float m_fixedUpdateAccumulator, m_fixedUpdateThreshold;
};
