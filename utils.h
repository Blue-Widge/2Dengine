#pragma once
#include <system_error>

enum
{
    SCREEN_WIDTH = 1280,
    SCREEN_HEIGHT = 720,
    SCENE_WIDTH = 800,
    SCENE_HEIGHT = 550,
    FIXED_UPDATE_TIME = 10 // ms
};

template <typename T>
struct Vec2
{
    T x;
    T y;
};

struct FRect
{
    float x, y;
    float w, h;
};

inline SDL_Rect ConvertFRect(const FRect& p_rect)
{
    const SDL_Rect rect = {static_cast<int>(p_rect.x), static_cast<int>(p_rect.y), static_cast<int>(p_rect.w), static_cast<int>(p_rect.h)};
    return rect;
}

template <typename T>
Vec2<T> operator +(const Vec2<T>& p_first, const Vec2<T>& p_second)
{
    return {p_first.x + p_second.x, p_first.y + p_second.y};
}

template <typename T>
Vec2<T> operator -(const Vec2<T>& p_first, const Vec2<T>& p_second)
{
    return {p_first.x - p_second.x, p_first.y - p_second.y};
}

template <typename T>
Vec2<T> operator +=(const Vec2<T>& p_first, const Vec2<T>& p_second)
{
    return {p_first.x + p_second.x, p_first.y + p_second.y};
}
template <typename T>
Vec2<T> operator -=(const Vec2<T>& p_first, const Vec2<T>& p_second)
{
    return {p_first.x - p_second.x, p_first.y - p_second.y};
}

enum Controls
{
    UP,
    RIGHT,
    DOWN,
    LEFT,
    USE
};

constexpr Uint8 controlsNb = 5;

