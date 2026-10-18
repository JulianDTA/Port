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

        // --- Active Scoring Animation State ---
        if (game.current_state == STATE_ROUND_SCORING) {
            if (input.btn_a || input.touch_just_pressed) {
                game_finish_scoring(&game);
            } else {
                game_tick_scoring(&game);
            }
            need_redraw = true;
        }
        // --- Stylus Touch Controls on Bottom Screen ---
        else if (input.touch_just_pressed) {
            int tx = input.touch_x;
            int ty = input.touch_y;

            // State: Round Active
            if (game.current_state == STATE_ROUND_ACTIVE) {
                // 1. Sort Buttons
                if (tx >= 2 && tx <= 58 && ty >= 2 && ty <= 24) {
                    deck_sort_by_rank(&game.deck);
                    game_update_selection_preview(&game);
                    need_redraw = true;
                } else if (tx >= 60 && tx <= 118 && ty >= 2 && ty <= 24) {
                    deck_sort_by_suit(&game.deck);
                    game_update_selection_preview(&game);
                    need_redraw = true;
                }
                // 2. Consumable Slot 0
                else if (tx >= 120 && tx <= 152 && ty >= 2 && ty <= 40) {
                    if (game.consumables[0].type != CONSUMABLE_NONE) {
                        if (game.selected_consumable_slot == 0) game.selected_consumable_slot = -1;
                        else game.selected_consumable_slot = 0;
                        need_redraw = true;
                    }
                }
                // 3. Consumable Slot 1
                else if (tx >= 152 && tx <= 184 && ty >= 2 && ty <= 40) {
                    if (game.consumables[1].type != CONSUMABLE_NONE) {
                        if (game.selected_consumable_slot == 1) game.selected_consumable_slot = -1;
                        else game.selected_consumable_slot = 1;
                        need_redraw = true;
                    }
                }
                // 4. [USE CARD] Button
                else if (game.selected_consumable_slot >= 0 && tx >= 184 && tx <= 254 && ty >= 2 && ty <= 20) {
                    if (consumable_use(&game, game.selected_consumable_slot)) {
                        need_redraw = true;
                    }
                }
                // 5. [SELL $1] Button
                else if (game.selected_consumable_slot >= 0 && tx >= 184 && tx <= 254 && ty >= 21 && ty <= 40) {
                    if (game_sell_consumable(&game, game.selected_consumable_slot)) {
                        need_redraw = true;
                    }
                }
                // 6. Cards in Hand
                else if (ty >= 44 && ty <= 130) {
                    int count = game.deck.hand.count;
                    int total_w = count * 31 - 3;
                    int start_x = (DS_SCREEN_W - total_w) / 2;
                    if (start_x < 2) start_x = 2;

                    for (int i = 0; i < count; ++i) {
                        int cx = start_x + (i * 31);
                        if (tx >= cx - 2 && tx <= cx + 29) {
                            const Card* sel[5];
                            int sel_cnt = deck_get_selected_cards(&game.deck, sel);
                            if (game.deck.hand.cards[i].is_selected) {
                                game.deck.hand.cards[i].is_selected = false;
                                game_update_selection_preview(&game);
                                need_redraw = true;
                            } else if (sel_cnt < 5) {
                                game.deck.hand.cards[i].is_selected = true;
                                game_update_selection_preview(&game);
                                need_redraw = true;
                            }
                            break;
                        }
                    }
                }
                // 7. [PLAY HAND] Button
                else if (tx >= 6 && tx <= 126 && ty >= 134 && ty <= 188) {
                    if (game_play_selected_hand(&game)) need_redraw = true;
                }
                // 8. [DISCARD] Button
                else if (tx >= 130 && tx <= 250 && ty >= 134 && ty <= 188) {
                    if (game_discard_selected_cards(&game)) need_redraw = true;
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
                if (!game.shop.show_inventory) {
                    // Buy Slot 0 (Joker)
                    if (tx >= 6 && tx <= 80 && ty >= 8 && ty <= 90) {
                        if (shop_buy_card_slot(&game, 0)) need_redraw = true;
                    }
                    // Buy Slot 1 (Tarot/Planet)
                    else if (tx >= 82 && tx <= 156 && ty >= 8 && ty <= 90) {
                        if (shop_buy_card_slot(&game, 1)) need_redraw = true;
                    }
                    // Open Booster Pack
                    else if (tx >= 158 && tx <= 250 && ty >= 8 && ty <= 90) {
                        if (shop_buy_pack(&game)) need_redraw = true;
                    }
                    // Buy Voucher
                    else if (tx >= 6 && tx <= 156 && ty >= 92 && ty <= 136) {
                        if (shop_buy_voucher(&game)) need_redraw = true;
                    }
                    // Open Inventory Sell Screen
                    else if (tx >= 158 && tx <= 250 && ty >= 92 && ty <= 136) {
                        game.shop.show_inventory = true;
                        need_redraw = true;
                    }
                    // Reroll Shop
                    else if (tx >= 6 && tx <= 120 && ty >= 138 && ty <= 188) {
                        if (shop_reroll(&game)) need_redraw = true;
                    }
                    // Next Blind
                    else if (tx >= 122 && tx <= 250 && ty >= 138 && ty <= 188) {
                        game_advance_next_blind(&game);
                        need_redraw = true;
                    }
                } else {
                    // Back to Shop / Done Button
                    if ((tx >= 4 && tx <= 104 && ty >= 2 && ty <= 26) ||
                        (tx >= 20 && tx <= 236 && ty >= 150 && ty <= 188)) {
                        game.shop.show_inventory = false;
                        need_redraw = true;
                    }
                    // Sell Jokers (5 slots)
                    for (int j = 0; j < 5; ++j) {
                        int jx = 4 + j * 50;
                        if (tx >= jx && tx <= jx + 48 && ty >= 36 && ty <= 90) {
                            if (game_sell_joker(&game, j)) need_redraw = true;
                            break;
                        }
                    }
                    // Sell Consumables (2 slots)
                    for (int c = 0; c < 2; ++c) {
                        int cx = 8 + c * 124;
                        if (tx >= cx && tx <= cx + 116 && ty >= 104 && ty <= 148) {
                            if (game_sell_consumable(&game, c)) need_redraw = true;
                            break;
                        }
                    }
                }
            }
            // State: Booster Pack Open
            else if (game.current_state == STATE_BOOSTER_OPEN) {
                if (tx >= 16 && tx <= 88 && ty >= 44 && ty <= 136) {
                    if (shop_choose_pack_card(&game, 0)) need_redraw = true;
                } else if (tx >= 92 && tx <= 164 && ty >= 44 && ty <= 136) {
                    if (shop_choose_pack_card(&game, 1)) need_redraw = true;
                } else if (tx >= 168 && tx <= 240 && ty >= 44 && ty <= 136) {
                    if (shop_choose_pack_card(&game, 2)) need_redraw = true;
                } else if (tx >= 72 && tx <= 184 && ty >= 144 && ty <= 186) {
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
            if (game.current_state == STATE_ROUND_SCORING) {
                game_finish_scoring(&game);
                need_redraw = true;
            } else if (game.current_state == STATE_ROUND_ACTIVE) {
                if (game_play_selected_hand(&game)) need_redraw = true;
            } else if (game.current_state == STATE_ROUND_VICTORY) {
                game_advance_to_shop(&game);
                need_redraw = true;
            } else if (game.current_state == STATE_SHOP) {
                if (game.shop.show_inventory) {
                    game.shop.show_inventory = false;
                } else {
                    game_advance_next_blind(&game);
                }
                need_redraw = true;
            } else if (game.current_state == STATE_GAME_OVER) {
                game_session_start_run(&game);
                game_update_selection_preview(&game);
                need_redraw = true;
            }
        }

        // Button B: Discard (or Back from Shop Inventory)
        if (input.btn_b) {
            if (game.current_state == STATE_ROUND_ACTIVE) {
                if (game_discard_selected_cards(&game)) need_redraw = true;
            } else if (game.current_state == STATE_SHOP && game.shop.show_inventory) {
                game.shop.show_inventory = false;
                need_redraw = true;
            }
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
                if (game.selected_consumable_slot < 0) {
                    game.selected_consumable_slot = (game.consumables[0].type != CONSUMABLE_NONE) ? 0 : 1;
                } else if (game.selected_consumable_slot == 0) {
                    game.selected_consumable_slot = (game.consumables[1].type != CONSUMABLE_NONE) ? 1 : -1;
                } else {
                    game.selected_consumable_slot = -1;
                }
                need_redraw = true;
            }
        }

        // Button Start: Next Blind / Advance
        if (input.btn_start) {
            if (game.current_state == STATE_ROUND_VICTORY) {
                game_advance_to_shop(&game);
                need_redraw = true;
            } else if (game.current_state == STATE_SHOP) {
                if (game.shop.show_inventory) {
                    game.shop.show_inventory = false;
                } else {
                    game_advance_next_blind(&game);
                }
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
