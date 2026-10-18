#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/card.h"
#include "core/poker_hand.h"
#include "core/scoring.h"
#include "core/jokers.h"
#include "core/deck.h"
#include "core/game_state.h"

static const char* suit_symbol(CardSuit suit) {
    switch (suit) {
        case SUIT_HEARTS:   return "H";
        case SUIT_DIAMONDS: return "D";
        case SUIT_CLUBS:    return "C";
        case SUIT_SPADES:   return "S";
        default:            return "?";
    }
}

static void print_card_box(const Card* c, int index) {
    char sel = c->is_selected ? '*' : ' ';
    printf("[%c%d] %2s%s ", sel, index, card_rank_name(c->rank), suit_symbol(c->suit));
}

static void display_game_screen(const GameSession* game) {
    printf("\n============================================================\n");
    printf("   BALATRO (Nintendo DSi Core Engine Simulator)\n");
    printf("============================================================\n");
    printf(" Ante: %d/8  |  Blind: %-5s  |  Goal: %-10lld  |  Score: %lld\n",
           game->ante,
           (game->current_blind == BLIND_SMALL ? "Small" : (game->current_blind == BLIND_BIG ? "Big" : "Boss")),
           (long long)game->target_score, (long long)game->current_round_score);
    printf(" Money: $%d  |  Hands: %d/%d     |  Discards: %d/%d\n",
           game->dollars, game->hands_left, game->max_hands, game->discards_left, game->max_discards);
    printf("------------------------------------------------------------\n");

    printf(" Jokers (%d/%d): ", game->joker_count, MAX_JOKERS_SLOTS);
    if (game->joker_count == 0) {
        printf("(None)\n");
    } else {
        printf("\n");
        for (int i = 0; i < game->joker_count; ++i) {
            if (game->jokers[i]) {
                printf("   [%d] %-16s (Cost: $%d)\n", i + 1, game->jokers[i]->name, game->jokers[i]->cost);
            }
        }
    }
    printf("------------------------------------------------------------\n");

    if (game->last_score.final_score > 0) {
        printf(" Last Played: %s\n", poker_hand_name(game->last_eval.type));
        printf(" Base: %lld x %lld | Card + Joker effects -> Chips: %lld, Mult: %.1f\n",
               (long long)game->last_eval.initial_chips, (long long)game->last_eval.initial_mult,
               (long long)game->last_score.chips, game->last_score.mult);
        printf(" -> Hand Total: +%lld Points!\n", (long long)game->last_score.final_score);
        printf("------------------------------------------------------------\n");
    }

    printf(" Hand (%d cards):\n  ", game->deck.hand.count);
    for (int i = 0; i < game->deck.hand.count; ++i) {
        print_card_box(&game->deck.hand.cards[i], i + 1);
        if ((i + 1) % 4 == 0 && (i + 1) < game->deck.hand.count) printf("\n  ");
    }
    printf("\n============================================================\n");
    printf(" Commands:\n");
    printf("   p 1 2 3 ...  -> Play selected card indices (up to 5)\n");
    printf("   d 1 2 ...    -> Discard selected card indices (up to 5)\n");
    printf("   q            -> Quit game\n");
    printf(" > ");
    fflush(stdout);
}

int main(void) {
    GameSession game;
    game_session_init(&game);
    game_session_start_run(&game);

    // Equip 2 fun starting Jokers for testing
    Joker cavendish, lusty;
    joker_create(&cavendish, JOKER_CAVENDISH, EDITION_NONE);
    joker_create(&lusty, JOKER_LUSTY, EDITION_NONE);
    game.jokers[0] = &cavendish;
    game.jokers[1] = &lusty;
    game.joker_count = 2;

    char line[256];
    while (game.current_state == STATE_ROUND_ACTIVE) {
        display_game_screen(&game);

        if (!fgets(line, sizeof(line), stdin)) break;

        // Parse command
        char cmd = ' ';
        int indices[5] = {0};
        int parsed_count = 0;

        char* token = strtok(line, " \t\r\n");
        if (!token) continue;

        cmd = token[0];
        if (cmd == 'q' || cmd == 'Q') {
            printf("\nExiting game. Thanks for playing Balatro-DSi!\n");
            break;
        }

        while ((token = strtok(NULL, " \t\r\n")) != NULL && parsed_count < 5) {
            int idx = atoi(token);
            if (idx >= 1 && idx <= game.deck.hand.count) {
                indices[parsed_count++] = idx - 1;
            }
        }

        if (parsed_count == 0) {
            printf("\n[!] Please specify card numbers (e.g. 'p 1 2 3' or 'd 4 5')\n");
            continue;
        }

        // Reset selections
        for (int i = 0; i < game.deck.hand.count; ++i) {
            game.deck.hand.cards[i].is_selected = false;
        }
        for (int i = 0; i < parsed_count; ++i) {
            game.deck.hand.cards[indices[i]].is_selected = true;
        }

        if (cmd == 'p' || cmd == 'P') {
            game_play_selected_hand(&game);
            if (game.current_state == STATE_ROUND_VICTORY) {
                printf("\n============================================================\n");
                printf("   ROUND DEFEATED! VICTORY!\n");
                printf("   Earned reward payout! Current Money: $%d\n", game.dollars);
                printf("============================================================\n");

                // Progress to next blind
                if (game.current_blind == BLIND_SMALL) {
                    game_session_select_blind(&game, BLIND_BIG);
                } else if (game.current_blind == BLIND_BIG) {
                    game_session_select_blind(&game, BLIND_BOSS);
                } else {
                    game.ante++;
                    if (game.ante > 8) {
                        printf("\n*** CONGRATULATIONS! YOU WON THE RUN (ANTE 8 COMPLETE)! ***\n");
                        break;
                    }
                    game_session_select_blind(&game, BLIND_SMALL);
                }
            } else if (game.current_state == STATE_GAME_OVER) {
                printf("\n============================================================\n");
                printf("   GAME OVER - Ran out of hands!\n");
                printf("   Final Ante: %d | Final Round Score: %lld / %lld\n",
                       game.ante, (long long)game.current_round_score, (long long)game.target_score);
                printf("============================================================\n");
                break;
            }
        } else if (cmd == 'd' || cmd == 'D') {
            if (game.discards_left <= 0) {
                printf("\n[!] No discards remaining!\n");
            } else {
                game_discard_selected_cards(&game);
            }
        } else {
            printf("\n[!] Unknown command. Use 'p' to play or 'd' to discard.\n");
        }
    }

    return 0;
}
