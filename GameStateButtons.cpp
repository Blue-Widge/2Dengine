#include "GameStateButtons.h"

#include "Gameloop.h"


GameStateButtons::GameStateButtons(SDL_Renderer* p_renderer, Gameloop* p_gameloop) : m_renderer(p_renderer),
                                                                                     m_gameLoop(p_gameloop)
{
    m_playRect = {HIERARCHY_WIDTH + SCENE_WIDTH - GAMESTATEBUTTONS_WIDTH, SCENE_HEIGHT,
        GAMESTATEBUTTONS_WIDTH / 3, GAMESTATEBUTTONS_HEIGHT};
    m_pauseRect = {HIERARCHY_WIDTH + SCENE_WIDTH - 4 * GAMESTATEBUTTONS_WIDTH / 6, SCENE_HEIGHT,
        GAMESTATEBUTTONS_WIDTH / 3, GAMESTATEBUTTONS_HEIGHT};
    m_stopRect = {HIERARCHY_WIDTH + SCENE_WIDTH - 2 * GAMESTATEBUTTONS_WIDTH / 6, SCENE_HEIGHT,
        GAMESTATEBUTTONS_WIDTH / 3, GAMESTATEBUTTONS_HEIGHT};

    
    SDL_Surface* playSurface = IMG_Load("./images/playButton.png");
    SDL_Surface* pauseSurface = IMG_Load("./images/pauseButton.png");
    SDL_Surface* stopSurface = IMG_Load("./images/stopButton.png");

    m_playTexture = SDL_CreateTextureFromSurface(m_renderer, playSurface);
    m_pauseTexture = SDL_CreateTextureFromSurface(m_renderer, pauseSurface);
    m_stopTexture = SDL_CreateTextureFromSurface(m_renderer, stopSurface);
    
    SDL_FreeSurface(playSurface);
    SDL_FreeSurface(pauseSurface);
    SDL_FreeSurface(stopSurface);
}

GameStateButtons::~GameStateButtons()
{
    SDL_DestroyTexture(m_playTexture);
    SDL_DestroyTexture(m_pauseTexture);
    SDL_DestroyTexture(m_stopTexture);
}

void GameStateButtons::displayGameStateButtons() const
{
    SDL_RenderCopy(m_renderer, m_playTexture, nullptr, &m_playRect);
    SDL_RenderCopy(m_renderer, m_pauseTexture, nullptr, &m_pauseRect);
    SDL_RenderCopy(m_renderer, m_stopTexture, nullptr, &m_stopRect);
}

bool GameStateButtons::detectPressedButtons(const int p_x, const int p_y) const
{
    if (detectButtonClicked(p_x, p_y, m_playRect))
    {
        m_gameLoop->playGame();
        return true;
    }

    if (detectButtonClicked(p_x, p_y, m_pauseRect))
    {
        m_gameLoop->pauseGame();
        return true;
    }

    if (detectButtonClicked(p_x, p_y, m_stopRect))
    {
        m_gameLoop->stopGame();
        return true;
    }
    return false;
}
