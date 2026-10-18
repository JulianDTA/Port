#include "core/scoring.h"
#include <math.h>

void score_state_init(ScoreState* state, int64_t base_chips, double base_mult) {
    if (!state) return;
    state->chips = (base_chips < 0) ? 0 : base_chips;
    state->mult = (base_mult < 1.0) ? 1.0 : base_mult;
    state->final_score = 0;
}

void score_state_add_chips(ScoreState* state, int64_t chips) {
    if (!state) return;
    state->chips += chips;
    if (state->chips < 0) state->chips = 0;
}

void score_state_add_mult(ScoreState* state, double mult) {
    if (!state) return;
    state->mult += mult;
    if (state->mult < 1.0) state->mult = 1.0;
}

void score_state_multiply_mult(ScoreState* state, double factor) {
    if (!state || factor <= 0.0) return;
    state->mult *= factor;
}

int64_t score_state_calculate_final(ScoreState* state) {
    if (!state) return 0;
    // In Balatro, final score is floor(chips * mult)
    double total = (double)state->chips * state->mult;
    state->final_score = (int64_t)floor(total);
    return state->final_score;
}
