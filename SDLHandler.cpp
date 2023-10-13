#include "SDLHandler.h"
#include <iostream>

#include "Entity.h"


void InputManager::checkInput()
{
    while (SDL_PollEvent(&m_event))
    {
        switch (m_event.type)
        {
        case SDL_QUIT:
            *m_isPlaying = false;
            break;
        case SDL_KEYDOWN:
            switch (m_event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    *m_isPlaying = false;
                    break;
                case SDLK_z:
                case SDLK_UP:
                    m_controls[UP] = true;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    m_controls[RIGHT] = true;
                    break;
                case SDLK_q:
                case SDLK_LEFT:
                    m_controls[LEFT] = true;
                    break;
                case SDLK_e:
                case SDLK_RETURN:
                    m_controls[USE] = true;
                break;
                default: ;
            }
            break;
        case SDL_KEYUP:
            switch (m_event.key.keysym.sym)
            {
                case SDLK_z:
                case SDLK_UP:
                    m_controls[UP] = false;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    m_controls[RIGHT] = false;
                    break;
                case SDLK_q:
                case SDLK_LEFT:
                    m_controls[LEFT] = false;
                    break;
                case SDLK_e:
                case SDLK_RETURN:
                    m_controls[USE] = false;
                break;
                default: ;
            }
        }
    }
}

void InputManager::sendControls() const
{
    if (!m_player)
        return;

    if (m_controls[UP])
        if (m_player->getOnGround())
            m_player->setYVelocity(-1500.f);
    if (m_controls[RIGHT])
        m_player->setXVelocity(50.f);
    if (m_controls[LEFT])
        m_player->setXVelocity(-50.f);
    if ((m_controls[LEFT] ^ m_controls[RIGHT]) == false)
        m_player->setXVelocity(0.f);
}

EntityManager::~EntityManager()
{
    for (auto entity : m_entities)
    {
        delete entity;
        entity = nullptr;
    }
}

Entity* EntityManager::addEntity()
{
    return addEntity(BASE_TEXTURE);
}

Entity* EntityManager::addEntity(const char* p_texturePath)
{
    return addEntity(p_texturePath, {0, 0, 50, 50});
}

Entity* EntityManager::addEntity(const char* p_texturePath, const FRect& p_rect)
{
    auto* entity = new Entity(this, m_nbEntities, m_renderer, p_texturePath, p_rect);
    ++m_nbEntities;
    m_entities.push_back(entity);

    return entity;
}

MoveableEntity* EntityManager::addMoveableEntity(const char* p_texturePath, const FRect& p_rect, const float p_mass)
{
    auto* entity = new MoveableEntity(this, m_nbEntities, m_renderer, p_texturePath, p_rect, p_mass);
    ++m_nbEntities;
    m_entities.push_back(entity);
    m_moveableEntities.push_back(entity);

    return entity;
}

Player* EntityManager::addPlayer(const char* p_texturePath, const FRect& p_rect, const float p_mass)
{
    auto* entity = Player::getPlayerInstance(this, m_nbEntities, m_renderer, p_texturePath, p_rect,
        p_mass, 0.81f);
    ++m_nbEntities;
    m_entities.push_back(entity);
    m_player = entity;
    return entity;
}


Vec2<float> EntityManager::getEntityAppliedVelocity(const MoveableEntity& p_moveableEntity,
                                                    const float p_deltaTime) const
{
    Vec2<float> appliedVelocity = {0.f, 0.f};
    if (p_moveableEntity.getIsKinematic())
        return appliedVelocity;
    for (const auto entity : m_entities)
    {
        if (*entity == p_moveableEntity)
            continue;
        const Vec2<float> currVelocity = p_moveableEntity.getCollider()->checkCollisions(*entity, p_deltaTime);
        appliedVelocity.x += currVelocity.x;
        appliedVelocity.y += currVelocity.y;
    }
    return appliedVelocity;
}

void EntityManager::resetEntities() const
{
    for (const auto entity : m_moveableEntities)
    {
        entity->resetEntity();
    }
}

void EntityManager::deleteEntities() const
{
    for (auto entity : m_entities)
    {
        delete entity;
        entity = nullptr;
    }
}

Gameloop::Gameloop(InputManager* p_inputManager, SDL_Renderer* p_renderer) : Gameloop(p_inputManager, p_renderer,
    nullptr)
{
}

Gameloop::Gameloop(InputManager* p_inputManager, SDL_Renderer* p_renderer,
                   SDL_Texture* p_background) : m_renderer(p_renderer), m_background(p_background),
                                                m_deltaTime(0.f), m_loopBeginTime(0),
                                                m_fixedUpdateTime(FIXED_UPDATE_TIME),
                                                m_playingGame(true),
                                                m_inputManager(p_inputManager)
{
    m_entityManager = new EntityManager(m_renderer);
    m_fixedUpdateThread = std::thread([this]() { fixedUpdate(); });
    auto test = m_entityManager->addMoveableEntity(BASE_TEXTURE, {10, 10, 50, 50}, 10.f);
    //auto test2 = m_entityManager->addMoveableEntity(BASE_TEXTURE, {10, 100, 50, 50}, 10.f);
    m_entityManager->addEntity(BASE_TEXTURE, {400, 500, 50, 50});
    m_entityManager->addEntity(BASE_TEXTURE, {10, 500, 100, 10});
    auto player = m_entityManager->addPlayer(PLAYER_BASE_TEXTURE, {15, 100, 50, 100}, 10.f);
    m_inputManager->setPlayerInstance(player);
    //test->setKinematic(true);
    //test2->setKinematic(true);
}

Gameloop::~Gameloop()
{
    m_playingGame = false;
    m_fixedUpdateThread.join();
    delete m_entityManager;
}

void Gameloop::updateDeltaTime()
{
    const Uint32 loopEndTime = SDL_GetTicks();
    m_deltaTime = static_cast<float>(loopEndTime - m_loopBeginTime) / 1000.f;
    m_loopBeginTime = loopEndTime;
}

void Gameloop::update() const
{
    if (!m_playingGame)
        return;
    const auto player = m_entityManager->getPlayer();
    const auto moveableEntities = m_entityManager->getMoveableEntities();
    player->applyGravity(m_deltaTime);
    player->applyMovements(m_deltaTime);
    for (const auto moveableEntity : moveableEntities)
    {
        moveableEntity->applyGravity(m_deltaTime);
    }
}

void Gameloop::fixedUpdate()
{
    while (m_playingGame)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_fixedUpdateTime));
        auto entities = m_entityManager->getMoveableEntities();
        for (const auto entity : entities)
        {
            entity->applyForces(m_fixedUpdateTime);
        }
    }
}

SDL_Rect Gameloop::convertEntityRectToScene(const FRect& p_rect) const
{
    const SDL_Rect rect = { static_cast<int>(p_rect.x) + m_sceneRect.x,
        static_cast<int>(p_rect.y),
        static_cast<int>(p_rect.w),
        static_cast<int>(p_rect.h)
    };
    return rect;
}

void Gameloop::playGame()
{
    m_playingGame = true;
}

void Gameloop::pauseGame()
{
    m_playingGame = false;
}

void Gameloop::stopGame()
{
    m_playingGame = false;
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
    m_inputManager = new InputManager(&m_isPlaying);
    m_gameloop = new Gameloop(m_inputManager, m_renderer, m_background);

    return true;
}

void Gameloop::draw() const
{
    SDL_RenderClear(m_renderer);SDL_RenderCopy(m_renderer, m_background, nullptr, &m_sceneRect);
    const std::vector<Entity*> entities = m_entityManager->getEntities();
    for (const auto entity : entities)
    {
        const FRect entityRect = entity->getEntityRect();
        const SDL_Rect convertedRect = convertEntityRectToScene(entityRect);
        SDL_RenderCopy(m_renderer, entity->getTexture(), nullptr, &convertedRect);
    }
    SDL_RenderPresent(m_renderer);
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
        SDL_Delay(0);
    }
    SDL_Quit();
}
