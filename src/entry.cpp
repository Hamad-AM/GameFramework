#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include <tiny_gltf.h>

#include <fpng.h>
#include <fpng.cpp>

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
