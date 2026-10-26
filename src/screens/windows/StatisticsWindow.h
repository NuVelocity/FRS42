#ifndef FRS42_STATISTICS_WINDOW_H
#define FRS42_STATISTICS_WINDOW_H

#include <array>
#include <memory>
#include <string>
#include <system/ui/JListBox.h>
#include <system/ui/JTabControl.h>
#include <system/ui/MdiWindow.h>
#include <vector>

namespace nuvelocity
{
    class Game;
    class Button;
    class Label;
} // namespace nuvelocity

namespace nuvelocity::frs42
{
    class StatisticsWindow : public MdiWindow
    {
    public:
        StatisticsWindow(Game* game);
        StatisticsWindow(Game* game, int initialTabIndex);
        StatisticsWindow(Game* game,
                         int initialTabIndex,
                         const std::string& listFooterText,
                         int selectedIndex);
        virtual ~StatisticsWindow() = default;

        void Update(Game* game) override;
        void Draw(Game* game) override;

    private:
        void InitializeTabs(Game* game);
        void PopulateData();
        void ApplyListLayout(Game* game);

        void OnTabChanged(int index);
        void UpdatePlayerButtons();
        void OnEditPlayer(Game* game);
        void OnDeletePlayer(Game* game);
        void OnDeleteConfirmed(Game* game, int playerIndex);

        std::shared_ptr<JTabControl> mTabControl;
        std::array<std::shared_ptr<JListBox>, 6> mLists;
        std::shared_ptr<Label> mListFooter;
        std::string mListFooterText;
        int mListSelectedIndex = -1;

        std::shared_ptr<Button> mEditPlayerButton;
        std::shared_ptr<Button> mDeletePlayerButton;
        std::shared_ptr<Button> mCloseButton;
        int mInitialTabIndex = 0;
    };
} // namespace nuvelocity::frs42

#endif // FRS42_STATISTICS_WINDOW_H
