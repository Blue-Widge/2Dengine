#pragma once
#include <SDL_rect.h>

class Collider
{
public:
    Collider();
    virtual ~Collider();
private:
    
};

class BoxCollider : Collider
{
public:
    explicit BoxCollider(SDL_Rect p_rect);
    void changeBoxPosition(int p_x, int p_y);
    void changeBoxDimensions(int p_width, int p_height);
    SDL_Rect getColliderRect() const { return m_rect; }
private:
    SDL_Rect m_rect;
};
