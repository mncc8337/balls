#define SDL_MAIN_USE_CALLBACKS 1

#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <grav/body.h>
#include <grav/constants.h>
#include <vec3.h>

const int WIDTH = 640;
const int HEIGHT = 480;

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

    if(!SDL_CreateWindowAndRenderer("grav", WIDTH, HEIGHT, 0, &window, &renderer)) {
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

    Body body;

    body.position = {WIDTH/2.0, HEIGHT/2.0, 0};
    body.radius = 20;
    body.color = {0.2, 0.2, 1};
    body.velocity = {0, 0, 0};
    body.mass = 1e33;
    bodies.push_back(body);

    body.position = {50, 450, 0};
    body.radius = 13;
    body.color = {1, 0.2, 0.2};
    body.velocity = {12, 0, 0};
    body.mass = 1e24;
    bodies.push_back(body);

    body.position = {100, 330, 0};
    body.radius = 18;
    body.color = {0.7, 0.8, 0.2};
    body.velocity = {12, 5, 0};
    body.mass = 1e30;
    bodies.push_back(body);

    body.position = {130, 360, 0};
    body.radius = 5;
    body.color = {0.2, 1, 0.2};
    body.velocity = {12, 10, 0};
    body.mass = 1e6;
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

    const double time_scale = 1;
    const double delta_time = delta_ticks / 1e3 * time_scale;

    for(unsigned i = 0; i < bodies.size(); i++) {
        for(unsigned j = i + 1; j < bodies.size(); j++) {
            Vec3 p1 = bodies[i].position * METERS_PER_PIXEL;
            Vec3 p2 = bodies[j].position * METERS_PER_PIXEL;
            Vec3 dir = p1 - p2;
            SDL_Log("INF::GAME: len^2 %f", dir.squared_length());

            double gmag = (G * bodies[i].mass * bodies[j].mass) / dir.squared_length();
            SDL_Log("INF::GAME: mag %f", gmag);

            Vec3 gforce = dir.normalize() * gmag;

            bodies[i].apply_force(-gforce, delta_time);
            bodies[j].apply_force(gforce, delta_time);
        }
    }

    for(unsigned i = 0; i < bodies.size(); i++) {
        bodies[i].inertia(delta_ticks / 1e3 * time_scale);
    }

    for(unsigned i = 0; i < bodies.size(); i++) {
        for(unsigned j = i + 1; j < bodies.size(); j++) {
            bodies[i].resolve_collision(bodies[j]);
        }
    }

    SDL_RenderTexture(renderer, orbit_texture, NULL, NULL);

    for(unsigned i = 0; i < bodies.size(); i++) {
        bodies[i].draw(renderer);
    }
    for(unsigned i = 0; i < bodies.size(); i++) {
        bodies[i].draw(renderer);
        Vec3 d = bodies[i].velocity.normalize() * 50 + bodies[i].position;
        SDL_SetRenderDrawColor(renderer, COLOR(bodies[i].color), SDL_ALPHA_OPAQUE);
        SDL_RenderLine(renderer, bodies[i].position.x, bodies[i].position.y, d.x, d.y);
    }

    // draw orbit to texture
    SDL_SetRenderTarget(renderer, orbit_texture);
    for(unsigned i = 0; i < bodies.size(); i++) {
        SDL_SetRenderDrawColor(renderer, COLOR(bodies[i].color), SDL_ALPHA_OPAQUE);
        SDL_RenderPoint(renderer, bodies[i].position.x, bodies[i].position.y);
    }
    SDL_SetRenderTarget(renderer, NULL);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    SDL_DestroyTexture(orbit_texture);
}
