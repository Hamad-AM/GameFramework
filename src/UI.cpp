#include "UI.h"

#include <glm/glm.hpp>

#include "event.h"
#include "input.h"

namespace alg
{
    void
    container::draw(renderer* render, vec2 position)
    {
        for (auto child : _children)
        {
            child->draw(render, {position.x + this->get_position().x, position.y + this->get_position().y});
        }
    }

    void
    ui_text::draw(renderer* render, vec2 position)
    {
        vec2 screen_pos{ position.x + this->get_position().x, position.y + this->get_position().y};

        render->draw_text(_text, screen_pos.x, screen_pos.y, get_size().y, _font, _color);
    }

    button::button(ui_text text, texture2d texture, vec2 position, vec2 size) : _text(text), _texture(texture), ui_element(position, size)
    {
        event_callback btn_press = [this](event* event) { this->on_button_press(event); };
        event_system::get().subscribe(event_type::left_mouse_button_press, btn_press);
        event_callback btn_hover = [this](event* event) { this->on_button_hover(event); };
        event_system::get().subscribe(event_type::mouse_move, btn_hover);
    }

    void
    button::draw(renderer* render, vec2 position)
    {
        vec2 screen_pos{ position.x + this->get_position().x, position.y + this->get_position().y};
        render->draw_sprite(_texture, { screen_pos.x, screen_pos.y }, { get_size().x, get_size().y });
    }
}