#ifndef NVE_MENU_BARRIER_H
#define NVE_MENU_BARRIER_H

#include "Brick.h"
#include <vector>

namespace nuvelocity::frs42
{
    class Ball;

    class MenuBarrier : public Brick
    {
    public:
        MenuBarrier() = default;

        MenuBarrier(const SDL_FPoint& p1, const SDL_FPoint& p2)
        {
            SetLine(p1, p2);
        }

        MenuBarrier(std::vector<SDL_FPoint> vertices)
                : mVertices(std::move(vertices))
        {
        }

        virtual ~MenuBarrier() = default;

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
        void ApplyGravityEffect(Game* aGame, Ball* ball, const SDL_FPoint& mousePos) const;

        void Hit() override {} // MenuBarrier is indestructible — never mark as destroyed

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

    private:
        SDL_FPoint mMousePosition;
        std::vector<SDL_FPoint> mVertices;
        int mHoveredSegment = -1; // index of hovered edge, -1 = none
        bool mShowHoverEffect = true;
        SDL_Color mHoverColor = {255, 255, 255, 255};
    };
} // namespace nuvelocity::frs42

#endif // NVE_MENU_BARRIER_H
