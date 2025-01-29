#include <grav/body.h>

void Body::apply_force(Vec3 force, double duration) {
    if(mass == INFINITY) return;

    Vec3 accel = force / mass;
    velocity += accel * duration;
}

void Body::inertia(double delta_time) {
    velocity -= velocity * drag_accel * delta_time;
    rect.x += velocity.x * delta_time;
    rect.y += velocity.y * delta_time;
}
