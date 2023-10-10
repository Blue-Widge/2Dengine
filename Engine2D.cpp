#include "SDLHandler.h"

int main(int argc, char* argv[])
{
    SDLHandler* handler = SDLHandler::getHandlerInstance();
    if (!handler->initSDL())
        return 1;
    handler->loop();
    delete handler;
    return 0;
}
