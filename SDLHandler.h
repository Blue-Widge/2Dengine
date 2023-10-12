#pragma once

#include <future>
#include <iostream>
#include <ostream>
#include <vector>

#include "SDL.h"
#include "utils.h"

enum
{
    SCREEN_WIDTH = 800,
    SCREEN_HEIGHT = 600,
    FIXED_UPDATE_TIME = 10 // ms
};

class Entity;
class MoveableEntity;

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
    EntityManager(SDL_Renderer* p_renderer) : m_renderer(p_renderer), m_nbEntities(0), m_entities(0),
                                              m_moveableEntities(0)
    {
    }
    ~EntityManager();
    void addEntity();
    void addEntity(const char* p_texturePath);
    void addEntity(const char* p_texturePath, const SDL_Rect& p_rect);
    void addMoveableEntity(const char* p_texturePath, const SDL_Rect& p_rect, float p_mass);
    Vec2<float> getEntityAppliedVelocity(const MoveableEntity& p_moveableEntity) const;
    inline std::vector<MoveableEntity*> getMoveableEntities() { return m_moveableEntities; }
    void resetEntities() const;
    void deleteEntities() const;
    inline std::vector<Entity*> getEntities() const { return m_entities; }

private:
    SDL_Renderer* m_renderer;
    Uint16 m_nbEntities;
    std::vector<Entity*> m_entities;
    std::vector<MoveableEntity*> m_moveableEntities;
};

class Gameloop
{
public:
    Gameloop(SDL_Renderer* p_renderer);
    Gameloop(SDL_Renderer* p_renderer, SDL_Texture* p_background);
    ~Gameloop();
    void updateDeltaTime();
    void fixedUpdate();
    void draw() const;
    void playGame();
    void pauseGame();
    void stopGame();
private:
    SDL_Renderer* m_renderer;
    SDL_Texture* m_background;
    
    float m_deltaTime;
    Uint32 m_loopBeginTime;
    std::chrono::milliseconds m_fixedUpdateTime;
    std::thread m_fixedUpdateThread;
    bool m_playingThread;
    EntityManager* m_entityManager;
};

class SDLHandler
{
public:
    ~SDLHandler();
    bool initSDL();
    void loop();
    static SDLHandler* getHandlerInstance();
    bool getIsPlaying() const { return m_isPlaying; }
private:
    SDLHandler() : m_window(nullptr), m_renderer(nullptr), m_background(nullptr), m_isPlaying(true),
                   m_inputManager(nullptr), m_gameloop(nullptr)
    {
    }

    static SDLHandler* instance;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_background;
    
    bool m_isPlaying;

    InputManager* m_inputManager;
    Gameloop* m_gameloop;
};
