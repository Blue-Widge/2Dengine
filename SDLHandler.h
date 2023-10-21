#pragma once

#include <future>
#include <vector>

#include "Inspector.h"
#include "SDL.h"
#include "utils.h"


class Gameloop;
class Entity;
class MoveableEntity;
class Player;

class InputManager
{
public:
    InputManager(bool* p_isPlaying, Inspector* p_inspector) : m_isPlaying(p_isPlaying), m_gameloop(nullptr), m_inspector(p_inspector) { }
    void checkInput();
    void sendControls() const;
    inline void setPlayerInstance(Player* p_player) { m_player = p_player; }
    void setGameloopObject(Gameloop* p_gameloop) { m_gameloop = p_gameloop; }

private:
    SDL_Event m_event = {0};
    bool* m_isPlaying;
    Player* m_player = nullptr;
    bool m_controls[controlsNb] = {false};
    Gameloop* m_gameloop;
    Inspector* m_inspector;
};

class EntityManager
{
public:
    EntityManager(SDL_Renderer* p_renderer) : m_renderer(p_renderer), m_nbEntities(0), m_entities(0),
                                              m_moveableEntities(0), m_player(nullptr)
    {
    }

    ~EntityManager();
    Entity* addEntity();
    Entity* addEntity(const char* p_texturePath);
    Entity* addEntity(const char* p_texturePath, const FRect& p_rect);
    MoveableEntity* addMoveableEntity(const char* p_texturePath, const FRect& p_rect, float p_mass);
    Player* addPlayer(const char* p_texturePath, const FRect& p_rect, float p_mass);
    Vec2<float> getEntityAppliedVelocity(const MoveableEntity& p_moveableEntity, const float p_deltaTime) const;
    inline std::vector<MoveableEntity*> getMoveableEntities() { return m_moveableEntities; }
    void resetEntities() const;
    void deleteEntities() const;
    inline std::vector<Entity*> getEntities() const { return m_entities; }
    inline Player* getPlayer() const { return m_player; }
    inline std::mutex& getEntitiesMutex() { return m_entitiesMutex; }
private:
    SDL_Renderer* m_renderer;
    Uint16 m_nbEntities;
    std::vector<Entity*> m_entities;
    std::vector<MoveableEntity*> m_moveableEntities;
    Player* m_player;
    std::mutex m_entitiesMutex;
};

class Gameloop
{
public:
    Gameloop(InputManager* p_inputManager, SDL_Renderer* p_renderer, const SDL_Rect& p_sceneRect);
    Gameloop(InputManager* p_inputManager, SDL_Renderer* p_renderer, const SDL_Rect& p_sceneRect, SDL_Texture* p_background);
    ~Gameloop();
    void updateDeltaTime();
    void update() const;
    void fixedUpdate();
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
    EntityManager* m_entityManager;
    InputManager* m_inputManager;
};

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
