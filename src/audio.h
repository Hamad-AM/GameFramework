#pragma once

#include "common.h"
#include "gmath.h"

    struct sound
    {
        virtual ~sound() {}
        virtual void start() = 0;
        virtual void start(u64 milliseconds) = 0;
        virtual void stop() = 0;
        virtual void stop(u64 milliseconds) = 0;
        virtual void pause() = 0;
        virtual b32 is_playing() const = 0;
        virtual void set_looping(b32 loop) = 0;
        virtual b32 is_looping() const = 0;
        virtual void set_volume(f32 v) = 0;
        virtual void set_pan(f32 p) = 0;
        virtual void set_pitch(f32 p) = 0;
        virtual f32 get_volume() const = 0;
        virtual f32 get_pan() const = 0;
        virtual f32 get_pitch() const = 0;
        virtual void set_fade(f32 volume_start, f32 volume_end, u64 milliseconds) = 0;
        virtual void destroy() = 0;

        
        const char* file;
        b32 has_position = false;
        b32 loop = false;
        std::string name;
    };

    class sound_channel
    {
    public:
        sound_channel() {}
        virtual ~sound_channel() {}
        virtual b32 is_paused() const = 0;
        virtual b32 finished_playing() const = 0;
        virtual void pause(b32 do_pause) = 0;
        virtual void set_volume(f32 v) = 0;
        virtual void set_pan(f32 p) = 0;
        virtual void set_frequency(f32 hz) = 0;
        virtual void set_3d_attributes(const vec3& position, const vec3& velocity) = 0;
        virtual void destroy() = 0;
    };

    class audio_system
    {
    public:
        audio_system() {}

        virtual ~audio_system() {}
        virtual void init() = 0;
        virtual void destroy() = 0;
        virtual void* get_system() = 0;
        virtual u64 get_time() = 0;

        virtual ref<sound> create_sound(const char* file) = 0;
        // virtual void play(ref<sound>& playing_sound, f32 volume = 0.5f, f32 pan = 1.0f, f32 frequency = -1, b32 start_paused = false) = 0;
        // virtual void play(ref<sound>& playing_sound, const vec3& position, const vec3& velocity, f32 volume = 1.0f, f32 pan = 1.0f, f32 frequency = -1, b32 start_paused = false) = 0;

    protected:
        std::vector<ref<sound>> sounds;
    };