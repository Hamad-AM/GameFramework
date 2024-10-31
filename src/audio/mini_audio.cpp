#include "mini_audio.h"

namespace atl
{
    void mini_audio_system::init()
    {
        result = ma_engine_init(NULL, &engine);
        assert(result == MA_SUCCESS);
    }

    mini_audio_system::~mini_audio_system()
    {
        destroy();
    }

    void mini_audio_system::destroy()
    {
        ma_engine_uninit(&engine);
        sounds.clear();
    }

    u64 mini_audio_system::get_time()
    {
        return ma_engine_get_time(&engine);
    }

    ref<sound> mini_audio_system::create_sound(const char* file)
    {
        ref<mini_sound> s(new mini_sound());
        s->file = file;
        result = ma_sound_init_from_file(&engine, file, 0, NULL, NULL, &s->internal_sound);
        assert(result == MA_SUCCESS);
        return s;
    }

    void* mini_audio_system::get_system()
    {
        return &engine;
    }

    mini_sound::~mini_sound()
    {
        mini_sound::destroy();
    }

    void mini_sound::destroy()
    {
        ma_sound_uninit(&internal_sound);
    }

    void mini_sound::start()
    {
        ma_sound_start(&internal_sound);
    }

    void mini_sound::start(u64 milliseconds)
    {
        ma_sound_set_start_time_in_milliseconds(&internal_sound, milliseconds);
        start();
    }

    void mini_sound::stop()
    {
        ma_sound_stop(&internal_sound);
        ma_sound_seek_to_pcm_frame(&internal_sound, 0);
    }

    void mini_sound::stop(u64 milliseconds)
    {
        ma_sound_set_stop_time_in_milliseconds(&internal_sound, milliseconds);
        stop();
    }

    void mini_sound::pause()
    {
        ma_sound_stop(&internal_sound);
    }

    void mini_sound::set_volume(f32 v)
    {
        ma_sound_set_volume(&internal_sound, v);
    }

    void mini_sound::set_pan(f32 p)
    {
        ma_sound_set_pan(&internal_sound, p);
    }

    void mini_sound::set_pitch(f32 p)
    {
        ma_sound_set_pitch(&internal_sound, p);
    }
    
    f32 mini_sound::get_volume() const
    {
        return ma_sound_get_volume(&internal_sound);
    }

    f32 mini_sound::get_pan() const
    {
        return ma_sound_get_pan(&internal_sound);
    }

    f32 mini_sound::get_pitch() const
    {
        return ma_sound_get_pitch(&internal_sound);
    }

    b32 mini_sound::is_playing() const
    {
        return ma_sound_is_playing(&internal_sound);
    }

    void mini_sound::set_looping(b32 loop)
    {
        ma_sound_set_looping(&internal_sound, loop);
    }

    b32 mini_sound::is_looping() const
    {
        return ma_sound_is_looping(&internal_sound);
    }

    void mini_sound::set_fade(f32 volume_start, f32 volume_end, u64 milliseconds)
    {
        ma_sound_set_fade_in_milliseconds(&internal_sound, volume_start, volume_end, milliseconds);
    }
}