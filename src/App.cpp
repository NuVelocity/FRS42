#define SDL_MAIN_USE_CALLBACKS

#include "MainMenuScene.h"
#include "SplashScene.h"
#include <FontBitmap.h>
#include <Game.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>

#include <array>
#include <memory>

#include "GameObjectRegistration.h"
#include <system/ui/MdiManager.h>

using namespace nuvelocity;
using namespace frs42;

static void RegisterGameFonts(Game* game)
{
    if (game == nullptr || game->mAsset == nullptr || game->mFont == nullptr)
    {
        return;
    }

    constexpr std::array<std::pair<const char*, const char*>, 12> kBitmapFonts = {
        std::pair{"Big White", "Fonts/Big White"},
        std::pair{"Med Gold", "Fonts/Med Gold"},
        std::pair{"Megovision", "Fonts/Megovision"},
        std::pair{"Numbers Blue", "Fonts/Numbers Blue"},
        std::pair{"OCR", "Fonts/OCR"},
        std::pair{"Sell", "Fonts/Sell"},
        std::pair{"Small Blue", "Fonts/Small Blue"},
        std::pair{"Small Gold", "Fonts/Small Gold"},
        std::pair{"WebLarge Blue", "Fonts/WebLarge Blue"},
        std::pair{"WebSmall Black", "Fonts/WebSmall Black"},
        std::pair{"Yellow Header", "Fonts/Yellow Header"},
        std::pair{"Yellow Large Header", "Fonts/Yellow Large Header"},
    };

    for (const auto& [name, path] : kBitmapFonts)
    {
        FontBitmap* bitmapFont = game->mAsset->LoadFontBitmap(path);
        if (bitmapFont == nullptr)
        {
            continue;
        }

        game->mFont->RegisterFont(name, std::unique_ptr<Font>(static_cast<Font*>(bitmapFont)));
    }
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    nuvelocity::frs42::RegisterGameObjectTypes();

#if DEBUG
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
#endif
    auto* game = new Game("FreeRS42");
    *appstate = game;

    game->GetArgs().add_argument("--debug-collisions")
        .help("Enable debug collision drawing")
        .default_value(false)
        .implicit_value(true);

    game->GetArgs().add_argument("--skip-splash")
        .help("Skip the splash screen")
        .default_value(false)
        .implicit_value(true);

    game->SetMouseCursor("Resources/Interface/MouseCursor");
    game->SetModuleInfo("Resources/ModuleInfo/TheGame.modinfo");
    if (game->Initialize(argc, argv))
    {
        RegisterGameFonts(game);

        // Load main menu music early.
        auto* asset = game->mAsset->Load("Music/Rock/Rockfast.ogg");
        game->mAudio->AssignBgm("Rock Fast", asset);

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
