#ifndef FRS42_LEVEL_SELECT_WINDOW_H
#define FRS42_LEVEL_SELECT_WINDOW_H

#include <system/ui/Button.h>
#include <system/ui/JListBox.h>
#include <system/ui/MdiWindow.h>

namespace nuvelocity::frs42
{
    class LevelSelectWindow : public nuvelocity::MdiWindow
    {
    public:
        LevelSelectWindow(Game* game);

    private:
        struct LevelEntry
        {
            std::string roundSetPath;
            std::string roundSetName;
            std::string roundName;
            int roundIndex;
        };

        void PopulateList(Game* game);
        void OnGoClick(Game* game);

        std::shared_ptr<nuvelocity::JListBox> mListView;
        std::vector<LevelEntry> mLevelEntries;
    };
} // namespace nuvelocity::frs42

#endif // FRS42_LEVEL_SELECT_WINDOW_H
