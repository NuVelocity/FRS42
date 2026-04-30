#include "CheckpointButton.h"

#include "Game.h"
#include "Sequence.h"
#include "system/FontManager.h"
#include "system/SpriteBatch.h"
#include <utility>

namespace nuvelocity::frs42
{
    CheckpointButton::CheckpointButton(Sequence* image, int roundNumber, std::string label)
            : mImage(image)
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
        SDL_Rect innerRect = rect;
        innerRect.x += 2;
        innerRect.y += 2;
        innerRect.w -= 4;
        innerRect.h -= 4;

        if (mImage != nullptr && mImage->GetFrameCount() > 0)
        {
            SDL_Surface* surface = mImage->GetSurface(0);
            if (surface != nullptr)
            {
                game->mSpriteBatch->Draw(surface, &innerRect);
            }
        }

        // Draw the label (e.g. "1-1") in the bottom right
        std::string font = "OCR";
        int tw = 0;
        int th = 0;
        game->mFont->MeasureStringWithFont(font, mLabel, 8, tw, th);
        game->mFont->DrawStringWithFontAt(font,
                                          game->mSpriteBatch,
                                          mLabel,
                                          rect.x + rect.w - tw - 6,
                                          rect.y + rect.h - th - 6,
                                          {.r = 0, .g = 88, .b = 244, .a = 255},
                                          13,
                                          nuvelocity::TextAlignment::Left,
                                          nullptr,
                                          -1,
                                          {.r = 0, .g = 88, .b = 244, .a = 255});

        if (IsHovered() && !IsPressed())
        {
            game->mSpriteBatch->OutlineRect(&rect, {.r = 255, .g = 255, .b = 255, .a = 128});
        }
    }
} // namespace nuvelocity::frs42
