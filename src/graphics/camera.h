#ifndef CAMERA_HH
#define CAMERA_HH

#include "../common.h"

class camera2d
{
public:
    glm::mat4
    projection_view() { return projection_view_; }
    glm::mat4
    projection() { return projection_; }
    glm::mat4
    view() { return view_; }

    void
    update(glm::vec3 position)
    {
        glm::mat4 identity = glm::mat4(1.0f);
        glm::mat4 camera_pos = glm::translate(identity, position);
        view_ = glm::inverse(camera_pos);
        projection_view_ = view_ * projection_;
    }

    void
    set_projection(glm::mat4 projection) { projection_ = projection; }

    void
    set_view(glm::mat4 view) { view_ = view; }

    void
    set_projection_view(glm::mat4 projection_view) { projection_view_ = projection_view; }
private:
    glm::mat4 projection_;
    glm::mat4 view_;
    glm::mat4 projection_view_;
};

#endif