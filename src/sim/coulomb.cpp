#define SDL_MAIN_USE_CALLBACKS 1

#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <body.h>
#include <vec3.h>

const int WIDTH = 640;
const int HEIGHT = 480;

const double METERS_PER_PIXEL = 1e-2;
const double TIME_SCALE = 1e-6;

// charge is stored using the y component of Body.color
// cursed, but it works
#define charge color.y

const double RESTITUTION = 0.0f;
const double k = 8.99e9;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* orbit_texture = NULL;

static int elapsed_ticks = 0;

static std::vector<Body> bodies;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("ERR::VIDEO: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if(!SDL_CreateWindowAndRenderer("coulomb", WIDTH, HEIGHT, 0, &window, &renderer)) {
        SDL_Log("ERR::WINDOW: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    orbit_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBX8888,
        SDL_TEXTUREACCESS_TARGET,
        WIDTH,
        HEIGHT
    );
    if(!orbit_texture) {
        SDL_Log("ERR::TEXTURE: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    Vec3 origin = Vec3(WIDTH/2.0, HEIGHT/2.0, 0)  * METERS_PER_PIXEL;

    Body part1;
    part1.position = origin + Vec3(-1, 0, 0);
    part1.mass = 0.01;
    part1.charge = -1;
    part1.radius = 0.1;
    bodies.push_back(part1);

    Body part2;
    part2.position = origin + Vec3(1, 0, 0);
    part2.mass = 0.01;
    part2.charge = 1;
    part2.radius = 0.1;
    bodies.push_back(part2);

    for(unsigned i = 0; i < bodies.size(); i++) {
        if(bodies[i].charge < 0)
            bodies[i].color.z = 1;
        else if(bodies[i].charge > 0)
            bodies[i].color.x = 1;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if(event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    int _ticks = SDL_GetTicks();
    int delta_ticks = _ticks - elapsed_ticks;
    elapsed_ticks = _ticks;

    const double delta_time = delta_ticks / 1e3 * TIME_SCALE;

    for(unsigned i = 0; i < bodies.size(); i++) {
        for(unsigned j = i + 1; j < bodies.size(); j++) {
            Vec3 p1 = bodies[i].position;
            Vec3 p2 = bodies[j].position;
            Vec3 dir = p1 - p2;

            double cmag = abs(k * bodies[i].charge * bodies[j].charge) / dir.squared_length();
            Vec3 cforce = dir.normalize() * cmag;

            if(bodies[i].charge * bodies[j].charge < 0) {
                bodies[i].apply_force(-cforce, delta_time);
                bodies[j].apply_force(cforce, delta_time);
            }
            else {
                bodies[i].apply_force(cforce, delta_time);
                bodies[j].apply_force(-cforce, delta_time);
            }
        }
    }

    for(unsigned i = 0; i < bodies.size(); i++) {
        bodies[i].inertia(delta_ticks / 1e3 * TIME_SCALE);
    }

    for(unsigned i = 0; i < bodies.size(); i++) {
        for(unsigned j = i + 1; j < bodies.size(); j++) {
            bodies[i].resolve_collision(bodies[j], RESTITUTION);
        }
    }

    SDL_RenderTexture(renderer, orbit_texture, NULL, NULL);

    for(unsigned i = 0; i < bodies.size(); i++) {
        if(bodies[i].radius > 3000) continue;
        double chr = bodies[i].color.y;
        bodies[i].color.y = 0;

        bodies[i].draw(renderer, {0, 0, 0}, METERS_PER_PIXEL);
        Vec3 position_px = bodies[i].position / METERS_PER_PIXEL;
        Vec3 d = bodies[i].velocity.normalize() * 10 + position_px;
        SDL_SetRenderDrawColor(renderer, COLOR(bodies[i].color), SDL_ALPHA_OPAQUE);
        SDL_RenderLine(renderer, position_px.x, position_px.y, d.x, d.y);

        bodies[i].color.y = chr;
    }

    // draw orbit
    SDL_SetRenderTarget(renderer, orbit_texture);
    for(unsigned i = 0; i < bodies.size(); i++) {
        double chr = bodies[i].color.y;
        bodies[i].color.y = 0;

        Vec3 position_px = bodies[i].position / METERS_PER_PIXEL;
        SDL_SetRenderDrawColor(renderer, COLOR(bodies[i].color), SDL_ALPHA_OPAQUE);
        SDL_RenderPoint(renderer, position_px.x, position_px.y);

        bodies[i].color.y = chr;
    }
    SDL_SetRenderTarget(renderer, NULL);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    SDL_DestroyTexture(orbit_texture);
}
