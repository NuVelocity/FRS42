#ifndef NVE_MUSIC_MIXER_DATA_H
#define NVE_MUSIC_MIXER_DATA_H

#include <Object.h>
#include <string>

namespace nuvelocity::frs42
{
    class MusicMixerData : public Object<MusicMixerData>
    {
    public:
        static void InitClassInfo(ClassInfo& info)
        {
            info.mName = "CMusicMixerData";
            AddProperty(info, "Num Seconds To Switch", &MusicMixerData::mNumSecondsToSwitch);
            AddProperty(info, "Full Mix Song", &MusicMixerData::mFullMixSong);
            AddProperty(info, "Med Mix Song", &MusicMixerData::mMedMixSong);
            AddProperty(info, "Ambiant Song", &MusicMixerData::mAmbiantSong);
        }

        float mNumSecondsToSwitch = 0.0F;
        std::string mFullMixSong;
        std::string mMedMixSong;
        std::string mAmbiantSong;
    };
} // namespace nuvelocity::frs42

#endif // NVE_MUSIC_MIXER_DATA_H
