#define SDL_MAIN_USE_CALLBACKS

#include "MainMenuScene.h"
#include <Game.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <StandAloneFrame.h>

#include "BrickLayout.h"
#include "CheckPointDefinition.h"
#include "FloatingBrick.h"
#include "RoundSet.h"

using namespace nuvelocity;
using namespace frs42;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    // FIXME: This should not be here.
    ObjectRegistry::Get().Register(BrickLayout::GetClassInfo());
    ObjectRegistry::Get().Register(CheckPointDefinition::GetClassInfo());
    ObjectRegistry::Get().Register(FloatingBrick::GetClassInfo());
    ObjectRegistry::Get().Register(RoundSet::GetClassInfo());

#if DEBUG
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
#endif
    auto* game = new Game("FreeRS42");
    *appstate = game;
    if (game->Initialize(argv))
    {
        game->SetScene(new frs42::MainMenuScene());
        return SDL_APP_CONTINUE;
    }
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    auto* ctx = static_cast<Game*>(appstate);

    if (event->type == SDL_EVENT_QUIT)
    {
        ctx->mAppResult = SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    auto* game = static_cast<Game*>(appstate);
    game->Update();
    game->Draw();
    return game->mAppResult;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    auto* game = static_cast<Game*>(appstate);
    delete game;
    SDL_Quit();
}
