#pragma once

#include <SDL_rect.h>

enum
{
    SCREEN_WIDTH = 1280,
    SCREEN_HEIGHT = 720,
    SCENE_WIDTH = 800,
    SCENE_HEIGHT = 550,
    INSPECTOR_WIDTH = (SCREEN_WIDTH - SCENE_WIDTH) / 2,
    INSPECTOR_HEIGHT = SCREEN_HEIGHT,
    HIERARCHY_WIDTH = INSPECTOR_WIDTH,
    HIERARCHY_HEIGHT = SCREEN_HEIGHT,
    FIXED_UPDATE_TIME = 10 // ms
};

enum Axis_e
{
    x,
    y
};
#pragma region floating SDL_Rect

struct FRect
{
    float x, y;
    float w, h;
};

inline SDL_Rect convertFRect(const FRect& p_rect)
{
    const SDL_Rect rect = {static_cast<int>(p_rect.x), static_cast<int>(p_rect.y), static_cast<int>(p_rect.w), static_cast<int>(p_rect.h)};
    return rect;
}
#pragma endregion

#pragma region vec2 with operators

template <typename T>
struct Vec2
{
    T x;
    T y;
};

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

template <typename T>
Vec2<T> operator*(float p_floatingPoint, const Vec2<T>& p_vec)
{
    Vec2<T> result = Vec2<T>({p_vec.x * p_floatingPoint, p_vec.y * p_floatingPoint});
    return result;
}

template <typename T>
Vec2<T> operator*(const Vec2<T>& p_vec, float p_floatingPoint)
{
    return p_floatingPoint * p_vec;
}
template <typename T>
Vec2<T> operator/(const Vec2<T>& p_vec, float p_floatingPoint)
{
    return p_vec * (1.f / p_floatingPoint);
}
#pragma endregion

#pragma region controls

enum Controls
{
    UP,
    RIGHT,
    DOWN,
    LEFT,
    USE
};

constexpr Uint8 controlsNb = 5;
#pragma endregion 