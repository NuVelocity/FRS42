#include <Game.h>
#include <SDL3/SDL.h>

#include "MainMenuScene.h"

namespace nuvelocity::frs42
{
    MainMenuScene::MainMenuScene()
            : Scene()
    {
    }

    void MainMenuScene::Load(Game* aGame)
    {
        mBackgroundFrame =
            aGame->mAsset->LoadStandAloneFrame("Resources/Interface/Main Menu Extreme");
        auto asset = aGame->mAsset->Load("Music/Rock/Rockfast.ogg");
        aGame->mAudio->AssignBgm("Rock Fast", asset);
        asset = aGame->mAsset->Load("Music/Infinity/Infinity Fast.ogg");
        aGame->mAudio->AssignBgm("Infinity Fast", asset);
        aGame->mAudio->PlayBgm("Rock Fast");
        // asset = aGame->mAsset->Load("Sounds/Lost Ball.ogg");
        // aGame->mAudio->AssignSfx("Lost Ball", asset);
        // aGame->mAsset->DumpPropertyFile("Affiliate.txt");
        // aGame->mAsset->DumpPropertyFile("Resources/ModuleInfo/TheGame.modinfo");
        // aGame->mAsset->DumpPropertyFile("Resources/Interface/Main Menu Extreme.txt");
        aGame->mAsset->DumpPropertyFile("Resources/Arena/Score Board Frame.txt");
    }

    void MainMenuScene::Update(Game* aGame)
    {
        // static uint64_t startTime = SDL_GetTicks();
        // static bool oneDone = false, twoDone = false;
        // if (!oneDone && SDL_GetTicks() - startTime > 5000) {
        //     aGame->mAudio->StopBgm("Rock Fast", 1000);
        //     oneDone = true;
        // }
        // else if (!twoDone && SDL_GetTicks() - startTime > 8000) {
        //     aGame->mAudio->PlayBgm("Infinity Fast", 1000);
        //     twoDone = true;
        // }
        // static uint64_t lastSfxTime = SDL_GetTicks();
        // if (SDL_GetTicks() - lastSfxTime > 2000) {
        //     aGame->mAudio->PlaySfx("Lost Ball");
        //     lastSfxTime = SDL_GetTicks();
        // }
        // Update logic for main menu (if any)
    }

    void MainMenuScene::Draw(Game* aGame)
    {
        SDL_SetRenderDrawColor(aGame->mRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(aGame->mRenderer);

        if (mBackgroundFrame)
        {
            if (SDL_Texture* texture = mBackgroundFrame->GetTexture(aGame->mRenderer))
            {
                int winWidth, winHeight;
                SDL_GetWindowSizeInPixels(aGame->mWindow, &winWidth, &winHeight);

                float texWidth, texHeight;
                SDL_GetTextureSize(texture, &texWidth, &texHeight);

                SDL_FRect destRect = {(winWidth - texWidth) / 2.0f, (winHeight - texHeight) / 2.0f,
                                      texWidth, texHeight};

                SDL_RenderTexture(aGame->mRenderer, texture, nullptr, &destRect);
            }
        }

        SDL_RenderPresent(aGame->mRenderer);
    }

    std::string MainMenuScene::GetName() const
    {
        return "MainMenuScene";
    }
} // namespace nuvelocity::frs42
