#include <Scene.h>

namespace nuvelocity::frs42
{
    class MainMenuScene : public Scene
    {
    private:
        StandAloneFrame* mBackgroundFrame;
        
    public:
        MainMenuScene();
        void Load(Game* aGame) override;
        void Update(Game* aGame) override;
        void Draw(Game* aGame) override;
        void Unload(Game* aGame) override {};
        std::string GetName() const override;
    };
} // namespace nuvelocity::frs42
