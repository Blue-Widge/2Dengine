#include "SDLHandler.h"
#include <iostream>

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
}

bool SDLHandler::initSDL()
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Couldn't init SDL" << std::endl;
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
    return true;
}

void SDLHandler::fixedUpdate()
{
    // PHYSICS THINGS
}

void SDLHandler::updateDeltaTime()
{
    const Uint32 loopEndTime = SDL_GetTicks();
    m_deltaTime = static_cast<float>(loopEndTime - m_loopBeginTime) / 1000.f;
    m_loopBeginTime = loopEndTime;
    m_fixedUpdateAccumulator += m_deltaTime;
}

void SDLHandler::checkInput()
{
    while(SDL_PollEvent(&m_event))
    {
        switch(m_event.type)
        {
        case SDL_QUIT:
            m_isPlaying = false;
            break;
        case SDL_KEYDOWN:
            break;
        case SDL_KEYUP:
            break;
            default: ;
        }
    }
}

void SDLHandler::draw() const
{
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_background, nullptr, nullptr);
//TODO : display each objects inside the scene
    SDL_RenderPresent(m_renderer);
}

void SDLHandler::loop()
{
    while(m_isPlaying)
    {
        updateDeltaTime();
        checkInput();
        draw();
        if (m_fixedUpdateAccumulator >= m_fixedUpdateThreshold)
        {
            fixedUpdate();
            m_fixedUpdateAccumulator -= m_fixedUpdateThreshold;
        }
        SDL_Delay(0);
    }
}
