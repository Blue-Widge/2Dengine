#pragma once
#include "Entity.h"
#include "GameStateButtons.h"
#include "SDLHandler.h"

class Gameloop;
class Inspector;

class InputManager
{
public:
    InputManager(bool* p_isPlaying, Inspector* p_inspector) : m_isPlaying(p_isPlaying), m_gameloop(nullptr),
                                                              m_inspector(p_inspector), m_gameStateButtons(nullptr),
                                                              m_entityChooser(nullptr)
    {
    }
    void checkInput();
    void sendControls();
    void setPlayerInstance(Player* p_player) { m_player = p_player; }
    void setGameloopObject(Gameloop* p_gameloop) { m_gameloop = p_gameloop; }
    void setGameStateButtonsObject(GameStateButtons* p_gameStateButtons) { m_gameStateButtons = p_gameStateButtons; }
    void setEntityChooser(EntityChooser* p_entityChooser) { m_entityChooser = p_entityChooser; }
    void setHierarchy(Hierarchy* p_hierarchy) { m_hierarchy = p_hierarchy; }
    void setEntityManager(EntityManager* p_entityManager) { m_entityManager = p_entityManager; }
private:
    SDL_Event m_event = {0};
    bool* m_isPlaying;
    Player* m_player = nullptr;
    bool m_controls[CONTROLS_NUMBER] = {false};
    Gameloop* m_gameloop;
    Inspector* m_inspector;
    Hierarchy* m_hierarchy;
    GameStateButtons* m_gameStateButtons;
    EntityChooser* m_entityChooser;
    EntityManager* m_entityManager;
};
