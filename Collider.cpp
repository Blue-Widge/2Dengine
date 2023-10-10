#include "Collider.h"

BoxCollider::BoxCollider(SDL_Rect p_rect)
{
    
}

void BoxCollider::changeBoxPosition(const int p_x, const int p_y)
{
    m_rect.x = p_x;
    m_rect.y = p_y;
}

void BoxCollider::changeBoxDimensions(const int p_width, const int p_height)
{
    m_rect.w = p_width;
    m_rect.h = p_height;
}
