#pragma once

#include "common.h"
#include "gmath.h"

    class camera2d
    {
    public:
        const mat4&
        projection_view() const { return projection_view_; }
        const mat4&
        projection() const { return projection_; }
        const mat4&
        view() const { return view_; }

        void
        update(vec3 new_position)
        {
            position = new_position;
            mat4 identity = mat4(1.0f);
            mat4 camera_pos = translate(identity, position);
            view_ = inverse(camera_pos);
            projection_view_ = view_ * projection_;
        }

        void
        set_projection(mat4 projection) { projection_ = projection; }

        void
        set_view(mat4 view) { view_ = view; }

        void
        set_projection_view(mat4 projection_view) { projection_view_ = projection_view; }
    private:
        mat4 projection_;
        mat4 view_;
        mat4 projection_view_;
        vec3 position;
    };

    class camera3d
    {
        mat4
        projection_view() { return projection_view_; }
        mat4
        projection() { return projection_; }
        mat4
        view() { return view_; }

        void
        update_position(vec3 new_position)
        {
            position = new_position;
            mat4 identity = mat4(1.0f);
            mat4 camera_pos = translate(identity, position);
            view_ = inverse(camera_pos);
            projection_view_ = view_ * projection_;
        }

        void
        update_rotation(f32 amount, vec3 rotation_axis)
        {
            rotation = rotation + rotation_axis * amount;
            view_ = rotate(view_, amount, rotation_axis);
            projection_view_ = view_ * projection_;
        }

        void
        set_projection(mat4 projection) { projection_ = projection; }

        void
        set_view(mat4 view) { view_ = view; }

        void
        set_projection_view(mat4 projection_view) { projection_view_ = projection_view; }
    private:
        mat4 projection_;
        mat4 view_;
        mat4 projection_view_;
        vec3 position;
        vec3 rotation;
    };
