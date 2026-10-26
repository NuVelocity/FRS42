#include "Megovision.h"
#include <Colors.h>
#include <Font.h>
#include <FontBitmap.h>
#include <Game.h>
#include <StandAloneFrame.h>
#include <algorithm>
#include <cmath>
#include <system/AssetManager.h>
#include <system/FontManager.h>
#include <system/SpriteBatch.h>

namespace nuvelocity::frs42
{
    constexpr int kMessageRectWidthAllowance = 5;
    constexpr int kSmallBlueWidthAllowance = -20;
    const SDL_Rect Megovision::kMessageAreaRect = {.x = 530 - (kMessageRectWidthAllowance / 2),
                                                   .y = 35,
                                                   .w = 90 + kMessageRectWidthAllowance,
                                                   .h = 95};

    Megovision::Megovision() = default;

    void Megovision::Load(Game* game)
    {
        mScoreBoardFrame = game->mAsset->LoadStandAloneFrame("Resources/Arena/Score Board Frame");

        auto adPaths = game->mAsset->EnumerateAdvertisements();
        for (const auto& ad : adPaths)
        {
            StandAloneFrame* frame = game->mAsset->LoadStandAloneFrame(ad.first);
            if (frame != nullptr)
            {
                mAdverts.push_back(frame);
            }
        }
    }

    void Megovision::Update(Game* game)
    {
        // Update ticker
        if (mTickerWidth > 0)
        {
            float loopWidth = static_cast<float>(mTickerWidth) + 25.0F;
            mTickerX -= game->GetDeltaTime() * 30.0F;
            if (mTickerX <= -loopWidth)
            {
                mTickerX += loopWidth;
            }
        }

        // Update messages
        if (mMessageTimer > 0.0F)
        {
            mMessageTimer -= game->GetDeltaTime();
            if (mMessageTimer <= 0.0F)
            {
                if (mMessageFlash)
                {
                    mMessageTimer = 1.75F; // Loop the flash cycle
                }
                else
                {
                    mMessageLabels.clear();
                }
            }
        }

        if (mMessageLayoutDirty && game != nullptr && game->mFont != nullptr)
        {
            if (mMessageLabels.size() == 1)
            {
                auto& label = mMessageLabels[0];
                label->SetAlignment(TextAlignment::Center);
                label->SetVerticalCenter(true);
                label->SetPointSize(kFontBitmapDefaultPointSize);
                label->SetWrap(true);
                label->SetRect(kMessageAreaRect);
            }
            else if (!mMessageLabels.empty())
            {
                for (auto& label : mMessageLabels)
                {
                    label->SetAlignment(TextAlignment::Center);
                    label->SetVerticalCenter(false);
                    label->SetPointSize(kFontBitmapDefaultPointSize);
                    label->SetWrap(true);
                }

                int totalHeight = 0;
                for (const auto& label : mMessageLabels)
                {
                    int maxWidth = kMessageAreaRect.w;
                    if (label->GetFont() == "Small Blue")
                    {
                        maxWidth += kSmallBlueWidthAllowance;
                    }
                    totalHeight += label->GetRequiredHeight(game, maxWidth);
                }

                int currentY = kMessageAreaRect.y + ((kMessageAreaRect.h - totalHeight) / 2);
                for (auto& label : mMessageLabels)
                {
                    int maxWidth = kMessageAreaRect.w;
                    if (label->GetFont() == "Small Blue")
                    {
                        maxWidth += kSmallBlueWidthAllowance;
                    }
                    int h = label->GetRequiredHeight(game, maxWidth);
                    SDL_Rect r = {kMessageAreaRect.x + (kMessageAreaRect.w - maxWidth) / 2,
                                  currentY,
                                  maxWidth,
                                  h};
                    label->SetRect(r);
                    currentY += h;
                }
            }
            mMessageLayoutDirty = false;
        }

        // Update Advertisements
        if (!mAdverts.empty())
        {
            float dt = game->GetDeltaTime();
            if (mIsTransitioning)
            {
                mAdTransitionProgress += dt / mAdTransitionDuration;
                if (mAdTransitionProgress >= 1.0F)
                {
                    mIsTransitioning = false;
                    mCurrentAdIndex = mNextAdIndex;
                    mAdTransitionProgress = 0.0F;
                    mAdTimer = mAdDisplayDuration;
                }
            }
            else
            {
                mAdTimer -= dt;
                if (mAdTimer <= 0.0F)
                {
                    mIsTransitioning = true;
                    mNextAdIndex = (mCurrentAdIndex + 1) % mAdverts.size();
                    mAdTransitionProgress = 0.0F;
                }
            }
        }
    }

    void Megovision::Draw(Game* game)
    {
        if (mScoreBoardFrame == nullptr)
        {
            return;
        }

        // Draw Advertisements
        if (!mAdverts.empty())
        {
            const int adX = 524;
            const int adY = 337;

            if (mIsTransitioning)
            {
                const int numStrips = 6;
                StandAloneFrame* curAd = mAdverts[mCurrentAdIndex];
                StandAloneFrame* nextAd = mAdverts[mNextAdIndex];
                int adW = curAd->GetWidth();
                int adH = curAd->GetHeight();
                int stripW = adW / numStrips;

                for (int i = 0; i < numStrips; ++i)
                {
                    int sx = i * stripW;

                    // Stagger the progress across strips
                    float stagger = static_cast<float>(i) / static_cast<float>(numStrips) * 0.5F;
                    float stripProgress =
                        std::clamp((mAdTransitionProgress - stagger) / 0.5F, 0.0F, 1.0F);

                    // Rotate 0 to 90 degrees
                    float angle = stripProgress * 1.5708F;
                    float cosVal = std::cos(angle);
                    float sinVal = std::sin(angle);

                    // Outgoing strip (anchored left)
                    int w1 = static_cast<int>(static_cast<float>(stripW) * cosVal);
                    if (w1 > 0)
                    {
                        uint8_t shade = static_cast<uint8_t>(255 * cosVal);
                        SDL_Color color = {.r = shade, .g = shade, .b = shade, .a = 255};
                        SDL_Rect src1 = {.x = sx, .y = 0, .w = stripW, .h = adH};
                        SDL_Rect dst1 = {.x = adX + sx, .y = adY, .w = w1, .h = adH};
                        game->mSpriteBatch->Draw(curAd->GetSurface(), &dst1, &src1, color);
                    }

                    // Incoming strip (anchored right)
                    int w2 = static_cast<int>(static_cast<float>(stripW) * sinVal);
                    if (w2 > 0)
                    {
                        uint8_t shade = static_cast<uint8_t>(255 * sinVal);
                        SDL_Color color = {.r = shade, .g = shade, .b = shade, .a = 255};
                        SDL_Rect src2 = {.x = sx, .y = 0, .w = stripW, .h = adH};
                        SDL_Rect dst2 = {.x = adX + sx + stripW - w2, .y = adY, .w = w2, .h = adH};
                        game->mSpriteBatch->Draw(nextAd->GetSurface(), &dst2, &src2, color);
                    }
                }
            }
            else
            {
                game->mSpriteBatch->Draw(mAdverts[mCurrentAdIndex], adX, adY);
            }
        }

        // Draw frame at 0,0 as in original implementation
        game->mSpriteBatch->Draw(mScoreBoardFrame, 0, 0);

        int sbX = mScoreBoardFrame->GetWidth();
        std::string labelFont = "Yellow Header";
        std::string numFont = "Numbers Blue";

        // Constants from original implementation
        int labelY = 185 - 8;
        int spacing = 30;

        auto drawStat = [&](const std::string& label, const std::string& value, int x, int y)
        {
            int tw = 16;
            // Label
            game->mFont->DrawStringWithFontAt(labelFont,
                                              game->mSpriteBatch,
                                              label,
                                              x - tw,
                                              y,
                                              Colors::White,
                                              kFontBitmapDefaultPointSize,
                                              TextAlignment::Right);

            // Value
            game->mFont->DrawStringWithFontAt(numFont,
                                              game->mSpriteBatch,
                                              value,
                                              x - tw,
                                              y + 10,
                                              Colors::White,
                                              kFontBitmapDefaultPointSize,
                                              TextAlignment::Right);
        };

        // Time
        int totalMs = static_cast<int>(mLevelTime * 1000.0F);
        int ms = (totalMs % 1000) / 10;
        int sec = (totalMs / 1000) % 60;
        int min = (totalMs / 60000);
        char timeBuf[16];
        SDL_snprintf(timeBuf, sizeof(timeBuf), "%d:%02d:%02d", min, sec, ms);
        drawStat("TIME", timeBuf, sbX, labelY);

        // Score
        drawStat("SCORE", std::to_string(mScore), sbX, labelY + spacing + 1);

        // Speed
        drawStat("SPEED", std::to_string(mBallSpeed), sbX, labelY + (spacing * 2) + 3);

        // Ion Spheres (display remaining)
        drawStat("ION SPHERES",
                 std::to_string(std::max(0, mIonSpheres)),
                 sbX,
                 labelY + (spacing * 3) + 4);

        // Ticker
        SDL_Rect tickerClip = {.x = 528, .y = 167, .w = 99, .h = 8};
        game->mSpriteBatch->SetClipRect(&tickerClip);

        float loopWidth = static_cast<float>(mTickerWidth) + 25.0F;
        float currentX = mTickerX;

        while (currentX < 99.0F)
        {
            if (currentX + static_cast<float>(mTickerWidth) > 0.0F)
            {
                game->mFont->DrawStringWithFontAt("Yellow Header",
                                                  game->mSpriteBatch,
                                                  mTickerText,
                                                  528 + static_cast<int>(currentX),
                                                  166,
                                                  Colors::White,
                                                  8);
            }
            currentX += loopWidth;
        }

        game->mSpriteBatch->SetClipRect(nullptr);

        // Messages
        if (game->mSpriteBatch->IsDrawBoundsEnabled())
        {
            game->mSpriteBatch->OutlineRect(&kMessageAreaRect, Colors::Green);
        }

        if (mMessageLabels.empty() || mMessageLayoutDirty)
        {
            return;
        }

        uint8_t alpha = 255;
        if (mMessageTimer > 0.0F)
        {
            if (mMessageFlash)
            {
                if (mMessageTimer > 0.75F)
                {
                    alpha = 255;
                }
                else if (mMessageTimer > 0.25F)
                {
                    alpha = static_cast<uint8_t>(((mMessageTimer - 0.25F) / 0.5F) * 255.0F);
                }
                else
                {
                    alpha = 0;
                }
            }
            else if (mMessageTimer < 0.5F)
            {
                alpha = static_cast<uint8_t>((mMessageTimer / 0.5F) * 255.0F);
            }
        }

        SDL_Color color = Colors::White;
        color.a = alpha;
        for (auto& label : mMessageLabels)
        {
            label->SetColor(color);
            label->Draw(game);
        }
    }

    void Megovision::SetTickerText(const std::string& text, Game* game)
    {
        mTickerText = text;
        int th = 0;
        game->mFont->MeasureStringWithFont("Yellow Header", mTickerText, 8, mTickerWidth, th);
        mTickerX = 99.0F;
    }

    void
    Megovision::ShowMessage(std::vector<std::unique_ptr<Label>> labels, float duration, bool flash)
    {
        mMessageLabels = std::move(labels);
        mMessageTimer = duration;
        mMessageFlash = flash;
        mMessageLayoutDirty = true;
    }

    void Megovision::ShowMessage(std::unique_ptr<Label> label, float duration, bool flash)
    {
        std::vector<std::unique_ptr<Label>> labels;
        labels.push_back(std::move(label));
        ShowMessage(std::move(labels), duration, flash);
    }
} // namespace nuvelocity::frs42
