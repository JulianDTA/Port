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
#include "core/consumable.h"
#include "core/boss_blinds.h"
#include "core/shop.h"

#ifdef ARM9
static touchPosition s_touch;
static uint16_t* s_top_fb = NULL;
static uint16_t* s_sub_fb = NULL;

void hal_init(void) {
    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

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

            // State: Round Active
            if (game.current_state == STATE_ROUND_ACTIVE) {
                // 1. Sort Buttons
                if (tx >= 6 && tx <= 90 && ty >= 4 && ty <= 28) {
                    deck_sort_by_rank(&game.deck);
                    game_update_selection_preview(&game);
                    need_redraw = true;
                } else if (tx >= 94 && tx <= 178 && ty >= 4 && ty <= 28) {
                    deck_sort_by_suit(&game.deck);
                    game_update_selection_preview(&game);
                    need_redraw = true;
                }
                // 2. Use Consumable Button (if one is active)
                else if (game.selected_consumable_slot >= 0 && tx >= 180 && tx <= 252 && ty >= 4 && ty <= 28) {
                    consumable_use(&game, game.selected_consumable_slot);
                    need_redraw = true;
                }
                // 3. Cards in Hand (generous touch bounds)
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
                // 4. [PLAY HAND] Button
                else if (tx >= 10 && tx <= 126 && ty >= 134 && ty <= 186) {
                    input.btn_a = true;
                }
                // 5. [DISCARD] Button
                else if (tx >= 130 && tx <= 246 && ty >= 134 && ty <= 186) {
                    input.btn_b = true;
                }
            }
            // State: Round Victory (Cash Out)
            else if (game.current_state == STATE_ROUND_VICTORY) {
                if (tx >= 20 && tx <= 236 && ty >= 126 && ty <= 186) {
                    game_advance_to_shop(&game);
                    need_redraw = true;
                }
            }
            // State: The Shop
            else if (game.current_state == STATE_SHOP) {
                // Buy Slot 0 (Joker)
                if (tx >= 10 && tx <= 80 && ty >= 12 && ty <= 90) {
                    if (shop_buy_card_slot(&game, 0)) need_redraw = true;
                }
                // Buy Slot 1 (Tarot/Planet)
                else if (tx >= 86 && tx <= 156 && ty >= 12 && ty <= 90) {
                    if (shop_buy_card_slot(&game, 1)) need_redraw = true;
                }
                // Open Booster Pack
                else if (tx >= 162 && tx <= 246 && ty >= 12 && ty <= 90) {
                    if (shop_buy_pack(&game)) need_redraw = true;
                }
                // Buy Voucher
                else if (tx >= 10 && tx <= 156 && ty >= 96 && ty <= 134) {
                    if (shop_buy_voucher(&game)) need_redraw = true;
                }
                // Reroll Shop
                else if (tx >= 10 && tx <= 120 && ty >= 140 && ty <= 186) {
                    if (shop_reroll(&game)) need_redraw = true;
                }
                // Next Blind
                else if (tx >= 128 && tx <= 246 && ty >= 140 && ty <= 186) {
                    game_advance_next_blind(&game);
                    need_redraw = true;
                }
            }
            // State: Booster Pack Open
            else if (game.current_state == STATE_BOOSTER_OPEN) {
                if (tx >= 20 && tx <= 86 && ty >= 46 && ty <= 134) {
                    shop_choose_pack_card(&game, 0);
                    need_redraw = true;
                } else if (tx >= 96 && tx <= 162 && ty >= 46 && ty <= 134) {
                    shop_choose_pack_card(&game, 1);
                    need_redraw = true;
                } else if (tx >= 172 && tx <= 238 && ty >= 46 && ty <= 134) {
                    shop_choose_pack_card(&game, 2);
                    need_redraw = true;
                } else if (tx >= 76 && tx <= 180 && ty >= 146 && ty <= 186) {
                    game.current_state = STATE_SHOP; // Skip pack
                    need_redraw = true;
                }
            }
            // State: Game Over
            else if (game.current_state == STATE_GAME_OVER) {
                if (tx >= 20 && tx <= 236 && ty >= 126 && ty <= 186) {
                    game_session_start_run(&game);
                    game_update_selection_preview(&game);
                    need_redraw = true;
                }
            }
        }

        // --- Physical DS Buttons Handling ---
        // Button A
        if (input.btn_a) {
            if (game.current_state == STATE_ROUND_ACTIVE) {
                if (game_play_selected_hand(&game)) need_redraw = true;
            } else if (game.current_state == STATE_ROUND_VICTORY) {
                game_advance_to_shop(&game);
                need_redraw = true;
            } else if (game.current_state == STATE_SHOP) {
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
            if (game_discard_selected_cards(&game)) need_redraw = true;
        }

        // Button X / L: Sort Rank
        if ((input.btn_x || input.btn_l) && game.current_state == STATE_ROUND_ACTIVE) {
            deck_sort_by_rank(&game.deck);
            game_update_selection_preview(&game);
            need_redraw = true;
        }

        // Button Y / R: Sort Suit
        if ((input.btn_y || input.btn_r) && game.current_state == STATE_ROUND_ACTIVE) {
            deck_sort_by_suit(&game.deck);
            game_update_selection_preview(&game);
            need_redraw = true;
        }

        // Button Select: Cycle Consumable selection
        if (input.btn_select && game.current_state == STATE_ROUND_ACTIVE) {
            if (game.consumable_count > 0) {
                game.selected_consumable_slot = (game.selected_consumable_slot + 1) % MAX_CONSUMABLES_SLOTS;
                need_redraw = true;
            }
        }

        // Button Start: Next Blind / Advance
        if (input.btn_start) {
            if (game.current_state == STATE_ROUND_VICTORY) {
                game_advance_to_shop(&game);
                need_redraw = true;
            } else if (game.current_state == STATE_SHOP) {
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
