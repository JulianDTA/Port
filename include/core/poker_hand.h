#ifndef POKER_HAND_H
#define POKER_HAND_H

#include "core/card.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_PLAYED_CARDS 5

typedef enum {
    HAND_NONE = 0,
    HAND_HIGH_CARD,
    HAND_PAIR,
    HAND_TWO_PAIR,
    HAND_THREE_OF_A_KIND,
    HAND_STRAIGHT,
    HAND_FLUSH,
    HAND_FULL_HOUSE,
    HAND_FOUR_OF_A_KIND,
    HAND_STRAIGHT_FLUSH,
    HAND_FIVE_OF_A_KIND,
    HAND_FLUSH_HOUSE,
    HAND_FLUSH_FIVE
} HandType;

typedef struct {
    HandType type;
    int32_t level;
    int64_t base_chips;
    int64_t base_mult;
    int64_t chip_level_gain;
    int64_t mult_level_gain;
    const char* name;
} HandInfo;

typedef struct {
    HandType type;
    const Card* scoring_cards[MAX_PLAYED_CARDS];
    int scoring_count;
    int64_t initial_chips;
    int64_t initial_mult;
} HandEvaluation;

void poker_hand_init_table(void);
HandInfo poker_hand_get_info(HandType type, int32_t level);
HandEvaluation poker_evaluate_played(const Card* cards[], int count, const int32_t hand_levels[]);
const char* poker_hand_name(HandType type);

#endif // POKER_HAND_H
