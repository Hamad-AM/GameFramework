#pragma once

#include "common.h"
#include "gmath.h"

#include <fmod.hpp>

namespace atl
{
    struct sound
    {
        ~sound();
        const char* file;
        b32 has_position = false;
        b32 loop = false;
        std::string name;
        FMOD::Sound* fmod_sound;
    };

    class sound_channel
    {
    public:
        sound_channel(FMOD::Channel* ch) : channel(ch) {}
        ~sound_channel();

        b32 is_paused() const;
        b32 finished_playing() const;

        void stop();
        void pause(b32 do_pause);
        void set_volume(f32 v);
        void set_pan(f32 p);
        void set_frequency(f32 hz);

        void set_3d_attributes(const vec3& position, const vec3& velocity);

        void destroy();

    private:
        FMOD::Channel* channel;
    };

    class audio_system
    {
    public:
        audio_system() {}
        ~audio_system();

        void init();

        ref<sound> create_sound(const char* file);
        ref<sound_channel> play(ref<sound>& playing_sound, f32 volume = 1.0f, f32 pan = 1.0f, f32 frequency = -1, b32 start_paused = false);
        ref<sound_channel> play(ref<sound>& playing_sound, const vec3& position, const vec3& velocity, f32 volume = 1.0f, f32 pan = 1.0f, f32 frequency = -1, b32 start_paused = false);

        void update();

        void destroy();

    private:
        FMOD_RESULT result;
        FMOD::System* system = nullptr;
        std::vector<ref<sound>> sounds;
        std::vector<ref<sound_channel>> channels;
    };
}