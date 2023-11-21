#include "InputManager.h"
#include "Gameloop.h"
#include "Inspector.h"

extern int g_scenePosX;
extern int g_scenePosY;
extern int g_sceneWidth;
extern int g_sceneHeight;

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
            case SDLK_DELETE:
            case SDLK_BACKSPACE:
            case SDLK_KP_BACKSPACE:
                m_controls[DELETE] = true;
                break;
            default:
                break;
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
            case SDLK_DELETE:
            case SDLK_BACKSPACE:
            case SDLK_KP_BACKSPACE:
                m_controls[DELETE] = false;
                break;
            default: ;
            }
        case SDL_MOUSEBUTTONUP:
            if (m_gameloop->getPlayingGame())
                break;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (m_event.button.button == SDL_BUTTON_LEFT)
            {
                const int mouseX = m_event.button.x;
                const int mouseY = m_event.button.y;
                if (!m_gameloop->getPlayingGame() && mouseX > g_scenePosX + g_sceneWidth)
                {
                    m_inspector->modifyInfoValue(mouseX, mouseY);
                    break;
                }

                if (!m_gameloop->getPlayingGame() && mouseX < g_scenePosX)
                    if (m_hierarchy->detectClickedName(mouseX, mouseY))
                        break;

                if (m_gameStateButtons->detectPressedButtons(mouseX, mouseY))
                    break;

                if (mouseY > g_sceneHeight)
                {
                    if (m_entityChooser->detectChosenEntity(mouseX, mouseY))
                        break;
                    //SOMETHING ELSE ?
                    break;
                }
                Entity* entity = m_gameloop->getEntityFromPos(mouseX, mouseY);
                m_inspector->selectEntity(entity);
            }
            break;
        default:
            break;
        }
    }
}

void InputManager::sendControls()
{
    if (!m_gameloop->getPlayingGame() && m_controls[DELETE])
    {
        const Entity* toDeleteEntity = m_inspector->getSelectedEntity();
        if (toDeleteEntity == nullptr)
            return;
        m_inspector->setEntityPtr(nullptr);
        m_entityManager->deleteEntityUpdate(toDeleteEntity);
        m_hierarchy->updateHierarchy();
        m_controls[DELETE] = false;
    }

    if (!m_player)
        return;

    if (m_controls[UP])
        if (m_player->getOnGround())
        {
            m_player->playJumpSound();
            m_player->setYVelocity(-250.f);
        }
    if (m_controls[RIGHT])
        m_player->setXVelocity(100.f);
    if (m_controls[LEFT])
        m_player->setXVelocity(-100.f);
    if ((m_controls[LEFT] ^ m_controls[RIGHT]) == false)
        m_player->setXVelocity(0.f);
}
