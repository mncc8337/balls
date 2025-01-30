#ifndef BODY_H
#define BODY_H

#include <cmath>
#include <SDL3/SDL_render.h>
#include <vec3.h>

class Body {
public:
    Vec3 position = {0, 0, 0};
    double radius = 10;
    Vec3 color = {0.2, 0.2, 1};
    Vec3 velocity = {0, 0, 0};
    double mass = INFINITY;

    void apply_force(Vec3 force, double duration = 0.001);
    void inertia(double delta_time);
    bool collide_with(Body& other);
    void resolve_collision(Body& other);
    void draw(SDL_Renderer* renderer);
};

#endif
