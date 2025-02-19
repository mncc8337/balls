#define SDL_MAIN_USE_CALLBACKS 1

#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <body.h>
#include <vec3.h>

const int WIDTH = 640;
const int HEIGHT = 480;

const double METERS_PER_PIXEL = 0.25e7;
const double TIME_SCALE = 500000;

const double RESTITUTION = 0.0f;
const double G = 6.6743e-11;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* orbit_texture = NULL;

static int elapsed_ticks = 0;

static std::vector<Body> bodies;

Vec3 origin = Vec3(WIDTH/2.0, HEIGHT/2.0, 0) * METERS_PER_PIXEL;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("ERR::VIDEO: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if(!SDL_CreateWindowAndRenderer("gravity", WIDTH, HEIGHT, 0, &window, &renderer)) {
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

    Body earth;
    earth.position = {0, 0, 0};
    earth.mass = 5.972168e24;
    earth.color = {0.4, 0.8, 0.5};
    earth.radius = 6378e3;
    bodies.push_back(earth);

    Body moon;
    moon.position = {405000e3, 0, 0};
    moon.velocity = Vec3(-0.0001, -0.0004, 0) * METERS_PER_PIXEL;
    moon.mass = 7.34767309e22;
    moon.color = {0.5, 0.5, 0.7};
    moon.radius = 1737.4e3;
    bodies.push_back(moon);

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

            double gmag = (G * bodies[i].mass * bodies[j].mass) / dir.squared_length();
            Vec3 gforce = dir.normalize() * gmag;

            bodies[i].apply_force(-gforce, delta_time);
            bodies[j].apply_force(gforce, delta_time);
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

    Vec3 offset = origin;

    for(unsigned i = 0; i < bodies.size(); i++) {
        bodies[i].draw(renderer, offset, METERS_PER_PIXEL);
        Vec3 position_px = (bodies[i].position + offset) / METERS_PER_PIXEL;
        Vec3 d = bodies[i].velocity.normalize() * 10 + position_px;
        SDL_SetRenderDrawColor(renderer, COLOR(bodies[i].color), SDL_ALPHA_OPAQUE);
        SDL_RenderLine(renderer, position_px.x, position_px.y, d.x, d.y);
    }

    // draw orbit
    SDL_SetRenderTarget(renderer, orbit_texture);
    for(unsigned i = 0; i < bodies.size(); i++) {
        Vec3 position_px = (bodies[i].position + offset) / METERS_PER_PIXEL;
        SDL_SetRenderDrawColor(renderer, COLOR(bodies[i].color), SDL_ALPHA_OPAQUE);
        SDL_RenderPoint(renderer, position_px.x, position_px.y);
    }
    SDL_SetRenderTarget(renderer, NULL);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    SDL_DestroyTexture(orbit_texture);
}
