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
        MenuBarrier(const BrickInfo& info)
                : Brick(info)
        {
            mVertices = info.GetCollisionPolygon();
            // Force type to Indestructible
            // no sequence to load, Brick constructor already loaded mSequence based on info
        }

        MenuBarrier(const BrickInfo& info, const SDL_FPoint& p1, const SDL_FPoint& p2)
                : Brick(info)
        {
            SetLine(p1, p2);
        }

        MenuBarrier(const BrickInfo& info, std::vector<SDL_FPoint> vertices)
                : Brick(info)
        {
            mVertices = std::move(vertices);
        }

        virtual ~MenuBarrier() = default;

        void SetLine(const SDL_FPoint& p1, const SDL_FPoint& p2)
        {
            mVertices = {p1, p2};
        }

        void Update(float deltaTime) override;
        void Draw(Game* game) const override;

        std::vector<SDL_FPoint> GetCollisionPolygon() const override
        {
            return mVertices;
        }
        bool IsClosedPolygon() const override
        {
            return mVertices.size() > 2;
        }

        bool Intersects(const SDL_FPoint& point); // non-const: updates mHoveredSegment
        void AttractBall(Ball* ball) const;

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

        void OnClick(const std::vector<Ball*>& balls) const;

    private:
        std::vector<SDL_FPoint> mVertices;
        int mHoveredSegment = -1; // index of hovered edge, -1 = none
        bool mShowHoverEffect = true;
        SDL_Color mHoverColor = {255, 255, 255, 255};
    };
} // namespace nuvelocity::frs42

#endif // NVE_MENU_BARRIER_H
