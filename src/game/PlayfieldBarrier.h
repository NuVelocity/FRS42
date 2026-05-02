#ifndef NVE_BARRIER_BRICK_H
#define NVE_BARRIER_BRICK_H

#include "Brick.h"
#include "Collidable2D.h"

#include <Colors.h>
#include <vector>

namespace nuvelocity::frs42
{
    class Ball;

    class PlayfieldBarrier : public Collidable2D
    {
    public:
        PlayfieldBarrier();

        PlayfieldBarrier(const SDL_FPoint& p1, const SDL_FPoint& p2)
                : PlayfieldBarrier()
        {
            mCollisionPolygon = {p1, p2};
        }

        PlayfieldBarrier(std::vector<SDL_FPoint> vertices)
                : PlayfieldBarrier()
        {
            mCollisionPolygon = std::move(vertices);
        }

        ~PlayfieldBarrier() override = default;

        void Update(Game* game) override;

        void Draw(Game* game) override;

        bool IntersectsMouse(const SDL_Point& point);

        void ApplyAttraction(Game* game, Ball* ball, const SDL_FPoint& mousePos) const;

        void OnHit(Game* game,
                   const SDL_Rect& bounds,
                   bool hitFromTop = false,
                   bool ignoreDirection = true) override;

        void SetHovered(const bool hovered)
        {
            if (!hovered)
            {
                mHoveredSegment = -1;
            }
        }

        bool IsHovered() const
        {
            return mHoveredSegment >= 0;
        }

        void SetShowHoverEffect(const bool show)
        {
            mShowHoverEffect = show;
        }

        void SetHoverColor(const SDL_Color color)
        {
            mHoverColor = color;
        }

        void SetAttractionEnabled(const bool enabled)
        {
            mAttractionEnabled = enabled;
        }

        bool IsAttractionEnabled() const
        {
            return mAttractionEnabled;
        }

        std::vector<SDL_FPoint> GetCollisionPolygon() const override
        {
            return mCollisionPolygon;
        }

    private:
        std::vector<SDL_FPoint> mCollisionPolygon;
        SDL_Point mMousePosition;
        int mHoveredSegment;
        bool mShowHoverEffect;
        SDL_Color mHoverColor;
        bool mAttractionEnabled;
    };
} // namespace nuvelocity::frs42

#endif // NVE_BARRIER_BRICK_H
