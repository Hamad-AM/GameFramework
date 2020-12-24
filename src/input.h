
enum ButtonState
{
    Up,
    Down,
};

enum Keys
{
    a,
    b,
    c,
    d,
    e,
    f,
    g,
    h,
    i,
    j,
    k,
    l,
    m,
    n,
    o,
    p,
    q,
    r,
    s,
    t,
    u,
    v,
    w,
    x,
    y,
    z,
    BACKSPACE,
    TAB,
    SPACE,
    RIGHT,
    LEFT,
    DOWN,
    UP,
    LCTRL,
    LSHIFT,
    LALT,
};

struct GamePad
{
    ButtonState A;
    ButtonState B;
    ButtonState back;
    ButtonState left_shoulder;
    ButtonState right_shoulder;
    ButtonState start;
    ButtonState X;
    ButtonState Y;
    ButtonState down;
    ButtonState left;
    ButtonState right;
    ButtonState up;
    ButtonState left_stick;
    ButtonState right_stick;

    float left_stick_x;
    float left_stick_y;
    float right_stick_x;
    float right_stick_y;

    float left_trigger;
    float right_trigger;
};

struct Keyboard
{
    ButtonState keys[104];
};

struct Mouse
{
    ButtonState left_button;
    ButtonState right_button;
    ButtonState middle_button;
    int32 scroll_wheel_value;
    float x;
    float y;
};

struct Input
{
    bool game_pad_connected;
    GamePad game_pad[4];
    Keyboard keyboard;
    Mouse mouse;
};

