#ifndef FRS42_SPLASH_SCENE_H
#define FRS42_SPLASH_SCENE_H

#include <Scene.h>

#include <array>
#include <cstddef>
#include <cstdint>

namespace nuvelocity
{
    class StandAloneFrame;
}

namespace nuvelocity::frs42
{
    class SplashScene : public Scene
    {
    private:
        struct SplashFrame
        {
            const char* assetPath = nullptr;
            StandAloneFrame* frame = nullptr;
        };

        static constexpr std::size_t kFrameCount = 2;
        static constexpr uint64_t kFadeDurationMs = 600;
        static constexpr uint64_t kHoldDurationMs = 1200;

        std::array<SplashFrame, kFrameCount> mFrames;
        std::size_t mCurrentFrameIndex = 0;
        float mElapsedFrameTime = 0.0f;
        bool mTransitioned = false;

        uint8_t GetBlackOverlayAlpha(float elapsedSeconds) const;

    public:
        SplashScene() = default;
        void Load(Game* game) override;
        void Update(Game* game) override;
        void Draw(Game* game) override;
        void Unload(Game* game) override {};
        std::string GetName() const override;
    };
} // namespace nuvelocity::frs42

#endif // FRS42_SPLASH_SCENE_H
