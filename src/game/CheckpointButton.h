#ifndef NVE_CHECKPOINT_BUTTON_H
#define NVE_CHECKPOINT_BUTTON_H

#include <string>
#include <ui/Button.h>

namespace nuvelocity
{
    class Sequence;
    class Game;
} // namespace nuvelocity

namespace nuvelocity::frs42
{
    class CheckpointButton : public nuvelocity::Button
    {
    public:
        CheckpointButton(Sequence* sequence, int roundNumber, std::string label);
        virtual ~CheckpointButton() = default;

        void Draw(nuvelocity::Game* game) override;

        int GetRoundNumber() const
        {
            return mRoundNumber;
        }
        const std::string& GetLabel() const
        {
            return mLabel;
        }

        void SetLocked(bool locked)
        {
            mIsLocked = locked;
        }
        bool IsLocked() const
        {
            return mIsLocked;
        }

    private:
        Sequence* mSequence;
        int mRoundNumber;
        std::string mLabel;
        bool mIsLocked = false;
    };
} // namespace nuvelocity::frs42

#endif // NVE_CHECKPOINT_BUTTON_H
