#define SDL_MAIN_USE_CALLBACKS 1

#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <grav/body.h>
#include <vec3.h>

#include <cstdio>

const int WIDTH = 640;
const int HEIGHT = 480;

const double G = 6.6743e-11;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static int elapsed_ticks = 0;

static std::vector<Body> bodies;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("ERR::VIDEO: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if(!SDL_CreateWindowAndRenderer("grav", WIDTH, HEIGHT, 0, &window, &renderer)) {
        SDL_Log("ERR::WINDOW: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    Body body;
    body.rect.w = 10; body.rect.h = 10;
    body.rect.x = 100; body.rect.y = 100;
    body.mass = 1e15;
    body.velocity = {-10, 2, 0};
    bodies.push_back(body);

    body.rect.x = 200; body.rect.y = 200;
    body.mass = 1e17;
    body.velocity = {0, -2, 0};
    bodies.push_back(body);

    body.rect.x = 200; body.rect.y = 300;
    body.mass = 1e14;
    body.velocity = {5, -2, 0};
    bodies.push_back(body);

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

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 23, 23, 200, SDL_ALPHA_OPAQUE);

    for(unsigned i = 0; i < bodies.size(); i++) {
        for(unsigned j = 0; j < bodies.size(); j++) {
            if(i == j) continue;

            Vec3 p1 = {bodies[i].rect.x, bodies[i].rect.y, 0};
            Vec3 p2 = {bodies[j].rect.x, bodies[j].rect.y, 0};
            Vec3 dir = p1 - p2;
            printf("%f\n", dir.squared_length());

            double g_magnitude = (G * bodies[i].mass * bodies[j].mass) / dir.squared_length();
            // limit the mag else it will go wild
            if(g_magnitude > 1e16) {
                g_magnitude = 1e16;
            }

            // this force will be appled 2 times so we need to divide it by 2
            Vec3 gforce = dir.normalize() * g_magnitude / 2;

            bodies[i].apply_force(-gforce, delta_ticks / 1000.0);
            bodies[j].apply_force( gforce, delta_ticks / 1000.0);
        }
    }

    for(unsigned i = 0; i < bodies.size(); i++) {
        bodies[i].inertia(delta_ticks / 1000.0);
        SDL_RenderFillRect(renderer, &bodies[i].rect);
    }

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
}
