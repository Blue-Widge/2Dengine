#include "SDLHandler.h"
#include <iostream>

#include "Entity.h"

SDLHandler* SDLHandler::instance = nullptr;

SDLHandler* SDLHandler::getHandlerInstance()
{
    if (instance == nullptr)
        instance = new SDLHandler();
    return instance;
}

SDLHandler::~SDLHandler()
{
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyTexture(m_background);
    TTF_CloseFont(m_font);
    TTF_Quit();
    m_window = nullptr;
    m_renderer = nullptr;
    m_background = nullptr;
}

bool SDLHandler::initSDL()
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Couldn't init SDL" << std::endl;
        return false;
    }

    if (TTF_Init())
    {
        std::cerr << "Couldn't init TTF" << std::endl;
        return false;
    }

    if (loadFont())
    {
        std::cerr << "Couldn't load font" << std::endl;
        return false;
    }
    
    m_window = SDL_CreateWindow("2D Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
        SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!m_window)
    {
        std::cerr << "Couldn't load window" << std::endl;
        return false;
    }
    
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
    if (!m_renderer)
    {
        std::cerr << "Couldn't load renderer " << std::endl;
        return false;
    }

    SDL_Surface* backgroundBMP = SDL_LoadBMP("./images/background.bmp");
    if (!backgroundBMP)
    {
        std::cerr << "Couldn't load background image" << std::endl;
        return false;
    }

    m_background = SDL_CreateTextureFromSurface(m_renderer, backgroundBMP);
    if (!m_background)
    {
        std::cerr << "Couldn't create background texture" << std::endl;
    }
    const int inspectorXpos = m_sceneRect.x + m_sceneRect.w;
    m_inspector = new Inspector(m_renderer, m_font);
    
    m_inputManager = new InputManager(&m_isPlaying, m_inspector);
    m_gameloop = new Gameloop(m_inputManager, m_renderer, m_sceneRect, m_background);
    m_inputManager->setGameloopObject(m_gameloop);
    return true;
}

bool SDLHandler::loadFont()
{
    m_font = TTF_OpenFont("./Font/segoeui.ttf", 20);
    return (m_font == nullptr);
}

void SDLHandler::loop() const
{
    while (m_isPlaying)
    {
        m_inputManager->checkInput();
        m_inputManager->sendControls();
        m_gameloop->updateDeltaTime();
        m_gameloop->update();
        m_gameloop->draw();
        m_inspector->displayInspector();
        SDL_RenderPresent(m_renderer);
        SDL_Delay(0);
    }
    SDL_Quit();
}
