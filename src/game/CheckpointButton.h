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
        CheckpointButton(Sequence* image, int roundNumber, std::string label);
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

    private:
        Sequence* mImage;
        int mRoundNumber;
        std::string mLabel;
    };
} // namespace nuvelocity::frs42

#endif // NVE_CHECKPOINT_BUTTON_H
