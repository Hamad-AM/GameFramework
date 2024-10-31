#include "application.h"

//#include <fmod.hpp>

int main()
{
    // // fmod test

    // FMOD::System     *system;
    // FMOD::Sound      *sound1, *sound2, *sound3;
    // FMOD::Channel    *channel = 0;
    // FMOD_RESULT       result;
    // void             *extradriverdata = 0;
    

    // /*
    //     Create a System object and initialize
    // */
    // result = FMOD::System_Create(&system);

    // result = system->init(32, FMOD_INIT_NORMAL, 0);

    // result = system->createSound("../assets/sound/coin_pickup.wav", FMOD_DEFAULT, 0, &sound1);

    // result = sound1->setMode(FMOD_LOOP_OFF);    /* drumloop.wav has embedded loop points which automatically makes looping turn on, */
    //                            /* so turn it off here.  We could have also just put FMOD_LOOP_OFF in the above CreateSound call. */

    // result = system->createSound("../assets/sound/jaguar.wav", FMOD_DEFAULT, 0, &sound2);

    // result = system->createSound("../assets/sound/swish.wav", FMOD_DEFAULT, 0, &sound3);
    // /*
    //     Main loop
    // */
    // do
    // {
    //     result = system->playSound(sound1, 0, false, &channel);

    //     // if (Common_BtnPress(BTN_ACTION2))
    //     // {
    //         // result = system->playSound(sound2, 0, false, &channel);
    //     //     ERRCHECK(result);
    //     // }

    //     // if (Common_BtnPress(BTN_ACTION3))
    //     // {
    //     //     result = system->playSound(sound3, 0, false, &channel);
    //     // }

    //     result = system->update();

    //     {
    //         unsigned int ms = 0;
    //         unsigned int lenms = 0;
    //         bool         playing = 0;
    //         bool         paused = 0;
    //         int          channelsplaying = 0;

    //         if (channel)
    //         {
    //             FMOD::Sound *currentsound = 0;

    //             result = channel->isPlaying(&playing);
    //             if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    //             {
    //                 std::cout << result << std::endl;
    //             }

    //             result = channel->getPaused(&paused);
    //             if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    //             {
    //                 std::cout << result << std::endl;
    //             }

    //             result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
    //             if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    //             {
    //                 std::cout << result << std::endl;
    //             }
               
    //             channel->getCurrentSound(&currentsound);
    //             if (currentsound)
    //             {
    //                 result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
    //                 if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    //                 {
    //                     std::cout << result << std::endl;
    //                 }
    //             }
    //         }

    //         system->getChannelsPlaying(&channelsplaying, NULL);
    //     }

    //     // sleep(5);
    // } while (true);

    // /*
    //     Shut down
    // */
    // result = sound1->release();
    // result = sound2->release();
    // result = sound3->release();
    // result = system->close();
    // result = system->release();

    // fmod test end

    {
        atl::application app;
        app.initialize(1280, 720);
        app.run();
        app.close();
    }
    
    return 0;
}