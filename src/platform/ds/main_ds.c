#ifdef ARM9
#include <nds.h>
#endif
#include <stdio.h>
#include <string.h>
#include "platform/hal.h"
#include "core/game_state.h"
#include "core/card.h"
#include "core/poker_hand.h"

#ifdef ARM9
static PrintConsole s_top_console;
static PrintConsole s_bottom_console;
static touchPosition s_touch;

void hal_init(void) {
    // Set 2D video mode on both main (top) and sub (bottom) engines
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    // Map VRAM banks for text console backgrounds
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    // Initialize text consoles on top and bottom screens
    consoleInit(&s_top_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&s_bottom_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
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

// Render Top Screen: HUD, Blind, Goal, Jokers, Score breakdown
static void render_top_screen(const GameSession* game) {
#ifdef ARM9
    consoleSelect(&s_top_console);
    iprintf("\x1b[2J"); // Clear screen
    iprintf("\x1b[1;2H*** BALATRO DSi ***");
    iprintf("\x1b[3;2HAnte: %d/8   Round: %s", game->ante,
            (game->current_blind == BLIND_SMALL ? "Small" : (game->current_blind == BLIND_BIG ? "Big" : "Boss")));

    iprintf("\x1b[5;2HGoal : %lld", (long long)game->target_score);
    iprintf("\x1b[6;2HScore: %lld", (long long)game->current_round_score);
    iprintf("\x1b[8;2HMoney: $%d", game->dollars);
    iprintf("\x1b[9;2HHands Left: %d  Discards: %d", game->hands_left, game->discards_left);

    // Active Jokers
    iprintf("\x1b[11;2H--- JOKERS (%d/5) ---", game->joker_count);
    for (int i = 0; i < game->joker_count; ++i) {
        if (game->jokers[i]) {
            iprintf("\x1b[%d;3H[%d] %s", 13 + i, i + 1, game->jokers[i]->name);
        }
    }

    if (game->last_score.final_score > 0) {
        iprintf("\x1b[19;2HLast Hand: %s", poker_hand_name(game->last_eval.type));
        iprintf("\x1b[20;2H+%lld Chips X %.1f Mult = %lld",
                (long long)game->last_score.chips, game->last_score.mult, (long long)game->last_score.final_score);
    }
#endif
}

// Render Bottom Screen: Cards in Hand, Touch buttons
static void render_bottom_screen(const GameSession* game) {
#ifdef ARM9
    consoleSelect(&s_bottom_console);
    iprintf("\x1b[2J"); // Clear screen
    iprintf("\x1b[1;2H--- YOUR HAND (Touch to select) ---");

    for (int i = 0; i < game->deck.hand.count; ++i) {
        const Card* c = &game->deck.hand.cards[i];
        char marker = c->is_selected ? '*' : ' ';
        iprintf("\x1b[%d;2H[%c] %2d: %s of %s", 3 + i, marker, i + 1, card_rank_name(c->rank), card_suit_name(c->suit));
    }

    // Touch Buttons at the bottom
    iprintf("\x1b[18;2H==============================");
    iprintf("\x1b[20;2H[ (A) PLAY HAND ]  [ (B) DISCARD ]");
    iprintf("\x1b[22;2HStylus: Touch card row to toggle");
#endif
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    hal_init();

    GameSession game;
    game_session_init(&game);
    game_session_start_run(&game);

    PlatformInput input;
    memset(&input, 0, sizeof(input));

    while (1) {
        hal_update_input(&input);

        // Handle Touch Card Selection on Bottom Screen (lines 3 to 3 + hand.count)
        if (input.touch_just_pressed) {
            int touch_row = input.touch_y / 8; // 8 pixels per text row in console
            int card_idx = touch_row - 3;
            if (card_idx >= 0 && card_idx < game.deck.hand.count) {
                // Count currently selected cards
                const Card* sel[5];
                int current_sel = deck_get_selected_cards(&game.deck, sel);
                if (game.deck.hand.cards[card_idx].is_selected) {
                    game.deck.hand.cards[card_idx].is_selected = false;
                } else if (current_sel < 5) {
                    game.deck.hand.cards[card_idx].is_selected = true;
                }
            }

            // Touch buttons for Play (row 19-21, left) and Discard (row 19-21, right)
            if (touch_row >= 19 && touch_row <= 21) {
                if (input.touch_x < 128) {
                    input.btn_a = true; // Play
                } else {
                    input.btn_b = true; // Discard
                }
            }
        }

        // Button A: Play Hand
        if (input.btn_a) {
            game_play_selected_hand(&game);
        }

        // Button B: Discard Hand
        if (input.btn_b) {
            game_discard_selected_cards(&game);
        }

        // Render current game state
        render_top_screen(&game);
        render_bottom_screen(&game);

        hal_wait_vsync();
    }

    return 0;
}
