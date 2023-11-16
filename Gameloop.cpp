#include "Gameloop.h"
#include "EntityManager.h"
#include "InputManager.h"

Gameloop::Gameloop(InputManager* p_inputManager, SDL_Renderer* p_renderer, const SDL_Rect& p_sceneRect) : Gameloop(p_inputManager, p_renderer,
    p_sceneRect, nullptr)
{
}

Gameloop::Gameloop(InputManager* p_inputManager, SDL_Renderer* p_renderer, const SDL_Rect& p_sceneRect,
                   SDL_Texture* p_background) : m_renderer(p_renderer), m_background(p_background),
                                                m_sceneRect(p_sceneRect), m_deltaTime(0.f),
                                                m_loopBeginTime(0),
                                                m_fixedUpdateTime(FIXED_UPDATE_TIME),
                                                m_playingGame(false),
                                                m_playingSDL(true),
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
}

Gameloop::~Gameloop()
{
    m_playingSDL = m_playingGame = false;
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

void Gameloop::fixedUpdate() const
{
    while (m_playingSDL)
    {
        if (!m_playingGame)
            return;
        auto startTime = std::chrono::steady_clock::now();
        auto entities = m_entityManager->getMoveableEntities();

        //TODO: optimisation effectuate for loop on multiples threads
        
        for (const auto entity : entities)
        {
            entity->applyForces(m_fixedUpdateTime);
        }
        auto endTime  = std::chrono::steady_clock::now();
        auto sleepTime = endTime - startTime;
        if (sleepTime > std::chrono::steady_clock::duration::zero())
            std::this_thread::sleep_until(startTime + m_fixedUpdateTime);
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

void Gameloop::draw() const
{
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_background, nullptr, &m_sceneRect);
    const std::vector<Entity*> entities = m_entityManager->getEntities();
    for (const auto entity : entities)
    {
        const FRect entityRect = entity->getEntityRect();
        const SDL_Rect convertedRect = convertEntityRectToScene(entityRect);
        SDL_RenderCopy(m_renderer, entity->getTexture(), nullptr, &convertedRect);
    }
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
    m_entityManager->resetEntities();
}

Entity* Gameloop::getEntityFromPos(int p_x, int p_y) const
{
    p_x -= (SCREEN_WIDTH - SCENE_WIDTH) / 2;
    const auto entities = m_entityManager->getEntities();
    for (Entity* entity : entities)
    {
        const auto entityRect = entity->getEntityRect();
        if (p_x >= entityRect.x && p_x <= entityRect.x + entityRect.w &&
            p_y >= entityRect.y && p_y <= entityRect.y + entityRect.h)
                return entity;
    }
    return nullptr;
}