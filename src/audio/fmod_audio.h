#if 0
#pragma once

#include "../audio.h"

#include "common.h"
#include "gmath.h"

#include <fmod.hpp>

namespace atl
{
    void fmod_error(FMOD_RESULT& r);

    struct fmod_sound : public sound
    {
        ~sound() override;
        FMOD::Sound* internal_sound;
    };

    class fmod_sound_channel : public sound_channel
    {
    public:
        fmod_sound_channel(FMOD::Channel* ch) : channel(ch) {}
        ~fmod_sound_channel() override;
        b32 is_paused() const override;
        b32 finished_playing() const override;
        void stop() override;
        void pause(b32 do_pause) override;
        void set_volume(f32 v) override;
        void set_pan(f32 p) override;
        void set_frequency(f32 hz) override;
        void set_3d_attributes(const vec3& position, const vec3& velocity) override;
        void destroy() override;

    private:
        FMOD::Channel* channel;
    };

    class fmod_audio_system : public audio_system
    {
    public:
        fmod_audio_system() {}
        ~fmod_audio_system();

        void init() override;
        void update() override;
        void destroy() override;
        ref<sound> create_sound(const char* file) override;
        ref<sound_channel> play(ref<sound>& playing_sound, f32 volume = 0.5f, f32 pan = 1.0f, f32 frequency = -1, b32 start_paused = false) override;
        ref<sound_channel> play(ref<sound>& playing_sound, const vec3& position, const vec3& velocity, f32 volume = 1.0f, f32 pan = 1.0f, f32 frequency = -1, b32 start_paused = false) override;

        FMOD::System* get_system() { return system; }

    private:
        FMOD_RESULT result;
        FMOD::System* system = nullptr;
    };
}
#endif