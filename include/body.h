#ifndef BODY_H
#define BODY_H

#define COLOR(color) (color.x * 255), (color.y * 255), (color.z * 255)

#include <cmath>
#include <SDL3/SDL_render.h>
#include <vec3.h>

class Body {
public:
    Vec3 position = {0, 0, 0}; // meter
    double radius = 10; // meter
    Vec3 color = {0, 0, 0};
    Vec3 velocity = {0, 0, 0}; // meter/second
    double mass = INFINITY; // kilogram

    void apply_force(Vec3 force, double duration = 0.001);
    void inertia(double delta_time);
    bool collide_with(Body& other);
    void resolve_collision(Body& other, const double restitution);
    void draw(SDL_Renderer* renderer, const Vec3 offset, const double meters_per_pixel);
};

#endif
