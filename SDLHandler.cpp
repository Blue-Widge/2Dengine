#include "SDLHandler.h"
#include <iostream>

#include "Entity.h"


void InputManager::checkInput()
{
    while(SDL_PollEvent(&m_event))
    {
        switch(m_event.type)
        {
        case SDL_QUIT:
            m_isPlaying = false;
            break;
        case SDL_KEYDOWN:
            switch (m_event.key.keysym.sym)
            {
                case SDLK_ESCAPE :
                    m_isPlaying = false;
                    break;
                default: ;
            }
            break;
        case SDL_KEYUP:
            break;
        default: ;
        }
    }
}

EntityManager::~EntityManager()
{
    for(auto entity: m_entities)
    {
        delete entity;
        entity = nullptr;
    }
}

void EntityManager::addEntity()
{
    EntityManager::addEntity(BASE_TEXTURE);
}

void EntityManager::addEntity(const char* p_texturePath)
{
    EntityManager::addEntity(p_texturePath, {0, 0, 50, 50});
}

void EntityManager::addEntity(const char* p_texturePath, const SDL_Rect& p_rect)
{
    auto* entity = new Entity(this, m_nbEntities, m_renderer, p_texturePath, p_rect);
    ++m_nbEntities;
    m_entities.push_back(entity);
}

void EntityManager::addMoveableEntity(const char* p_texturePath, const SDL_Rect& p_rect, const float p_mass)
{
    auto* entity = new MoveableEntity(this, m_nbEntities, m_renderer, p_texturePath, p_rect, p_mass);
    ++m_nbEntities;
    m_entities.push_back(entity);   
    m_moveableEntities.push_back(entity);
}


Vec2<float> EntityManager::getEntityAppliedVelocity(const MoveableEntity& p_moveableEntity) const
{
    Vec2<float> appliedVelocity = {0.f, 0.f};
    if (p_moveableEntity.m_isKinematic)
        return appliedVelocity;
    for(const auto entity : m_entities)
    {
        if (*entity == p_moveableEntity)
            continue;
        const Vec2<float> currVelocity = p_moveableEntity.getCollider()->checkCollision(p_moveableEntity, *entity);
        appliedVelocity.x += currVelocity.x;
        appliedVelocity.y += currVelocity.y;
    }
    return appliedVelocity;
}

void EntityManager::resetEntities() const
{
    for(const auto entity : m_moveableEntities)
    {
        entity->resetEntity();
    }
}

void EntityManager::deleteEntities() const
{
    for(auto entity : m_entities)
    {
        delete entity;
        entity = nullptr;
    }
}

Gameloop::Gameloop(SDL_Renderer* p_renderer) : Gameloop(p_renderer, nullptr)
{
    
}

Gameloop::Gameloop(SDL_Renderer* p_renderer, SDL_Texture* p_background) : m_renderer(p_renderer),
                                                                          m_background(p_background), m_deltaTime(0.f),
                                                                          m_loopBeginTime(0),
                                                                           m_fixedUpdateTime(FIXED_UPDATE_TIME),
                                                                          m_playingThread(true)
{
    m_entityManager = new EntityManager(m_renderer);
    m_fixedUpdateThread = std::thread([this]() { fixedUpdate(); });
    m_entityManager->addMoveableEntity(BASE_TEXTURE, {10, 10, 50, 50}, 10.f);
    m_entityManager->addEntity(BASE_TEXTURE, {100, 100, 50, 50});
    m_entityManager->addEntity(BASE_TEXTURE, {10, 300, 100, 10});
}

Gameloop::~Gameloop()
{
    m_playingThread = false;
    m_fixedUpdateThread.join();
    delete m_entityManager;
}

void Gameloop::updateDeltaTime()
{
    const Uint32 loopEndTime = SDL_GetTicks();
    m_deltaTime = static_cast<float>(loopEndTime - m_loopBeginTime) / 1000.f;
    m_loopBeginTime = loopEndTime;
}

void Gameloop::fixedUpdate()
{
    while(m_playingThread)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_fixedUpdateTime));
        
        auto entities = m_entityManager->getMoveableEntities();
        for(const auto entity : entities)
        {
            entity->applyForces(m_fixedUpdateTime);
        }

    }
}

void Gameloop::playGame()
{
    m_playingThread = true;
}

void Gameloop::pauseGame()
{
    m_playingThread = false;
}

void Gameloop::stopGame()
{
    m_playingThread = false;
    m_fixedUpdateTime = std::chrono::milliseconds::zero();
    m_entityManager->resetEntities();
}

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
    m_inputManager = new InputManager(m_isPlaying);
    m_gameloop = new Gameloop(m_renderer, m_background);
    
    return true;
}

void Gameloop::draw() const
{
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_background, nullptr, nullptr);
    std::vector<Entity*> entities = m_entityManager->getEntities();
    for(auto entity : entities)
    {
        SDL_Rect entityRect = entity->getEntityRect();
        SDL_RenderCopy(m_renderer, entity->getTexture(), nullptr, &entityRect);
    }
    SDL_RenderPresent(m_renderer);
}

void SDLHandler::loop()
{
    while(m_isPlaying)
    {
        m_inputManager->checkInput();
        m_gameloop->updateDeltaTime();
        m_gameloop->draw();
        SDL_Delay(0);
    }
    SDL_Quit();
}
