#if 0
#include "../audio.h"

#include <fmod_errors.h>

#include <iostream>

namespace atl
{
    void fmod_error(FMOD_RESULT& r)
    {
        if (r != FMOD_OK)
        {
            std::cout << "FMOD Error " << r << " " << FMOD_ErrorString(r) << std::endl;
        }
    }

    void fmod_audio_system::init()
    {
        result = FMOD::System_Create(&system);
        fmod_error(result);

        result = system->init(512, FMOD_INIT_NORMAL, 0);
        fmod_error(result);
    }

    ref<sound> fmod_audio_system::create_sound(const char* file)
    {
        sound s;
        s.name = file;
        FMOD::Sound* internal_sound;
        result = system->createSound(file, FMOD_DEFAULT, {}, &internal_sound);
        fmod_error(result);
        ref<sound> s_p = make_ref<sound>(s);
        s_p->internal_sound = internal_sound;
        sounds.push_back(s_p);
        return s_p;
    }

    ref<sound_channel> fmod_audio_system::play(ref<sound>& playing_sound, f32 volume, f32 pan, f32 frequency, b32 start_paused)
    {
        FMOD::Channel* f_channel = nullptr;
        playing_sound->internal_sound->setMode(FMOD_LOOP_OFF);
        result = system->playSound(playing_sound->internal_sound, nullptr, false, &f_channel);
        fmod_error(result);

        ref<sound_channel> game_channel = make_ref<fmod_sound_channel>(f_channel);
        game_channel->set_volume(volume);
        game_channel->set_pan(pan);
        // game_channel->set_frequency(frequency);
        game_channel->pause(start_paused);

        channels.push_back(game_channel);
        return game_channel;
    }

    ref<sound_channel> fmod_audio_system::play(ref<sound>& playing_sound, const vec3& position, const vec3& velocity, f32 volume, f32 pan, f32 frequency, b32 start_paused)
    {
        ref<sound_channel> chan = play(playing_sound, volume, pan, frequency, start_paused);
        chan->set_3d_attributes(position, velocity);
        return chan;
    }

    fmod_audio_system::~fmod_audio_system()
    {
        destroy();
    }

    void fmod_audio_system::destroy()
    {
        result = system->close();
        fmod_error(result);
        result = system->release();
        fmod_error(result);
        system = nullptr;
    }

    void fmod_audio_system::update()
    {
        result = system->update();
        fmod_error(result);
    }

    fmod_sound_channel::~fmod_sound_channel()
    {
        channel = nullptr;
    }

    void fmod_sound_channel::pause(b32 do_pause)
    {
        if (channel != nullptr)
        {
            channel->setPaused(do_pause);
        }
    }

    b32 fmod_sound_channel::is_paused() const
    {
        b32 paused = true;
        if (channel != nullptr)
        {
            channel->getPaused(&paused);
        }
        return paused;
    }

    b32 fmod_sound_channel::finished_playing() const
    {
        if (channel == nullptr)
        {
            return true;
        }
        else
        {
            b32 result;
            channel->isPlaying(&result);
            return !result;
        }
    }

    void fmod_sound_channel::stop()
    {
        if (channel != nullptr)
        {
            channel->stop();
            channel = nullptr;
        }
    }

    void fmod_sound_channel::set_volume(f32 v)
    {
        if (channel != nullptr)
        {
            channel->setVolume(v);
        }
    }

    void fmod_sound_channel::set_pan(f32 p)
    {
        if (channel != nullptr)
        {
            channel->setPan(p);
        }
    }
    void fmod_sound_channel::set_frequency(f32 hz)
    {
        if (channel != nullptr)
        {
            channel->setFrequency(hz);
        }
    }

    void fmod_sound_channel::set_3d_attributes(const vec3& position, const vec3& velocity)
    {
        if (channel != nullptr)
        {
            channel->set3DAttributes((FMOD_VECTOR*)&position.x, (FMOD_VECTOR*)&velocity.x);
        }
    }

    void fmod_sound_channel::destroy()
    {
        channel = nullptr;
    }

    fmod_sound::~fmod_sound()
    {
        if (internal_sound)
        {
            internal_sound->release();
            internal_sound = nullptr;
        }
    }
}
#endif