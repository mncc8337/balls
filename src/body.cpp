#include <body.h>

void Body::apply_force(Vec3 force, double duration) {
    if(mass == INFINITY) return;

    Vec3 accel = force / mass;
    velocity += accel * duration;
}

void Body::inertia(double delta_time) {
    position += velocity * delta_time;
}

bool Body::collide_with(Body& other) {
    Vec3 d = other.position - position;
    return d.squared_length() <= (other.radius + radius) * (other.radius + radius);
}

void Body::resolve_collision(Body& other, const double restitution) {
    Vec3 d = other.position - position;
    float distance_squared = d.squared_length();
    float radius_sum = radius + other.radius;

    if(distance_squared >= radius_sum * radius_sum)
        return;

    Vec3 normal = d.normalize();

    Vec3 relative_velocity = velocity - other.velocity;
    float impulse_magnitude = -(1.0f + restitution) * relative_velocity.dot(normal);
    impulse_magnitude /= (1.0f / mass + 1.0f / other.mass);
    Vec3 impulse = normal * impulse_magnitude;

    if(mass != INFINITY) {
        velocity += impulse / mass;
    }
    if(other.mass != INFINITY) {
        other.velocity -= impulse / other.mass;
    }

    // avoid overlapping
    float penetration_depth = radius_sum - std::sqrt(distance_squared);
    Vec3 correction = normal * (penetration_depth / (1.0f / mass + 1.0f / other.mass)) * 1.0f;
    if(mass != INFINITY) {
        position -= correction / mass;
    }
    if(other.mass != INFINITY) {
        other.position += correction / other.mass;
    }
}

void Body::draw(SDL_Renderer* renderer, const Vec3 offset, const double meters_per_pixel) {
    Vec3 position_px = (position + offset) / meters_per_pixel;
    double radius_px = radius / meters_per_pixel;

    const double diameter_px = radius_px * 2;

    double x = radius_px - 1;
    double y = 0;
    double tx = 1;
    double ty = 1;
    double error = tx - diameter_px;

    SDL_SetRenderDrawColor(renderer, COLOR(color), SDL_ALPHA_OPAQUE);
    while (x >= y) {
        SDL_RenderPoint(renderer, position_px.x + x, position_px.y - y);
        SDL_RenderPoint(renderer, position_px.x + x, position_px.y + y);
        SDL_RenderPoint(renderer, position_px.x - x, position_px.y - y);
        SDL_RenderPoint(renderer, position_px.x - x, position_px.y + y);
        SDL_RenderPoint(renderer, position_px.x + y, position_px.y - x);
        SDL_RenderPoint(renderer, position_px.x + y, position_px.y + x);
        SDL_RenderPoint(renderer, position_px.x - y, position_px.y - x);
        SDL_RenderPoint(renderer, position_px.x - y, position_px.y + x);

        if(error <= 0) {
            y += 1;
            error += ty;
            ty += 2;
        }
        else {
            x -= 1;
            tx += 2;
            error += tx - diameter_px;
        }
    }
}
