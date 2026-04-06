#ifndef NVE_BRICK_INFO_H
#define NVE_BRICK_INFO_H

#include "BrickType.h"
#include "model/Model.h"
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <sstream>

namespace nuvelocity::frs42
{
    class BrickInfo : public Object<BrickInfo>
    {
    public:
        BrickInfo() = default;
        ~BrickInfo() = default;

        static void InitClassInfo(ClassInfo& aInfo)
        {
            aInfo.mName = "CBrickInfo";
            AddProperty(aInfo, "Brick Type", &BrickInfo::mBrickTypeStr);
            AddProperty(aInfo, "Primary Sequence", &BrickInfo::mPrimarySequencePath);
            AddProperty(aInfo, "Sequence 2", &BrickInfo::mSequence2Path);
            AddProperty(aInfo, "Sequence 3", &BrickInfo::mSequence3Path);
            AddProperty(aInfo, "Destroyed Seq", &BrickInfo::mDestroyedSeqPath);
            AddProperty(aInfo, "Destroyed Sound", &BrickInfo::mDestroyedSoundPath);
            AddProperty(aInfo, "Indestructible Sound", &BrickInfo::mIndestructibleSoundPath);
            AddProperty(aInfo, "Damaged Sound", &BrickInfo::mDamagedSoundPath);
            AddProperty(aInfo, "Score Value", &BrickInfo::mScoreValue);
            AddProperty(aInfo, "Break Particle Gen", &BrickInfo::mBreakParticleGenPath);
            AddProperty(aInfo, "Break Particle Types", &BrickInfo::mBreakParticleTypes, "Type");
            AddProperty(aInfo, "Collision Polygon", &BrickInfo::mCollisionPolygonStr);
        }

        BrickType GetBrickType() const { return StringToBrickType(mBrickTypeStr); }
        const std::string& GetPrimarySequencePath() const { return mPrimarySequencePath; }
        const std::string& GetSequence2Path() const { return mSequence2Path; }
        const std::string& GetSequence3Path() const { return mSequence3Path; }
        const std::string& GetDestroyedSeqPath() const { return mDestroyedSeqPath; }
        const std::string& GetDestroyedSoundPath() const { return mDestroyedSoundPath; }
        const std::string& GetIndestructibleSoundPath() const { return mIndestructibleSoundPath; }
        const std::string& GetDamagedSoundPath() const { return mDamagedSoundPath; }
        int GetScoreValue() const { return mScoreValue; }
        const std::string& GetBreakParticleGenPath() const { return mBreakParticleGenPath; }
        const std::vector<std::string>& GetBreakParticleTypes() const { return mBreakParticleTypes; }

        void SetCollisionPolygonStr(const std::string& poly) { mCollisionPolygonStr = poly; }
        
        std::vector<SDL_FPoint> GetCollisionPolygon() const
        {
            std::vector<SDL_FPoint> points;
            std::stringstream ss(mCollisionPolygonStr);
            std::string segment;
            std::vector<float> coords;

            while (std::getline(ss, segment, ','))
            {
                try {
                    coords.push_back(std::stof(segment));
                } catch (...) {
                    // Ignore non-numeric segments
                }
            }

            for (size_t i = 0; i + 1 < coords.size(); i += 2)
            {
                points.push_back(SDL_FPoint{coords[i], coords[i+1]});
            }

            return points;
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
        std::vector<std::string> mBreakParticleTypes;
        std::string mCollisionPolygonStr = "";
    };
} // namespace nuvelocity::frs42

#endif // NVE_BRICK_INFO_H
