#include "event.h"

namespace alg
{
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
}