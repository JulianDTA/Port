#ifdef ARM9
#include <nds.h>
#endif
#include <stdio.h>
#include <string.h>
#include "platform/hal.h"
#include "platform/renderer_ds.h"
#include "core/game_state.h"
#include "core/card.h"
#include "core/poker_hand.h"
#include "core/jokers.h"

#ifdef ARM9
static touchPosition s_touch;
static uint16_t* s_top_fb = NULL;
static uint16_t* s_sub_fb = NULL;

void hal_init(void) {
    // 16-bit direct bitmap mode on both screens (MODE 5 2D)
    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);

    // Map VRAM Bank A (128 KB) to Main engine background
    // Map VRAM Bank C (128 KB) to Sub engine background
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    // Initialize layer 3 as a 16-bit bitmap on both screens
    int bgTop = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    int bgSub = bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

    s_top_fb = bgGetGfxPtr(bgTop);
    s_sub_fb = bgGetGfxPtr(bgSub);
}

void hal_update_input(PlatformInput* input) {
    if (!input) return;
    scanKeys();
    uint32_t keys_down = keysDown();
    uint32_t keys_held = keysHeld();

    touchRead(&s_touch);
    input->is_touching = (keys_held & KEY_TOUCH) != 0;
    input->touch_just_pressed = (keys_down & KEY_TOUCH) != 0;
    input->touch_x = s_touch.px;
    input->touch_y = s_touch.py;

    input->btn_a = (keys_down & KEY_A) != 0;
    input->btn_b = (keys_down & KEY_B) != 0;
    input->btn_x = (keys_down & KEY_X) != 0;
    input->btn_y = (keys_down & KEY_Y) != 0;
    input->btn_start = (keys_down & KEY_START) != 0;
    input->btn_select = (keys_down & KEY_SELECT) != 0;

    input->dpad_x = 0;
    if (keys_down & KEY_LEFT) input->dpad_x = -1;
    if (keys_down & KEY_RIGHT) input->dpad_x = 1;

    input->dpad_y = 0;
    if (keys_down & KEY_UP) input->dpad_y = -1;
    if (keys_down & KEY_DOWN) input->dpad_y = 1;
}

void hal_wait_vsync(void) {
    swiWaitForVBlank();
}
#else
void hal_init(void) {}
void hal_update_input(PlatformInput* input) { (void)input; }
void hal_wait_vsync(void) {}
#endif

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    hal_init();

    GameSession game;
    game_session_init(&game);
    game_session_start_run(&game);

    // Equip starting Jokers for visual flair
    Joker cavendish, lusty, joker_card;
    joker_create(&cavendish, JOKER_CAVENDISH, EDITION_NONE);
    joker_create(&lusty, JOKER_LUSTY, EDITION_NONE);
    joker_create(&joker_card, JOKER_JOKER, EDITION_FOIL);
    game.jokers[0] = &cavendish;
    game.jokers[1] = &lusty;
    game.jokers[2] = &joker_card;
    game.joker_count = 3;

    PlatformInput input;
    memset(&input, 0, sizeof(input));

    bool need_redraw = true;

    while (1) {
        hal_update_input(&input);

        // Stylus Touch Controls on Bottom Screen
        if (input.touch_just_pressed) {
            int tx = input.touch_x;
            int ty = input.touch_y;

            // 1. Touch Cards in Hand (x from 8 to 248, y from 45 to 115)
            if (ty >= 45 && ty <= 115) {
                for (int i = 0; i < game.deck.hand.count; ++i) {
                    int cx = 8 + (i * 30);
                    if (tx >= cx && tx <= cx + 28) {
                        const Card* sel[5];
                        int count = deck_get_selected_cards(&game.deck, sel);
                        if (game.deck.hand.cards[i].is_selected) {
                            game.deck.hand.cards[i].is_selected = false;
                            need_redraw = true;
                        } else if (count < 5) {
                            game.deck.hand.cards[i].is_selected = true;
                            need_redraw = true;
                        }
                        break;
                    }
                }
            }

            // 2. Touch [PLAY HAND] Button (x: 16 to 124, y: 138 to 180)
            if (tx >= 16 && tx <= 124 && ty >= 138 && ty <= 180) {
                input.btn_a = true;
            }

            // 3. Touch [DISCARD] Button (x: 132 to 240, y: 138 to 180)
            if (tx >= 132 && tx <= 240 && ty >= 138 && ty <= 180) {
                input.btn_b = true;
            }
        }

        // Action: PLAY HAND (Button A or Play button touched)
        if (input.btn_a) {
            if (game_play_selected_hand(&game)) {
                need_redraw = true;
                if (game.current_state == STATE_ROUND_VICTORY) {
                    // Advance to next blind
                    if (game.current_blind == BLIND_SMALL) {
                        game_session_select_blind(&game, BLIND_BIG);
                    } else if (game.current_blind == BLIND_BIG) {
                        game_session_select_blind(&game, BLIND_BOSS);
                    } else {
                        game.ante++;
                        game_session_select_blind(&game, BLIND_SMALL);
                    }
                }
            }
        }

        // Action: DISCARD (Button B or Discard button touched)
        if (input.btn_b) {
            if (game_discard_selected_cards(&game)) {
                need_redraw = true;
            }
        }

        // Render graphical screens when needed
#ifdef ARM9
        if (need_redraw && s_top_fb && s_sub_fb) {
            renderer_draw_top_screen(s_top_fb, &game);
            renderer_draw_bottom_screen(s_sub_fb, &game);
            need_redraw = false;
        }
#endif

        hal_wait_vsync();
    }

    return 0;
}
