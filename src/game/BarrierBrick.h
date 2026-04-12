#ifndef NVE_BARRIER_BRICK_H
#define NVE_BARRIER_BRICK_H

#include "Brick.h"
#include <vector>

namespace nuvelocity::frs42
{
    class Ball;

    class BarrierBrick : public Brick
    {
    public:
        BarrierBrick() = default;

        BarrierBrick(const SDL_FPoint& p1, const SDL_FPoint& p2)
        {
            SetLine(p1, p2);
        }

        BarrierBrick(std::vector<SDL_FPoint> vertices)
                : mVertices(std::move(vertices))
        {
        }

        virtual ~BarrierBrick() = default;

        void AttachBrickInfo(Game* game, const BrickInfo& info) override
        {
            Brick::AttachBrickInfo(game, info);
            if (mVertices.empty())
            {
                mVertices = info.GetCollisionPolygon();
            }
        }

        void SetLine(const SDL_FPoint& p1, const SDL_FPoint& p2)
        {
            mVertices = {p1, p2};
        }

        void Update(Game* aGame) override;
        void Draw(Game* aGame) override;

        std::vector<SDL_FPoint> GetCollisionPolygon() const override
        {
            return mVertices;
        }
        bool IsClosedPolygon() const override
        {
            return mVertices.size() > 2;
        }

        bool Intersects(const SDL_FPoint& point); // non-const: updates mHoveredSegment
        void ApplyAttraction(Game* aGame, Ball* ball, const SDL_FPoint& mousePos) const;

        void Hit() override {} // BarrierBrick is indestructible — never mark as destroyed

        // SetHovered(false) clears the highlight; SetHovered(true) is a no-op
        // (Intersects already wrote the correct segment index)
        void SetHovered(bool hovered)
        {
            if (!hovered)
                mHoveredSegment = -1;
        }
        bool IsHovered() const
        {
            return mHoveredSegment >= 0;
        }

        void SetShowHoverEffect(bool show)
        {
            mShowHoverEffect = show;
        }
        void SetHoverColor(SDL_Color color)
        {
            mHoverColor = color;
        }

        void SetAttractionEnabled(bool enabled)
        {
            mAttractionEnabled = enabled;
        }
        bool IsAttractionEnabled() const
        {
            return mAttractionEnabled;
        }

    private:
        SDL_FPoint mMousePosition;
        std::vector<SDL_FPoint> mVertices;
        int mHoveredSegment = -1; // index of hovered edge, -1 = none
        bool mShowHoverEffect = true;
        SDL_Color mHoverColor = {255, 255, 255, 255};
        bool mAttractionEnabled = false;
    };
} // namespace nuvelocity::frs42

#endif // NVE_BARRIER_BRICK_H
