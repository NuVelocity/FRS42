#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <StandAloneFrame.h>
#include <Game.h>
#include "MainMenuScene.h"

using namespace nuvelocity;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    auto* game = new Game("FreeRS42");
    *appstate = game;
    if (game->Initialize(argv)) {
        game->SetScene(new frs42::MainMenuScene());
        return SDL_APP_CONTINUE;
    }
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    auto* ctx = (Game*)appstate;

    if (event->type == SDL_EVENT_QUIT)
    {
        ctx->mAppResult = SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) 
{
    auto* game = (Game*)appstate;
    game->Update();
    game->Draw();
    return game->mAppResult;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    auto* game = (Game*)appstate;
    if (game) {
        delete game;
    }
    SDL_Quit();
}
