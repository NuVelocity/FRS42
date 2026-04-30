#ifndef NVE_BACKGROUND_DEFINITION_H
#define NVE_BACKGROUND_DEFINITION_H

#include "BackgroundSpriteGenerator.h"
#include "MusicMixerData.h"
#include <Object.h>
#include <SDL3/SDL.h>
#include <string>
#include <vector>

namespace nuvelocity::frs42
{
    class BackgroundDefinition : public Object<BackgroundDefinition>
    {
    public:
        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CBackgroundDefinition";
            AddProperty(info, "Draw Shadows", &BackgroundDefinition::mDrawShadows);
            AddProperty(info, "Shadow Offset", &BackgroundDefinition::mShadowOffset);
            AddProperty(info, "Foreground Image", &BackgroundDefinition::mForegroundImage);
            info.GetLastProperty()->SetDescription(
                "This image is infront of everything including the playfield and the ship");
            AddProperty(info, "Playfield", &BackgroundDefinition::mPlayfield);
            AddProperty(info, "Foreground Sprites", &BackgroundDefinition::mForegroundSprites);
            AddProperty(info, "Midground Image", &BackgroundDefinition::mMidgroundImage);
            info.GetLastProperty()->SetDescription(
                "This image is behind the playfield but in front of the background sprites");
            AddProperty(info, "Background Sprites", &BackgroundDefinition::mBackgroundSprites);
            AddProperty(info, "Background Image", &BackgroundDefinition::mBackgroundImage);
            info.GetLastProperty()->SetDescription(
                "This image is behind everything including the background sprites");
            AddProperty(info, "Music Tracks", &BackgroundDefinition::mMusicTracks);
            AddProperty(info, "Sprite Generators", &BackgroundDefinition::mSpriteGenerators);
        }

        const std::string& GetBackgroundImage() const
        {
            return mBackgroundImage;
        }
        const std::string& GetMidgroundImage() const
        {
            return mMidgroundImage;
        }
        const std::string& GetForegroundImage() const
        {
            return mForegroundImage;
        }
        const std::vector<BackgroundSpriteGenerator*>& GetSpriteGenerators() const
        {
            return mSpriteGenerators;
        }

    public:
        bool mDrawShadows = false;
        SDL_FPoint mShadowOffset = {0, 0};
        std::string mForegroundImage = "!None";
        bool mPlayfield = false;
        int mForegroundSprites = 0;
        std::string mMidgroundImage = "!None";
        int mBackgroundSprites = 0;
        std::string mBackgroundImage = "!None";
        MusicMixerData* mMusicTracks = nullptr;
        std::vector<BackgroundSpriteGenerator*> mSpriteGenerators;
    };
} // namespace nuvelocity::frs42

#endif // NVE_BACKGROUND_DEFINITION_H
