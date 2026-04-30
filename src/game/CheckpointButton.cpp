#include "CheckpointButton.h"

#include "Game.h"
#include "Sequence.h"
#include "system/FontManager.h"
#include "system/SpriteBatch.h"
#include <utility>

namespace nuvelocity::frs42
{
    CheckpointButton::CheckpointButton(Sequence* sequence, int roundNumber, std::string label)
            : mSequence(sequence)
            , mRoundNumber(roundNumber)
            , mLabel(std::move(label))
    {
    }

    void CheckpointButton::Draw(nuvelocity::Game* game)
    {
        if (game == nullptr || game->mSpriteBatch == nullptr)
        {
            return;
        }

        // Use the skin-provided button drawing (border, background, etc.)
        nuvelocity::Button::Draw(game);

        SDL_Rect rect = GetScreenRect();

        // Inset for content
        SDL_Rect innerRect = {.x = rect.x + 4, .y = rect.y + 4, .w = rect.w - 8, .h = rect.h - 8};
        Uint8 alpha = 255;

        if (IsPressed())
        {
            innerRect.x += 1;
            innerRect.y += 1;
            if (!IsEnabled())
            {
                alpha -= 100;
            }
        }

        if (mSequence != nullptr && mSequence->GetFrameCount() > 0)
        {
            SDL_Surface* surface = mSequence->GetSurface(0);
            if (surface != nullptr)
            {
                game->mSpriteBatch->Draw(surface,
                                         &innerRect,
                                         nullptr,
                                         SDL_Color{.r = 255, .g = 255, .b = 255, .a = alpha});
            }
        }

        // Draw the label (e.g. "1-1") in the bottom right
        std::string font = "OCR";
        int tw = 0;
        int th = 0;
        game->mFont->MeasureStringWithFont(font, mLabel, -1, tw, th);
        game->mFont->DrawStringWithFontAt(font,
                                          game->mSpriteBatch,
                                          mLabel,
                                          rect.x + rect.w - 8,
                                          rect.y + rect.h - 4 - th,
                                          {.r = 0, .g = 88, .b = 244, .a = 255},
                                          13,
                                          nuvelocity::TextAlignment::Right,
                                          nullptr,
                                          -1,
                                          {.r = 0, .g = 88, .b = 244, .a = 255});
    }
} // namespace nuvelocity::frs42
