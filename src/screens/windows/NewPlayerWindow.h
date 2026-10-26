#ifndef FRS42_NEW_PLAYER_WINDOW_H
#define FRS42_NEW_PLAYER_WINDOW_H

#include <functional>
#include <memory>
#include <string>
#include <system/ui/MdiWindow.h>
#include <system/ui/TextBox.h>
#include <vector>

namespace nuvelocity
{
    class Game;
    class Sequence;
} // namespace nuvelocity

namespace nuvelocity::frs42
{
    class NewPlayerWindow : public MdiWindow
    {
    public:
        NewPlayerWindow(Game* game, int playerIndex = -1, std::function<void(Game*)> onDone = {});
        virtual ~NewPlayerWindow();

        void Update(Game* game) override;
        void Draw(Game* game) override;

    private:
        void OnSave(Game* game);
        void OnCancel(Game* game);
        void ChangeShip(Game* game, int delta);

        void LoadPlayer(Game* game, int playerIndex);

        std::shared_ptr<TextBox> mNameInput;
        int mSelectedShipIndex = 0;
        std::shared_ptr<class DisplayShip> mPreviewShip;

        int mEditPlayerIndex = -1;
        std::function<void(Game*)> mOnDone;

        // Animation state
        float mAnimationTimer = 0.0F;
        int mCurrentTiltFrame = 10;
        bool mTiltIncreasing = true;
    };
} // namespace nuvelocity::frs42

#endif // FRS42_NEW_PLAYER_WINDOW_H
