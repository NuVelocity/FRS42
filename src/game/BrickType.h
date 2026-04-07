#ifndef NVE_BRICK_TYPE_H
#define NVE_BRICK_TYPE_H

#include <string>
#include <vector>

namespace nuvelocity::frs42
{
    enum class BrickType
    {
        Normal,
        PowerUp,
        Obstacle,
        Indestructible,
        IndestructibleTop,
        IndestructibleBottom,
        Exploding,
        ThreeHit,
        ThreeHitBottom,
        ThreeHitTop,
        OffOn,
        OnOff,
        Detinator,
        PushAway,
        NudgeUp,
        NudgeDown,
        NudgeLeft,
        NudgeRight,
        BrickShredder,
        TrappedBall,
        Unknown
    };

    inline BrickType StringToBrickType(const std::string& typeStr)
    {
        if (typeStr == "Normal")
            return BrickType::Normal;
        if (typeStr == "Power Up")
            return BrickType::PowerUp;
        if (typeStr == "Obstacle")
            return BrickType::Obstacle;
        if (typeStr == "Indestructible")
            return BrickType::Indestructible;
        if (typeStr == "Indestructible Top")
            return BrickType::IndestructibleTop;
        if (typeStr == "Indestructible Bottom")
            return BrickType::IndestructibleBottom;
        if (typeStr == "Exploding")
            return BrickType::Exploding;
        if (typeStr == "3 Hit")
            return BrickType::ThreeHit;
        if (typeStr == "3 Hit Bottom")
            return BrickType::ThreeHitBottom;
        if (typeStr == "3 Hit Top")
            return BrickType::ThreeHitTop;
        if (typeStr == "Off/On")
            return BrickType::OffOn;
        if (typeStr == "On/Off")
            return BrickType::OnOff;
        if (typeStr == "Detinator")
            return BrickType::Detinator;
        if (typeStr == "Push Away")
            return BrickType::PushAway;
        if (typeStr == "Nudge Up")
            return BrickType::NudgeUp;
        if (typeStr == "Nudge Down")
            return BrickType::NudgeDown;
        if (typeStr == "Nudge Left")
            return BrickType::NudgeLeft;
        if (typeStr == "Nudge Right")
            return BrickType::NudgeRight;
        if (typeStr == "Brick Shredder")
            return BrickType::BrickShredder;
        if (typeStr == "Trapped Ball")
            return BrickType::TrappedBall;
        return BrickType::Unknown;
    }

    inline std::string BrickTypeToString(BrickType type)
    {
        switch (type)
        {
        case BrickType::Normal:
            return "Normal";
        case BrickType::PowerUp:
            return "Power Up";
        case BrickType::Obstacle:
            return "Obstacle";
        case BrickType::Indestructible:
            return "Indestructible";
        case BrickType::IndestructibleTop:
            return "Indestructible Top";
        case BrickType::IndestructibleBottom:
            return "Indestructible Bottom";
        case BrickType::Exploding:
            return "Exploding";
        case BrickType::ThreeHit:
            return "3 Hit";
        case BrickType::ThreeHitBottom:
            return "3 Hit Bottom";
        case BrickType::ThreeHitTop:
            return "3 Hit Top";
        case BrickType::OffOn:
            return "Off/On";
        case BrickType::OnOff:
            return "On/Off";
        case BrickType::Detinator:
            return "Detinator";
        case BrickType::PushAway:
            return "Push Away";
        case BrickType::NudgeUp:
            return "Nudge Up";
        case BrickType::NudgeDown:
            return "Nudge Down";
        case BrickType::NudgeLeft:
            return "Nudge Left";
        case BrickType::NudgeRight:
            return "Nudge Right";
        case BrickType::BrickShredder:
            return "Brick Shredder";
        case BrickType::TrappedBall:
            return "Trapped Ball";
        default:
            return "Unknown";
        }
    }
} // namespace nuvelocity::frs42

#endif // NVE_BRICK_TYPE_H
