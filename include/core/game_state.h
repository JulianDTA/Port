#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "core/card.h"
#include "core/poker_hand.h"
#include "core/scoring.h"
#include "core/jokers.h"
#include "core/deck.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    STATE_TITLE = 0,
    STATE_BLIND_SELECT,
    STATE_ROUND_ACTIVE,
    STATE_ROUND_SCORING,
    STATE_ROUND_VICTORY,
    STATE_SHOP,
    STATE_GAME_OVER
} GameStateType;

typedef enum {
    BLIND_SMALL = 0,
    BLIND_BIG,
    BLIND_BOSS
} BlindType;

typedef struct {
    GameStateType current_state;
    int ante;
    BlindType current_blind;
    int64_t target_score;
    int64_t current_round_score;

    int dollars;
    int hands_left;
    int discards_left;
    int max_hands;
    int max_discards;

    DeckManager deck;
    Joker* jokers[MAX_JOKERS_SLOTS];
    int joker_count;

    int32_t hand_levels[13]; // Level for each HandType

    // Last played scoring animation info
    HandEvaluation last_eval;
    ScoreState last_score;

    // Real-time dynamic selection preview (before pressing Play)
    HandEvaluation preview_eval;
    ScoreState preview_score;
    int round_payout;
} GameSession;

void game_session_init(GameSession* game);
void game_session_start_run(GameSession* game);
void game_session_select_blind(GameSession* game, BlindType blind);
int64_t game_calculate_blind_target(int ante, BlindType blind);
void game_update_selection_preview(GameSession* game);
bool game_play_selected_hand(GameSession* game);
bool game_discard_selected_cards(GameSession* game);
void game_advance_next_blind(GameSession* game);

#endif // GAME_STATE_H
