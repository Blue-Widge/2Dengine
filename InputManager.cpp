#include "InputManager.h"
#include "Gameloop.h"
#include "Inspector.h"

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
            case SDL_MOUSEBUTTONUP:
                if (m_gameloop->getPlayingGame())
                    break; 
            break;
            
            case SDL_MOUSEBUTTONDOWN:
                if (m_event.button.button == SDL_BUTTON_LEFT)
                {
                    const int mouseX = m_event.button.x;
                    const int mouseY = m_event.button.y;
                    if (mouseX > SCENE_WIDTH + HIERARCHY_WIDTH)
                    {
                        m_inspector->modifyInfoValue(mouseX, mouseY);
                        break;
                    }
                    if (mouseY > SCENE_HEIGHT)
                    {
                        if (m_gameStateButtons->detectPressedButtons(mouseX, mouseY))
                            break;
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

void InputManager::sendControls() const
{
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
