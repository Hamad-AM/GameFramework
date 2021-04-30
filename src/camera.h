#ifndef CAMERA_HH
#define CAMERA_HH

#include "math.h"

class Camera
{
public:
    m4x4
    projection_view() { return projection_view; }
    m4x4
    projection() { return projection_; }
    m4x4
    view() { return view_; }

    void
    set_projection(m4x4 projection) { projection_ = projection; }

    void
    set_view(m4x4 view) { view_ = view; }

    void
    set_projection_view(m4x4 projection_view) { projection_view_ = projection_view; }
private:
    m4x4 projection_;
    m4x4 view_;
    m4x4 project_view_;
    v3 position_;
}


#endif