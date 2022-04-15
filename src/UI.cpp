#include "UI.h"

#include <glm/glm.hpp>

#include "event.h"
#include "input.h"


void
container::draw(renderer2d* renderer, vec2 position)
{
    for (auto child : _children)
    {
        child->draw(renderer, {position.x + this->get_position().x, position.y + this->get_position().y});
    }
}

void
ui_text::draw(renderer2d* renderer, vec2 position)
{
    vec2 screen_pos{ position.x + this->get_position().x, position.y + this->get_position().y};

    renderer->draw_text_single(_text, screen_pos.x, screen_pos.y, get_size().y, {_color.x, _color.y, _color.z});
}

button::button(ui_text text, texture2d texture, vec2 position, vec2 size) : _text(text), _texture(texture), ui_element(position, size)
{
    event_callback btn_press = [this](event* event) { this->on_event(event); };
    event_system::get().subscribe(event_type::left_mouse_button_press, btn_press);
    event_callback btn_hover = [this](event* event) { this->on_event(event); };
    event_system::get().subscribe(event_type::mouse_move, btn_hover);
}

void
button::on_event(event* event)
{
    mouse_event* me = (mouse_event*)event;
    vec2 position = me->mouse_position;
    if (position.x < (this->get_position().x + this->get_size().x) &&
        position.x > (this->get_position().x) &&
        position.y < (this->get_position().y + this->get_size().y) &&
        position.y > (this->get_position().y))
    {
        switch(me->type)
        {
        case event_type::left_mouse_button_press: this->on_button_press();
        case event_type::mouse_move: this->on_button_hover();
        }
    }
}

void
button::draw(renderer2d* renderer, vec2 position)
{
    vec2 screen_pos{ position.x + this->get_position().x, position.y + this->get_position().y};
    renderer->draw_sprite_single(_texture, { screen_pos.x, screen_pos.y }, { get_size().x, get_size().y });
}