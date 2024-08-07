﻿#pragma once

#include "utils.h"
#include <SDL.h>
#include <chrono>
#include <SDL_mixer.h>
#include <thread>
#include <vector>

class InputManager;
class EntityManager;
class Entity;
class MoveableEntity;
class Player;
class GameStateButtons;

class Gameloop
{
public:
    Gameloop(InputManager* p_inputManager, SDL_Renderer* p_renderer, SDL_Rect& p_sceneRect);
    Gameloop(InputManager* p_inputManager, SDL_Renderer* p_renderer, SDL_Rect& p_sceneRect, SDL_Texture* p_background);
    ~Gameloop();
    void updateDeltaTime();
    void update();
    void fixedUpdate();
    SDL_Rect convertEntityRectToScene(const FRect& p_rect) const;
    void draw() const;
    void playGame();
    void pauseGame();
    void stopGame();
    bool& getPlayingGame() { return m_playingGame; }
    Entity* getEntityFromPos(int p_x, int p_y) const;
    EntityManager* getEntityManager() const { return m_entityManager; }
    void checkCollectibles();
    void setCheckStateButtons(GameStateButtons* p_gameStateButtons) { m_gameStateButtons = p_gameStateButtons; }
private:
    SDL_Renderer* m_renderer;
    SDL_Texture* m_background;

    SDL_Rect& m_sceneRect;

    float m_deltaTime;
    Uint32 m_loopBeginTime;
    std::chrono::milliseconds m_fixedUpdateTime;
    std::thread m_fixedUpdateThread;
    bool m_playingGame;
    bool m_playingSDL;
    EntityManager* m_entityManager;
    InputManager* m_inputManager;
    GameStateButtons* m_gameStateButtons;

    Mix_Chunk* m_winSoundEffect = nullptr;
    void chargeMyLevel() const;

    const unsigned int m_processor_count = std::thread::hardware_concurrency();
    std::vector<std::thread> m_threads;
};
