#ifndef SCORING_H
#define SCORING_H

#include "core/card.h"
#include "core/poker_hand.h"
#include <stdint.h>

typedef struct {
    int64_t chips;
    double mult;
    int64_t final_score;
} ScoreState;

void score_state_init(ScoreState* state, int64_t base_chips, double base_mult);
void score_state_add_chips(ScoreState* state, int64_t chips);
void score_state_add_mult(ScoreState* state, double mult);
void score_state_multiply_mult(ScoreState* state, double factor);
int64_t score_state_calculate_final(ScoreState* state);

#endif // SCORING_H
