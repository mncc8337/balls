#include <cmath>
#include <SDL3/SDL_rect.h>
#include <vec3.h>

// this Body object does not deal with collisions

class Body {
public:
    SDL_FRect rect;
    double mass = INFINITY;
    Vec3 velocity = Vec3(0, 0, 0);
    double drag_accel = 0;

    void apply_force(Vec3 force, double duration = 0.001);
    void inertia(double delta_time);
};
