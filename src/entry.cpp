#include "application.h"

int main()
{
    {
        atl::application app;
        app.initialize(1280, 720);
        app.run();
        app.close();
    }
    
    return 0;
}