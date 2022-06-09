#pragma once

#include "common.h"
#include "gmath.h"
#include "event.h"

#include "graphics/texture.h"
#include "graphics/renderer2d.h"

namespace alg
{
    class ui_element
    {
    public:
        ui_element(vec2 position, vec2  size) : _position(position), _size(size) {}

        virtual void draw(renderer* render, vec2 position) { return; }

        vec2 get_position() { return _position; }
        vec2 get_size() { return _size; }
        bool get_visable() { return _visable; }

        void set_position(vec2 p) { _position = p; }
        void set_size(vec2 size) { _size = size; }
        void visability(bool vis) { _visable = vis; }

    private:
        vec2 _position;
        vec2 _size;
        bool _visable = false;
    };

    enum container_type
    {
        vbox,
        hbox,
        fbox
    };

    class container : public ui_element
    {
    public:
        container(container_type type, vec2 position, vec2 size) : _type(type), ui_element(position, size)
        {}

        void draw(renderer* render, vec2 position);

    private:
        std::vector<ref<ui_element>> _children;
        container_type _type;
    };

    class ui_text : public ui_element
    {
    public:
        ui_text(const char* text, vec2 position, vec2 size) : _text(text), ui_element(position, size) {}

        void draw(renderer* render, vec2 position);

        void set_text(const char* text) { _text = text; }
        const char* get_text() { return _text; }

    private:
        const char* _text;
        font_type _font = font_type::Montserrat;
        vec4 _color{0.0f, 0.0f, 0.0f, 1.0f};
    };

    class button : public ui_element
    {
    public:
        button(ui_text text, texture2d texture, vec2 position, vec2 size);

        void on_event(event* event);

        virtual void on_button_press() { return; }

        virtual void on_button_hover() { return; }

        void set_texture(texture2d texture) { _texture = texture; }
        texture2d get_texture() { return _texture; }

        void set_text(const char* text) { _text.set_text(text); }
        const char* get_text() { return _text.get_text(); }

        void draw(renderer* render, vec2 position);

    private:
        texture2d _texture;
        ui_text _text;
    };

    class ui_pane
    {
    public:
        ui_pane(s32 width, s32 height);

        void draw(renderer* render);

    private:
        container _root;
    };
}