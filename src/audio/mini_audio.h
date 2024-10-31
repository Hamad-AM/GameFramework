#pragma once

#include "../audio.h"

#include "miniaudio.h"

namespace atl
{
    void mini_error(ma_result& r);

    struct mini_sound : public sound
    {
        ~mini_sound() override;
        void destroy() override;

        void start() override;
        void start(u64 milliseconds) override;
        void stop() override;
        void stop(u64 milliseconds) override;
        void pause() override;
        void set_volume(f32 v) override;
        void set_pan(f32 p) override;
        void set_pitch(f32 p) override;
        f32 get_volume() const override;
        f32 get_pan() const override;
        f32 get_pitch() const override;
        void set_fade(f32 volume_start, f32 volume_end, u64 milliseconds) override;
        b32 is_playing() const override;
        void set_looping(b32 loop) override;
        b32 is_looping() const override;
    
    private:
        ma_sound internal_sound;
        friend class mini_audio_system;
    };

    // class mini_sound_channel : public sound_channel
    // {
    // public:
    //     mini_sound_channel(FMOD::Channel* ch) : channel(ch) {}
    //     ~mini_sound_channel() override;
    //     b32 is_paused() const override;
    //     b32 finished_playing() const override;
    //     void pause(b32 do_pause) override;
    //     void set_volume(f32 v) override;
    //     void set_pan(f32 p) override;
    //     void set_frequency(f32 hz) override;
    //     void set_3d_attributes(const vec3& position, const vec3& velocity) override;
    //     void destroy() override;

    // private:
    //     FMOD::Channel* channel;
    // };

    class mini_audio_system : public audio_system
    {
    public:
        mini_audio_system() {}
        ~mini_audio_system() override;

        void init() override;
        // void update() override;
        void destroy() override;
        u64 get_time() override;
        ref<sound> create_sound(const char* file) override;
        // void play(ref<sound>& playing_sound, f32 volume = 0.5f, f32 pan = 1.0f, f32 frequency = -1, b32 start_paused = false) override;
        // void play(ref<sound>& playing_sound, const vec3& position, const vec3& velocity, f32 volume = 1.0f, f32 pan = 1.0f, f32 frequency = -1, b32 start_paused = false) override;
        void* get_system() override;


    private:
        ma_result result;
        ma_engine engine;
        ma_engine_config config;
    };
}