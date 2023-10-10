    #pragma once
    #include <SDL_render.h>
    #include "Collider.h"

    enum Axis_e
    {
        x,
        y
    };

    template <typename T>
    struct Vec2
    {
        T x;
        T y;
    };


    class Entity
    {
    public:
        Entity() = default;
        Entity(SDL_Renderer* p_renderer, const char* p_path);
        Entity(SDL_Renderer* p_renderer, const char* p_path, const SDL_Rect& p_rect);
        ~Entity();

        void setPosition(const int p_x, const int p_y)
        {
            m_rect.x = p_x;
            m_rect.y = p_y;
        }

        void setRotation(const float p_rotationAngle) { m_rotationAngle = p_rotationAngle; }
        SDL_Rect getEntityRect() const { return m_rect; }
        
    protected:
        SDL_Renderer* m_renderer;
        SDL_Rect m_rect;
        SDL_Texture* m_texture;
        float m_rotationAngle = 0.f;
        BoxCollider* m_collider;
        Uint16 m_depth = 0;
    };

    class MoveableEntity : Entity
    {
        MoveableEntity(SDL_Renderer* p_renderer, const char* p_path, const SDL_Rect& p_rect, float p_mass);
        MoveableEntity(SDL_Renderer* p_renderer, const char* p_path, const SDL_Rect& p_rect, float p_mass,
                       float p_viscosity);

    public:
        void move(Axis_e p_axis, float p_moveSpeed, float p_deltaTime);
        void rotate(float p_rotationSpeed, float p_deltaTime);
        void applyForces(Vec2<float> p_forces);

    private:
        float m_mass = 0.f;
        float m_viscosity = 1.f;
        Vec2<float> m_velocity;

        bool m_gravityReactive = true;
        constexpr static float gravity = 9.81f;
        bool m_isKinematic = false;
    };
