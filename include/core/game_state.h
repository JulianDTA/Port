#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdint.h>
#include <stdbool.h>
#include "core/card.h"
#include "core/poker_hand.h"
#include "core/scoring.h"
#include "core/jokers.h"
#include "core/deck.h"
#include "core/consumable.h"
#include "core/boss_blinds.h"
#include "core/shop.h"

typedef enum {
    STATE_TITLE = 0,
    STATE_ROUND_ACTIVE,
    STATE_ROUND_SCORING,
    STATE_ROUND_VICTORY,
    STATE_SHOP,
    STATE_BOOSTER_OPEN,
    STATE_GAME_OVER
} GameStateType;

typedef enum {
    BLIND_SMALL = 0,
    BLIND_BIG,
    BLIND_BOSS
} BlindType;

typedef enum {
    SORT_MODE_NONE = 0,
    SORT_MODE_RANK,
    SORT_MODE_SUIT
} HandSortMode;

typedef struct GameSession {
    GameStateType current_state;
    HandSortMode active_sort_mode;
    int ante;
    BlindType current_blind;
    BossBlindId current_boss;
    int64_t target_score;
    int64_t current_round_score;

    int dollars;
    int hands_left;
    int discards_left;
    int max_hands;
    int max_discards;

    DeckManager deck;

    // Jokers
    Joker jokers_storage[MAX_JOKERS_SLOTS];
    Joker* jokers[MAX_JOKERS_SLOTS];
    int joker_count;

    // Consumables (Tarots & Planets)
    Consumable consumables[MAX_CONSUMABLES_SLOTS];
    int consumable_count;
    Consumable last_used_consumable;
    int selected_consumable_slot; // -1 if none, 0 or 1 if inspecting/using

    // Poker hand levels (1-indexed, HandType 1..12)
    int32_t hand_levels[13];
    int32_t hand_play_counts[13];

    // Shop
    ShopState shop;

    // Last played scoring animation info
    HandEvaluation last_eval;
    ScoreState last_score;
    int scoring_step;
    int scoring_timer;

    // Real-time dynamic selection preview (before pressing Play)
    HandEvaluation preview_eval;
    ScoreState preview_score;
    int round_payout;

    // Active round Joker Inspector & Management
    bool inspecting_jokers;
    int inspected_joker_idx;

    uint32_t rng_state;
} GameSession;

void game_session_init(GameSession* game);
void game_session_start_run(GameSession* game);
void game_session_select_blind(GameSession* game, BlindType blind);
int64_t game_calculate_blind_target(int ante, BlindType blind, BossBlindId boss);
void game_update_selection_preview(GameSession* game);
bool game_play_selected_hand(GameSession* game);
bool game_discard_selected_cards(GameSession* game);
void game_advance_to_shop(GameSession* game);
void game_advance_next_blind(GameSession* game);
bool game_sell_joker(GameSession* game, int joker_index);
bool game_sell_consumable(GameSession* game, int consumable_index);
bool game_swap_jokers(GameSession* game, int idx1, int idx2);
void game_tick_scoring(GameSession* game);
void game_finish_scoring(GameSession* game);

#endif // GAME_STATE_H
