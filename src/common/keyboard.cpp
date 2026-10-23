// the keyboard as the controller in port 1, on top of a real pad there if one is plugged in
#include <SDL3/SDL.h>

#include "core/input.h"

namespace game {

namespace {

enum Button : uint16_t {
    kUp = 1 << 0,
    kDown = 1 << 1,
    kLeft = 1 << 2,
    kRight = 1 << 3,
    kStart = 1 << 4,
    kBack = 1 << 5,
    kLeftThumb = 1 << 6,
    kRightThumb = 1 << 7,
};
enum Analog { kA, kB, kX, kY, kBlack, kWhite, kLeftTrigger, kRightTrigger };

constexpr struct {
    SDL_Scancode key;
    uint16_t button;
} kButtons[] = {
    {SDL_SCANCODE_UP, kUp},         {SDL_SCANCODE_DOWN, kDown},        {SDL_SCANCODE_LEFT, kLeft},
    {SDL_SCANCODE_RIGHT, kRight},   {SDL_SCANCODE_RETURN, kStart},     {SDL_SCANCODE_BACKSPACE, kBack},
    {SDL_SCANCODE_ESCAPE, kBack},   {SDL_SCANCODE_LSHIFT, kLeftThumb}, {SDL_SCANCODE_RSHIFT, kRightThumb},
};

constexpr struct {
    SDL_Scancode key;
    Analog analog;
} kAnalogs[] = {
    {SDL_SCANCODE_SPACE, kA}, {SDL_SCANCODE_J, kA},     {SDL_SCANCODE_K, kB},
    {SDL_SCANCODE_U, kX},     {SDL_SCANCODE_I, kY},     {SDL_SCANCODE_O, kBlack},
    {SDL_SCANCODE_L, kWhite}, {SDL_SCANCODE_Q, kLeftTrigger}, {SDL_SCANCODE_E, kRightTrigger},
};

// a pair of keys as one stick axis, the pad's own deflection kept when no key is down
int16_t Axis(const bool* keys, SDL_Scancode neg, SDL_Scancode pos, int16_t pad) {
    int v = (keys[pos] ? 32767 : 0) - (keys[neg] ? 32767 : 0);
    return v ? int16_t(v) : pad;
}

}  // namespace

void KeyboardPad(int port, input::Pad& p) {
    if (port != 0) return;
    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (!keys) return;
    p.connected = true;
    for (auto [key, button] : kButtons)
        if (keys[key]) p.buttons |= button;
    for (auto [key, analog] : kAnalogs)
        if (keys[key]) p.analog[analog] = 0xFF;
    p.lx = Axis(keys, SDL_SCANCODE_A, SDL_SCANCODE_D, p.lx);
    p.ly = Axis(keys, SDL_SCANCODE_S, SDL_SCANCODE_W, p.ly);  // y up, as the console's stick
    p.rx = Axis(keys, SDL_SCANCODE_KP_4, SDL_SCANCODE_KP_6, p.rx);
    p.ry = Axis(keys, SDL_SCANCODE_KP_2, SDL_SCANCODE_KP_8, p.ry);
}

}  // namespace game
