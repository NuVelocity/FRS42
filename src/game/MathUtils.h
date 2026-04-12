#ifndef NVE_MATH_UTILS_H
#define NVE_MATH_UTILS_H

#include <SDL3/SDL.h>
#include <optional>
#include <random>
#include <vector>

namespace nuvelocity::frs42
{
    /**
     * @brief Collection of static geometry and collision utility functions.
     */
    class MathUtils
    {
    public:
        /**
         * @brief Checks if a point is inside a polygon using ray casting.
         */
        static bool IsPointInPolygon(const SDL_FPoint& p, const SDL_FPoint* vertices, size_t count);

        /**
         * @brief Calculates the squared distance from a point to a line segment.
         */
        static float
        DistanceSqToSegment(const SDL_FPoint& p, const SDL_FPoint& a, const SDL_FPoint& b);

        /**
         * @brief Calculates the minimum distance from a point to the boundary of a polygon.
         * @param closed If true, the last vertex is connected back to the first.
         */
        static float DistanceToPolygon(const SDL_FPoint& p,
                                       const SDL_FPoint* vertices,
                                       size_t count,
                                       bool closed = true);

        /**
         * @brief Checks if a circle intersects an axis-aligned rectangle.
         */
        static bool
        CircleIntersectsRect(const SDL_FPoint& center, float radius, const SDL_FRect& rect);

        /**
         * @brief Performs collision detection/resolution between a circle and a line segment.
         * @return true if colliding.
         */
        static bool CircleToSegmentCollision(const SDL_FPoint& center,
                                             float radius,
                                             const SDL_FPoint& p1,
                                             const SDL_FPoint& p2,
                                             SDL_FPoint& outNormal,
                                             float& outPenetration);

        /**
         * @brief Attempts to find a random point inside a polygon that can fit a ball of given
         * radius.
         * @param samplingBounds The axis-aligned bounding box to sample points from.
         * @param radius The radius of the object being spawned (must be fully inside).
         * @param excludeRects A list of rectangles to avoid.
         * @return A valid point or std::nullopt if none found after maxAttempts.
         */
        static std::optional<SDL_FPoint>
        GetRandomPointInPolygon(std::mt19937& gen,
                                const SDL_FPoint* vertices,
                                size_t count,
                                const SDL_FRect& samplingBounds,
                                float radius = 0.0f,
                                const std::vector<SDL_FRect>& excludeRects = {},
                                int maxAttempts = 100);
    };
} // namespace nuvelocity::frs42

#endif // NVE_MATH_UTILS_H
