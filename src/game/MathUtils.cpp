#include "MathUtils.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nuvelocity::frs42
{
    bool MathUtils::IsPointInPolygon(const SDL_FPoint& p, const SDL_FPoint* vertices, size_t count)
    {
        bool inside = false;
        for (size_t i = 0, j = count - 1; i < count; j = i++)
        {
            if (((vertices[i].y > p.y) != (vertices[j].y > p.y)) &&
                (p.x < (vertices[j].x - vertices[i].x) * (p.y - vertices[i].y) /
                               (vertices[j].y - vertices[i].y) +
                           vertices[i].x))
            {
                inside = !inside;
            }
        }
        return inside;
    }

    float
    MathUtils::DistanceSqToSegment(const SDL_FPoint& p, const SDL_FPoint& a, const SDL_FPoint& b)
    {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float lenSq = dx * dx + dy * dy;
        if (lenSq < 0.0001f)
        {
            float dax = p.x - a.x;
            float day = p.y - a.y;
            return dax * dax + day * day;
        }

        float t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / lenSq;
        t = std::max(0.0f, std::min(1.0f, t));

        float closestX = a.x + t * dx;
        float closestY = a.y + t * dy;

        float distDx = p.x - closestX;
        float distDy = p.y - closestY;
        return distDx * distDx + distDy * distDy;
    }

    float MathUtils::DistanceToPolygon(const SDL_FPoint& p,
                                       const SDL_FPoint* vertices,
                                       size_t count,
                                       bool closed)
    {
        if (count == 0)
            return std::numeric_limits<float>::max();

        float minDistanceSq = std::numeric_limits<float>::max();
        const size_t segmentCount = closed ? count : (count - 1);

        for (size_t i = 0; i < segmentCount; ++i)
        {
            size_t next = (i + 1) % count;
            float dSq = DistanceSqToSegment(p, vertices[i], vertices[next]);
            if (dSq < minDistanceSq)
            {
                minDistanceSq = dSq;
            }
        }

        return std::sqrt(minDistanceSq);
    }

    bool
    MathUtils::CircleIntersectsRect(const SDL_FPoint& center, float radius, const SDL_FRect& rect)
    {
        // Find the closest point to the circle within the rectangle
        float closestX = std::max(rect.x, std::min(center.x, rect.x + rect.w));
        float closestY = std::max(rect.y, std::min(center.y, rect.y + rect.h));

        float dx = center.x - closestX;
        float dy = center.y - closestY;

        return (dx * dx + dy * dy) < (radius * radius);
    }

    bool MathUtils::CircleToSegmentCollision(const SDL_FPoint& center,
                                             float radius,
                                             const SDL_FPoint& p1,
                                             const SDL_FPoint& p2,
                                             SDL_FPoint& outNormal,
                                             float& outPenetration)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float lenSq = dx * dx + dy * dy;
        if (lenSq < 0.0001f)
            return false;

        float t = ((center.x - p1.x) * dx + (center.y - p1.y) * dy) / lenSq;
        t = std::max(0.0f, std::min(1.0f, t));

        float closestX = p1.x + t * dx;
        float closestY = p1.y + t * dy;

        float distDx = center.x - closestX;
        float distDy = center.y - closestY;
        float distSq = distDx * distDx + distDy * distDy;

        if (distSq < radius * radius)
        {
            float dist = std::sqrt(distSq);
            outPenetration = radius - dist;
            if (dist > 0.0001f)
            {
                outNormal.x = distDx / dist;
                outNormal.y = distDy / dist;
            }
            else
            {
                // Degenerate case: center exactly on segment, use segment perpendicular
                outNormal.x = -dy / std::sqrt(lenSq);
                outNormal.y = dx / std::sqrt(lenSq);
            }
            return true;
        }
        return false;
    }

    std::optional<SDL_FPoint>
    MathUtils::GetRandomPointInPolygon(std::mt19937& gen,
                                       const SDL_FPoint* vertices,
                                       size_t count,
                                       const SDL_FRect& samplingBounds,
                                       float radius,
                                       const std::vector<SDL_FRect>& excludeRects,
                                       int maxAttempts)
    {
        std::uniform_real_distribution<float> disX(samplingBounds.x,
                                                   samplingBounds.x + samplingBounds.w);
        std::uniform_real_distribution<float> disY(samplingBounds.y,
                                                   samplingBounds.y + samplingBounds.h);

        for (int attempt = 0; attempt < maxAttempts; ++attempt)
        {
            SDL_FPoint candidate{disX(gen), disY(gen)};

            // 1. Center must be inside the polygon
            if (!IsPointInPolygon(candidate, vertices, count))
                continue;

            // 2. Entire ball must be inside (distance to boundary >= radius)
            if (radius > 0.0f && DistanceToPolygon(candidate, vertices, count) < radius)
                continue;

            // 3. Must not overlap any exclusion zones
            bool excluded = false;
            for (const auto& rect : excludeRects)
            {
                if (CircleIntersectsRect(candidate, radius, rect))
                {
                    excluded = true;
                    break;
                }
            }

            if (!excluded)
            {
                return candidate;
            }
        }

        return std::nullopt;
    }
} // namespace nuvelocity::frs42
