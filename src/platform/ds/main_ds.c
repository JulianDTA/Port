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
#include "core/deck.h"

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
    input->btn_l = (keys_down & KEY_L) != 0;
    input->btn_r = (keys_down & KEY_R) != 0;
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
    game_update_selection_preview(&game);

    PlatformInput input;
    memset(&input, 0, sizeof(input));

    bool need_redraw = true;

    while (1) {
        hal_update_input(&input);

        // --- Stylus Touch Controls on Bottom Screen ---
        if (input.touch_just_pressed) {
            int tx = input.touch_x;
            int ty = input.touch_y;

            if (game.current_state == STATE_ROUND_ACTIVE) {
                // 1. Sort Buttons
                // Sort by Rank (x: 6..90, y: 4..28)
                if (tx >= 6 && tx <= 90 && ty >= 4 && ty <= 28) {
                    deck_sort_by_rank(&game.deck);
                    game_update_selection_preview(&game);
                    need_redraw = true;
                }
                // Sort by Suit (x: 94..178, y: 4..28)
                else if (tx >= 94 && tx <= 178 && ty >= 4 && ty <= 28) {
                    deck_sort_by_suit(&game.deck);
                    game_update_selection_preview(&game);
                    need_redraw = true;
                }
                // 2. Touch Cards in Hand (generous bounds: y: 44..124)
                else if (ty >= 44 && ty <= 124) {
                    for (int i = 0; i < game.deck.hand.count; ++i) {
                        int cx = 8 + (i * 30);
                        if (tx >= cx - 2 && tx <= cx + 28) {
                            const Card* sel[5];
                            int count = deck_get_selected_cards(&game.deck, sel);
                            if (game.deck.hand.cards[i].is_selected) {
                                game.deck.hand.cards[i].is_selected = false;
                                game_update_selection_preview(&game);
                                need_redraw = true;
                            } else if (count < 5) {
                                game.deck.hand.cards[i].is_selected = true;
                                game_update_selection_preview(&game);
                                need_redraw = true;
                            }
                            break;
                        }
                    }
                }
                // 3. Touch [PLAY HAND] Button (x: 10..126, y: 134..186)
                else if (tx >= 10 && tx <= 126 && ty >= 134 && ty <= 186) {
                    input.btn_a = true;
                }
                // 4. Touch [DISCARD] Button (x: 130..246, y: 134..186)
                else if (tx >= 130 && tx <= 246 && ty >= 134 && ty <= 186) {
                    input.btn_b = true;
                }
            } else if (game.current_state == STATE_ROUND_VICTORY) {
                // Touch [NEXT BLIND >>] Button (x: 20..236, y: 126..186)
                if (tx >= 20 && tx <= 236 && ty >= 126 && ty <= 186) {
                    game_advance_next_blind(&game);
                    need_redraw = true;
                }
            } else if (game.current_state == STATE_GAME_OVER) {
                // Touch [RESTART RUN] Button (x: 20..236, y: 126..186)
                if (tx >= 20 && tx <= 236 && ty >= 126 && ty <= 186) {
                    game_session_start_run(&game);
                    game_update_selection_preview(&game);
                    need_redraw = true;
                }
            }
        }

        // --- Physical DS Buttons Handling ---
        // Button A: Play Hand (or advance / restart when round ends)
        if (input.btn_a) {
            if (game.current_state == STATE_ROUND_ACTIVE) {
                if (game_play_selected_hand(&game)) {
                    need_redraw = true;
                }
            } else if (game.current_state == STATE_ROUND_VICTORY) {
                game_advance_next_blind(&game);
                need_redraw = true;
            } else if (game.current_state == STATE_GAME_OVER) {
                game_session_start_run(&game);
                game_update_selection_preview(&game);
                need_redraw = true;
            }
        }

        // Button B: Discard
        if (input.btn_b && game.current_state == STATE_ROUND_ACTIVE) {
            if (game_discard_selected_cards(&game)) {
                need_redraw = true;
            }
        }

        // Button X or L Shoulder: Sort by Rank
        if ((input.btn_x || input.btn_l) && game.current_state == STATE_ROUND_ACTIVE) {
            deck_sort_by_rank(&game.deck);
            game_update_selection_preview(&game);
            need_redraw = true;
        }

        // Button Y or R Shoulder: Sort by Suit
        if ((input.btn_y || input.btn_r) && game.current_state == STATE_ROUND_ACTIVE) {
            deck_sort_by_suit(&game.deck);
            game_update_selection_preview(&game);
            need_redraw = true;
        }

        // Button Start: Advance Next Blind or Restart Run
        if (input.btn_start) {
            if (game.current_state == STATE_ROUND_VICTORY) {
                game_advance_next_blind(&game);
                need_redraw = true;
            } else if (game.current_state == STATE_GAME_OVER) {
                game_session_start_run(&game);
                game_update_selection_preview(&game);
                need_redraw = true;
            }
        }

        // --- Render graphical screens ---
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
