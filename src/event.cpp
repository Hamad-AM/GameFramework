#include "event.h"

event_system* event_system::s_event_system = new event_system();

void
event_system::subscribe(event_type type, event_callback callback)
{
    observers[type].push_back(callback);
}

void
event_system::post_all(event_type type, event* event)
{
    for (event_callback cb : observers[type])
        cb(event);
}
