#ifndef NVE_BRICK_INFO_H
#define NVE_BRICK_INFO_H

#include "BrickType.h"
#include "ParticleType.h"
#include <Object.h>
#include <SDL3/SDL.h>
#include <sstream>
#include <string>
#include <vector>

namespace nuvelocity
{
    class ParticleGeneratorInfo;
}

namespace nuvelocity::frs42
{
    class BrickInfo : public Object<BrickInfo>
    {
    public:
        BrickInfo() = default;
        ~BrickInfo() override = default;

        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CBrickInfo";
            AddProperty(info, "Brick Type", &BrickInfo::mBrickTypeStr);
            AddProperty(info, "Primary Sequence", &BrickInfo::mPrimarySequencePath);
            AddProperty(info, "Sequence 2", &BrickInfo::mSequence2Path);
            AddProperty(info, "Sequence 3", &BrickInfo::mSequence3Path);
            AddProperty(info, "Destroyed Seq", &BrickInfo::mDestroyedSeqPath);
            AddProperty(info, "Destroyed Sound", &BrickInfo::mDestroyedSoundPath);
            AddProperty(info, "Indestructible Sound", &BrickInfo::mIndestructibleSoundPath);
            AddProperty(info, "Damaged Sound", &BrickInfo::mDamagedSoundPath);
            AddProperty(info, "Score Value", &BrickInfo::mScoreValue);
            AddProperty(info, "Break Particle Gen", &BrickInfo::mBreakParticleGenPath);
            AddProperty(info, "Break Particle Types", &BrickInfo::mBreakParticleTypes);
            AddPolygonProperty(info, "Collision Polygon", &BrickInfo::mCollisionPolygon);
        }

        BrickType GetBrickType() const
        {
            return StringToBrickType(mBrickTypeStr);
        }
        const std::string& GetPrimarySequencePath() const
        {
            return mPrimarySequencePath;
        }
        const std::string& GetSequence2Path() const
        {
            return mSequence2Path;
        }
        const std::string& GetSequence3Path() const
        {
            return mSequence3Path;
        }
        const std::string& GetDestroyedSeqPath() const
        {
            return mDestroyedSeqPath;
        }
        const std::string& GetDestroyedSoundPath() const
        {
            return mDestroyedSoundPath;
        }
        const std::string& GetIndestructibleSoundPath() const
        {
            return mIndestructibleSoundPath;
        }
        const std::string& GetDamagedSoundPath() const
        {
            return mDamagedSoundPath;
        }
        int GetScoreValue() const
        {
            return mScoreValue;
        }
        const std::string& GetBreakParticleGenPath() const
        {
            return mBreakParticleGenPath;
        }
        const std::vector<ParticleType*>& GetBreakParticleTypes() const
        {
            return mBreakParticleTypes;
        }

        const nuvelocity::ParticleGeneratorInfo* GetBreakParticleGen() const
        {
            return mBreakParticleGen;
        }

        void SetBreakParticleGen(const nuvelocity::ParticleGeneratorInfo* info)
        {
            mBreakParticleGen = info;
        }

        void SetCollisionPolygon(const std::string& csv)
        {
            ParseCSVToPoints(csv, mCollisionPolygon);
        }

        const std::vector<SDL_FPoint>& GetCollisionPolygon() const
        {
            return mCollisionPolygon;
        }

    private:
        std::string mBrickTypeStr = "Normal";
        std::string mPrimarySequencePath = "!None";
        std::string mSequence2Path = "!None";
        std::string mSequence3Path = "!None";
        std::string mDestroyedSeqPath = "!None";
        std::string mDestroyedSoundPath = "!None";
        std::string mIndestructibleSoundPath = "!None";
        std::string mDamagedSoundPath = "!None";
        int mScoreValue = 0;
        std::string mBreakParticleGenPath = "!None";
        std::vector<ParticleType*> mBreakParticleTypes;
        const nuvelocity::ParticleGeneratorInfo* mBreakParticleGen = nullptr;
        std::vector<SDL_FPoint> mCollisionPolygon = {{-15.75F, -8.75F},
                                                     {15.75F, -8.75F},
                                                     {15.75F, 8.75F},
                                                     {-15.75F, 8.75F},
                                                     {-15.75F, -8.75F}};
    };
} // namespace nuvelocity::frs42

#endif // NVE_BRICK_INFO_H
