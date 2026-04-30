#ifndef FRS42_ARENA_SCENE_H
#define FRS42_ARENA_SCENE_H

#include "Megovision.h"
#include "Playfield.h"
#include "RoundSet.h"
#include <Scene.h>
#include <string>
#include <system/ui/Label.h>

namespace nuvelocity::frs42
{
    class BrickLayout;
    class BrickInfo;
    class ArenaScene : public Scene
    {
    public:
        ArenaScene(std::string roundSetName, int roundIndex);
        ~ArenaScene() override = default;

        void Load(Game* game) override;
        void Unload(Game* game) override;
        void Update(Game* game) override;
        void Draw(Game* game) override;
        std::string GetName() const override
        {
            return "ArenaScene";
        }

    private:
        std::string mRoundSetName;
        uint8_t mRoundIndex;

        RoundSet* mRoundSet = nullptr;
        Playfield mPlayfield;
        Megovision mMegovision;

        SDL_Rect mPlayfieldRect;
        StandAloneFrame* mGameOverFrame;
        std::string mCheatBuffer;
        bool mIsPaused = false;

        bool LoadRoundSet(Game* game);
        BrickLayout* LoadBrickLayout(Game* game);
        void PopulateBricks(Game* game, BrickLayout* layout);
        void BuildLevelUI(Game* game, BrickLayout* layout);
        BrickInfo* GetOrLoadBrickInfo(Game* game, const std::string& path);
        void ShowPauseMenu(Game* game);
        void ShowOptionsDialog(Game* game);
    };
} // namespace nuvelocity::frs42

#endif // FRS42_ARENA_SCENE_H
