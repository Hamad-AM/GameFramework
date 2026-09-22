#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "application.h"

int main()
{
    {
        application app;
        app.initialize(1280, 720);
        app.run();
        app.close();
    }
    return 0;
}
