#include "PowerUp.h"
#include <Frame.h>
#include <Game.h>
#include <Sequence.h>
#include <cmath>
#include <random>
#include <system/SpriteBatch.h>

namespace nuvelocity::frs42
{
    PowerUp::PowerUp(PowerUpType type, const SDL_FPoint& pos, Sequence* seq)
            : mType(type)
            , mPosition(pos)
            , mSequence(seq)
    {
        mVelocity = {0.0F, 100.0F}; // Slow fall
        mStartTick = SDL_GetTicks();
    }

    void PowerUp::Update(Game* game)
    {
        float dt = game->GetDeltaTime();
        mPosition.y += mVelocity.y * dt;

        if (mPosition.y > static_cast<float>(game->mWindowHeight) + 50.0F)
        {
            mIsDead = true;
        }
    }

    void PowerUp::Draw(Game* game)
    {
        if (mSequence == nullptr || game == nullptr || game->mSpriteBatch == nullptr)
        {
            return;
        }

        const uint64_t now = SDL_GetTicks();
        const uint64_t elapsed = now - mStartTick;
        const float fps = mSequence->GetFramesPerSecond();
        const std::size_t frameCount = mSequence->GetFrameCount();
        if (frameCount == 0)
        {
            return;
        }

        const std::size_t frameIndex =
            static_cast<std::size_t>((static_cast<double>(elapsed) * fps) / 1000.0) % frameCount;

        if (const Frame* frame = mSequence->GetFrame(frameIndex); frame != nullptr)
        {
            SDL_Surface* surface = frame->GetSurface();
            SDL_Rect destRect{
                .x = static_cast<int>(std::lround(mPosition.x)) + frame->GetHotSpot().x,
                .y = static_cast<int>(std::lround(mPosition.y)) + frame->GetHotSpot().y,
                .w = surface->w,
                .h = surface->h};
            game->mSpriteBatch->Draw(surface, &destRect);
        }
    }

    std::string PowerUp::GetSoundPath(PowerUpType type)
    {
        switch (type)
        {
        case PowerUpType::Slow:
            return "Power Ups/Slow.ogg";
        case PowerUpType::ExpandPaddle:
            return "Power Ups/Snap On Larger Shiled.ogg";
        case PowerUpType::Catch:
            return "Power Ups/Catcher.ogg";
        case PowerUpType::FireBall:
            return "Power Ups/Fire Ball.ogg";
        case PowerUpType::Multiply3:
            return "Power Ups/Multiply 3.ogg";
        case PowerUpType::Multiply8:
            return "Power Ups/Multiply 8.ogg";
        case PowerUpType::Gun:
            return "Power Ups/Gun Deploy.ogg";
        case PowerUpType::BigGun:
            return "Power Ups/Big Gun.ogg";
        case PowerUpType::ExtraBall:
            return "Power Ups/Extra Ball.ogg";
        case PowerUpType::RailBall:
            return "Power Ups/Rail Ball.ogg";
        case PowerUpType::NormalBall:
            return "Power Ups/Normal Ball.ogg";
        case PowerUpType::SmallBall:
            return "Power Ups/Small Ball.ogg";
        case PowerUpType::ShrinkPaddle:
            return "Power Ups/Snap On Smaller Shiled.ogg";
        case PowerUpType::Fast:
            return "Power Ups/Fast.ogg";
        case PowerUpType::TheBomb:
            // This "power-up" cannot be picked-up.
            break;
        default:
            break;
        }
        return "";
    }

    std::string PowerUp::GetSequencePath(PowerUpType type)
    {
        switch (type)
        {
        case PowerUpType::Slow:
            return "Resources/Power Ups/Slow";
        case PowerUpType::ExpandPaddle:
            return "Resources/Power Ups/Expand";
        case PowerUpType::Catch:
            return "Resources/Power Ups/Catcher";
        case PowerUpType::FireBall:
            return "Resources/Power Ups/Fire Ball";
        case PowerUpType::Multiply3:
            return "Resources/Power Ups/Multiply 3";
        case PowerUpType::Multiply8:
            return "Resources/Power Ups/Multiply 8";
        case PowerUpType::Gun:
            return "Resources/Power Ups/Gun";
        case PowerUpType::BigGun:
            return "Resources/Power Ups/Big Gun";
        case PowerUpType::ExtraBall:
            return "Resources/Power Ups/Extra Ball";
        case PowerUpType::RailBall:
            return "Resources/Power Ups/Rail Ball";
        case PowerUpType::NormalBall:
            return "Resources/Power Ups/Normal Ball";
        case PowerUpType::SmallBall:
            return "Resources/Power Ups/Small Ball";
        case PowerUpType::ShrinkPaddle:
            return "Resources/Power Ups/Shrink";
        case PowerUpType::Fast:
            return "Resources/Power Ups/Fast";
        case PowerUpType::TheBomb:
            return "Resources/Power Ups/Bomb";
        default:
            break;
        }
        return "";
    }

    std::string PowerUp::GetName(PowerUpType type)
    {
        switch (type)
        {
        case PowerUpType::Slow:
            return "Ball Slow Down";
        case PowerUpType::ExpandPaddle:
            return "Shield Expander";
        case PowerUpType::Catch:
            return "Ball Catcher";
        case PowerUpType::FireBall:
            return "Fire Ball";
        case PowerUpType::Multiply3:
            return "3 Way Ball Split";
        case PowerUpType::Multiply8:
            return "8 Way Ball Split";
        case PowerUpType::Gun:
            return "Laser Blaster Gun";
        case PowerUpType::BigGun:
            return "Meg-O-Blaster Gun";
        case PowerUpType::ExtraBall:
            return "Extra Ion Sphere";
        case PowerUpType::RailBall:
            return "Rail Ball";
        case PowerUpType::NormalBall:
            return "Normal Ball";
        case PowerUpType::SmallBall:
            return "Small Ball";
        case PowerUpType::ShrinkPaddle:
            return "Shield Shrinker";
        case PowerUpType::Fast:
            return "Ball Speed Up";
        case PowerUpType::TheBomb:
            return "Bomb";
        }
        return "Powerup";
    }

    std::string PowerUp::GetDescription(PowerUpType type)
    {
        switch (type)
        {
        case PowerUpType::Slow:
            return "Slow down for better control.";
        case PowerUpType::ExpandPaddle:
            return "Increase the size of your ship.";
        case PowerUpType::Catch:
            return "Catch and release balls at your command.";
        case PowerUpType::FireBall:
            return "Destroy multiple bricks with one hit.";
        case PowerUpType::Multiply3:
            return "Triple your fun with three balls.";
        case PowerUpType::Multiply8:
            return "Release a swarm of eight balls.";
        case PowerUpType::Gun:
            return "Equip your ship with dual lasers.";
        case PowerUpType::BigGun:
            return "Equip your ship with massive cannons.";
        case PowerUpType::ExtraBall:
            return "Earn an extra ion sphere.";
        case PowerUpType::RailBall:
            return "Your ball cuts through all bricks.";
        case PowerUpType::NormalBall:
            return "Reset your ball to normal state.";
        case PowerUpType::SmallBall:
            return "Shrink your ball for precision.";
        case PowerUpType::ShrinkPaddle:
            return "Shrink your paddle for a challenge.";
        case PowerUpType::Fast:
            return "Speed up the action.";
        case PowerUpType::TheBomb:
            return "Destroy this menace for points.";
        }
        return "";
    }

    PowerUpType PowerUp::TypeFromString(const std::string& name)
    {
        if (name == "Slow")
            return PowerUpType::Slow;
        if (name == "Expand Paddle")
            return PowerUpType::ExpandPaddle;
        if (name == "Catch")
            return PowerUpType::Catch;
        if (name == "Fire Ball")
            return PowerUpType::FireBall;
        if (name == "Multiply 3")
            return PowerUpType::Multiply3;
        if (name == "Multiply 8")
            return PowerUpType::Multiply8;
        if (name == "Gun")
            return PowerUpType::Gun;
        if (name == "Big Gun")
            return PowerUpType::BigGun;
        if (name == "Extra Ball")
            return PowerUpType::ExtraBall;
        if (name == "Rail Ball")
            return PowerUpType::RailBall;
        if (name == "Normal Ball")
            return PowerUpType::NormalBall;
        if (name == "Small Ball")
            return PowerUpType::SmallBall;
        if (name == "Shrink Paddle")
            return PowerUpType::ShrinkPaddle;
        if (name == "Fast")
            return PowerUpType::Fast;
        if (name == "The Bomb")
            return PowerUpType::TheBomb;
        return PowerUpType::NormalBall;
    }
} // namespace nuvelocity::frs42
