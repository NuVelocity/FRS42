#ifndef FRS42_SPLASH_SCENE_H
#define FRS42_SPLASH_SCENE_H

#include <Image.h>
#include <Scene.h>

#include <array>
#include <cstddef>
#include <cstdint>

namespace nuvelocity::frs42
{
    class SplashScene : public Scene
    {
    private:
        struct SplashFrame
        {
            const char* assetPath = nullptr;
            Image image;
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
        void Load(Game* aGame) override;
        void Update(Game* aGame) override;
        void Draw(Game* aGame) override;
        void Unload(Game* aGame) override {};
        std::string GetName() const override;
    };
} // namespace nuvelocity::frs42

#endif // FRS42_SPLASH_SCENE_H
