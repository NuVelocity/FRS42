#ifndef FRS42_MEGOVISION_H
#define FRS42_MEGOVISION_H

#include <GameComponent.h>
#include <memory>
#include <string>
#include <system/ui/Label.h>
#include <vector>

namespace nuvelocity
{
    class Game;
    class StandAloneFrame;
} // namespace nuvelocity

namespace nuvelocity::frs42
{
    class Megovision : public GameComponent
    {
    public:
        Megovision();
        virtual ~Megovision() = default;

        void Load(Game* game);
        void Update(Game* game) override;
        void Draw(Game* game) override;

        void SetScore(int score)
        {
            mScore = score;
        }
        void SetIonSpheres(int spheres)
        {
            mIonSpheres = spheres;
        }
        void SetLevelTime(float seconds)
        {
            mLevelTime = seconds;
        }
        void SetBallSpeed(int speed)
        {
            mBallSpeed = static_cast<int>(static_cast<float>(speed) / 3.4F);
        }
        void SetTickerText(const std::string& text, Game* game);

        void
        ShowMessage(std::vector<std::unique_ptr<Label>> labels, float duration, bool flash = false);
        void ShowMessage(const std::vector<std::string>& lines, float duration, bool flash = false);

        void ForceAdTransition()
        {
            if (!mIsTransitioning && !mAdverts.empty())
            {
                mIsTransitioning = true;
                mNextAdIndex = (mCurrentAdIndex + 1) % mAdverts.size();
                mAdTransitionProgress = 0.0f;
            }
        }

    private:
        StandAloneFrame* mScoreBoardFrame = nullptr;

        int mScore = 0;
        int mIonSpheres = 0;
        float mLevelTime = 0.0f;
        int mBallSpeed = 0;

        float mTickerX = 99.0f;
        int mTickerWidth = 0;
        std::string mTickerText;

        std::vector<StandAloneFrame*> mAdverts;
        int mCurrentAdIndex = 0;
        int mNextAdIndex = -1;
        float mAdTimer = 5.0f;
        float mAdTransitionProgress = 0.0f;
        bool mIsTransitioning = false;

        float mAdDisplayDuration = 6.0f;
        float mAdTransitionDuration = 1.0f;

        std::vector<std::unique_ptr<Label>> mMessageLabels;
        float mMessageTimer = 0.0f;
        bool mMessageFlash = false;
    };
} // namespace nuvelocity::frs42

#endif // FRS42_MEGOVISION_H
