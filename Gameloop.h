#pragma once

#include "utils.h"
#include <SDL.h>
#include <vector>
#include <chrono>
#include <thread>

class InputManager;
class EntityManager;
class Entity;
class MoveableEntity;
class Player;
class Gameloop
{
public:
    Gameloop(InputManager* p_inputManager, SDL_Renderer* p_renderer, const SDL_Rect& p_sceneRect);
    Gameloop(InputManager* p_inputManager, SDL_Renderer* p_renderer, const SDL_Rect& p_sceneRect, SDL_Texture* p_background);
    ~Gameloop();
    void updateDeltaTime();
    void update() const;
    void fixedUpdate() const;
    SDL_Rect convertEntityRectToScene(const FRect& p_rect) const;
    void draw() const;
    void playGame();
    void pauseGame();
    void stopGame();
    inline bool getPlayingGame() const { return m_playingGame; }
    Entity* getEntityFromPos(const int p_x, const int p_y) const;
private:
    SDL_Renderer* m_renderer;
    SDL_Texture* m_background;

    const SDL_Rect& m_sceneRect;
    
    float m_deltaTime;
    Uint32 m_loopBeginTime;
    std::chrono::milliseconds m_fixedUpdateTime;
    std::thread m_fixedUpdateThread;
    bool m_playingGame;
    bool m_playingSDL;
    EntityManager* m_entityManager;
    InputManager* m_inputManager;
};