#pragma once

#include <future>
#include <vector>

#include "SDL.h"
#include "Entity.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class InputManager
{
public:
    explicit InputManager(bool& p_isPlaying) : m_event({0}), m_isPlaying(p_isPlaying) { }
    void checkInput();
private:
    SDL_Event m_event;
    bool& m_isPlaying;
};

class EntityManager
{
public:
    EntityManager() : m_entities(0) { }
    void addEntity(Entity p_entity);

private:
    std::vector<Entity> m_entities;
};

class Gameloop
{
public:
    Gameloop() : m_deltaTime(0.f), m_loopBeginTime(0), m_fixedUpdateTime(10) { }
    void updateDeltaTime();
    void fixedUpdate();
private:
    float m_deltaTime;
    Uint32 m_loopBeginTime;
    std::chrono::milliseconds m_fixedUpdateTime;

};

class SDLHandler
{
public:
    ~SDLHandler();
    bool initSDL();
    void loop();
    void draw() const;
    static SDLHandler* getHandlerInstance();
    bool getIsPlaying() const { return m_isPlaying; }
private:
    SDLHandler() : m_window(nullptr), m_renderer(nullptr), m_background(nullptr), m_isPlaying(true), m_inputManager(m_isPlaying)
    {
    }

    static SDLHandler* instance;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_background;
    
    bool m_isPlaying;

    InputManager m_inputManager;
    EntityManager m_entityManager;
    Gameloop m_gameloop;
};
