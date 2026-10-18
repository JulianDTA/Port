#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <stdbool.h>

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH  256
#endif
#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 192
#endif

typedef struct {
    int touch_x;
    int touch_y;
    bool is_touching;
    bool touch_just_pressed;
    bool btn_a;
    bool btn_b;
    bool btn_x;
    bool btn_y;
    bool btn_l;
    bool btn_r;
    bool btn_start;
    bool btn_select;
    int dpad_x;
    int dpad_y;
} PlatformInput;

void hal_init(void);
void hal_update_input(PlatformInput* input);
void hal_clear_screens(void);
void hal_draw_text_top(int x, int y, const char* text);
void hal_draw_text_bottom(int x, int y, const char* text);
void hal_draw_card(int screen_id, int x, int y, int card_id, bool is_selected);
void hal_wait_vsync(void);

#endif // HAL_H
