#define SDL_MAIN_USE_CALLBACKS

#include "MainMenuScene.h"
#include "SplashScene.h"
#include <Game.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <system/AssetManager.h>
#include <system/AudioManager.h>

#include "GameObjectRegistration.h"
#include <system/ui/MdiManager.h>

using namespace nuvelocity;
using namespace frs42;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    nuvelocity::frs42::RegisterGameObjectTypes();

#if DEBUG
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
#endif
    auto* game = new Game("FreeRS42");
    *appstate = game;

    game->GetArgs()
        .add_argument("--skip-splash")
        .help("Skip the splash screen")
        .default_value(false)
        .implicit_value(true);

    game->SetMouseCursor("Resources/Interface/MouseCursor");
    game->SetModuleInfo("Resources/ModuleInfo/TheGame.modinfo");
    if (game->Initialize(argc, argv))
    {
        // Load main menu music early and assign substitutions.
        game->mAsset->AddMusicSubstitution("Infinity/Infinity Norm.ogg",
                                           "Infinity/Infinity Fast.ogg");
        game->mAsset->AddMusicSubstitution("Infinity/Infinity Slow.ogg",
                                           "Infinity/Infinity Fast.ogg");
        game->mAsset->AddMusicSubstitution("Rock/Rocknorm.ogg", "Rock/Rockfast.ogg");
        game->mAsset->AddMusicSubstitution("Rock/Rockslow.ogg", "Rock/Rockfast.ogg");
        game->mAsset->AddMusicSubstitution("Space/SpaceRumble norm.ogg",
                                           "Space/SpaceRumble fast.ogg");
        game->mAsset->AddMusicSubstitution("Space/SpaceRumble slow.ogg",
                                           "Space/SpaceRumble fast.ogg");
        game->mAsset->AddMusicSubstitution("Water/Water Norm.ogg", "Water/Water Fast.ogg");
        game->mAsset->AddMusicSubstitution("Water/Water Slow.ogg", "Water/Water Fast.ogg");
        game->mAsset->AddMusicSubstitution("Theme.ogg", "Rock/Rockfast.ogg");
        game->mAudio->RegisterBgm(game->mAsset->LoadMusic("Theme.ogg"));

        // Load and Register Window Skins
        JWindowSkin* ricochetSkin =
            game->mAsset->LoadWindowSkin("Resources/Interface/JWindowSkins/Ricochet.JWindowSkin");
        if (ricochetSkin != nullptr)
        {
            game->mMdi->RegisterSkin("Ricochet", ricochetSkin);
        }

        if (game->GetArgs().get<bool>("--skip-splash"))
        {
            game->SetScene(new MainMenuScene());
        }
        else
        {
            game->SetScene(new SplashScene());
        }
        return SDL_APP_CONTINUE;
    }
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    auto* ctx = static_cast<Game*>(appstate);
    ctx->HandleEvent(*event);

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
    game->EndFrame();
    return game->mAppResult;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    auto* game = static_cast<Game*>(appstate);
    delete game;
    SDL_Quit();
}
