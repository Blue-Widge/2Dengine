#pragma once

enum
{
    SCREEN_WIDTH = 800,
    SCREEN_HEIGHT = 600,
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


