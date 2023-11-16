#pragma once
#include "Entity.h"

class Gameloop;
class Inspector;

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

