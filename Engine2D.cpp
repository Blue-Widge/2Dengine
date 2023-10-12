#include "SDLHandler.h"

int main()
{
    SDLHandler* handler = SDLHandler::getHandlerInstance();
    if (!handler->initSDL())
        return 1;
    handler->loop();
    delete handler;
    handler = nullptr;
    return 0;
}
