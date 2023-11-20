#include "Gameloop.h"

#include <algorithm>

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
    
    chargeMyLevel();
    m_winSoundEffect = Mix_LoadWAV("./sounds/victory.mp3");
}

Gameloop::~Gameloop()
{
    m_playingSDL = m_playingGame = false;
    m_fixedUpdateThread.join();
    delete m_entityManager;
    Mix_FreeChunk(m_winSoundEffect);
}

void Gameloop::updateDeltaTime()
{
    const Uint32 loopEndTime = SDL_GetTicks();
    m_deltaTime = static_cast<float>(loopEndTime - m_loopBeginTime) / 1000.f;
    m_loopBeginTime = loopEndTime;
}

void Gameloop::update()
{
    if (!m_playingGame)
        return;
    const auto player = m_entityManager->getPlayer();
    player->applyMovements(m_deltaTime);
    const auto collectibles = m_entityManager->getCollectibles();
    for(auto* collectible : collectibles)
    {
        collectible->detectCollected(player->getCollider()->getColliderRect());
    }
    checkCollectibles();
}

void Gameloop::fixedUpdate() const
{
    while (m_playingSDL)
    {
        if (!m_playingGame)
            continue;
        auto startTime = std::chrono::steady_clock::now();
        auto moveableEntities = m_entityManager->getMoveableEntities();

        //TODO: optimisation effectuate for loop on multiples threads
        const float fixedUpdateTime = m_fixedUpdateTime.count() / 1000.f;
        for (const auto entity : moveableEntities)
        {
            entity->applyForces(fixedUpdateTime);
            entity->applyGravity(fixedUpdateTime);
        }
        m_entityManager->solveInsidersEntities(fixedUpdateTime);
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
    m_inputManager->setPlayerInstance(m_entityManager->getPlayer());
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

Entity* Gameloop::getEntityFromPos(int p_x, const int p_y) const
{
    p_x -= (SCREEN_WIDTH - SCENE_WIDTH) / 2;
    const auto entities = m_entityManager->getEntities();
    for (Entity* entity : entities)
    {
        const auto entityRect = entity->getEntityRect();
        if (detectButtonClicked(p_x, p_y, convertFRect(entityRect)))
            return entity;
    }
    return nullptr;
}

void Gameloop::checkCollectibles()
{
    std::vector<Collectible*> collectibles = m_entityManager->getCollectibles();
    if (!std::all_of(collectibles.cbegin(), collectibles.cend(),
        [](const Collectible* p_collectible) { return p_collectible->getIsCollected();}))
            return;
    stopGame();
    //WIN
    Mix_PlayChannel(2, m_winSoundEffect, 0);
}


void Gameloop::chargeMyLevel() const
{
    m_entityManager->addPlayer(BASE_PLAYER_TEXTURE, {50, 30, 20, 40}, 80);
    m_entityManager->addEntity(BASE_TEXTURE, {10, 75, 100, 10});
    m_entityManager->addEntity(BASE_TEXTURE, {10, 150, 100, 10});
    m_entityManager->addCollectible(BASE_COLLECTIBLE_TEXTURE, {50, 130, 20, 20});
    
    m_entityManager->addEntity(BASE_TEXTURE, {10, 225, 100, 10});
    m_entityManager->addCollectible(BASE_COLLECTIBLE_TEXTURE, {50, 205, 20, 20});
    m_entityManager->addEntity(BASE_TEXTURE, {10, 300, 100, 10});
    m_entityManager->addCollectible(BASE_COLLECTIBLE_TEXTURE, {50, 280, 20, 20});
    m_entityManager->addEntity(BASE_TEXTURE, {150, 0, 20, 300});
    m_entityManager->addEntity(BASE_TEXTURE, {30, 400, 200, 20});
    m_entityManager->addEntity(BASE_TEXTURE, {300, 400, 200, 20});
    m_entityManager->addEntity(BASE_TEXTURE, {350, 380, 25, 20});
    m_entityManager->addMoveableEntity(BASE_MOVEABLE_TEXTURE, {360, 365, 75, 15}, 10);
    m_entityManager->addEntity(BASE_TEXTURE, {400, 380, 25, 20});
    m_entityManager->addCollectible(BASE_COLLECTIBLE_TEXTURE, {380, 380, 20, 20});

    m_entityManager->addEntity(BASE_TEXTURE, {550, 325, 100, 10});
    m_entityManager->addCollectible(BASE_COLLECTIBLE_TEXTURE, {590, 305, 20, 20});
    m_entityManager->addEntity(BASE_TEXTURE, {500, 250, 100, 10});
    m_entityManager->addCollectible(BASE_COLLECTIBLE_TEXTURE, {540, 230, 20, 20});
    m_entityManager->addEntity(BASE_TEXTURE, {400, 150, 100, 10});
    m_entityManager->addCollectible(BASE_COLLECTIBLE_TEXTURE, {440, 130, 20, 20});
    m_entityManager->addEntity(BASE_TEXTURE, {450, 75, 100, 10});
    m_entityManager->addCollectible(BASE_COLLECTIBLE_TEXTURE, {490, 55, 20, 20});
}
