#include "core/poker_hand.h"
#include <stdlib.h>
#include <string.h>

static HandInfo s_hand_definitions[] = {
    { HAND_NONE,           1,   0,  0,  0, 0, "None" },
    { HAND_HIGH_CARD,      1,   5,  1, 10, 1, "High Card" },
    { HAND_PAIR,           1,  10,  2, 15, 1, "Pair" },
    { HAND_TWO_PAIR,       1,  20,  2, 20, 1, "Two Pair" },
    { HAND_THREE_OF_A_KIND,1,  30,  3, 20, 2, "Three of a Kind" },
    { HAND_STRAIGHT,       1,  30,  4, 30, 3, "Straight" },
    { HAND_FLUSH,          1,  35,  4, 15, 2, "Flush" },
    { HAND_FULL_HOUSE,     1,  40,  4, 25, 2, "Full House" },
    { HAND_FOUR_OF_A_KIND, 1,  60,  7, 30, 3, "Four of a Kind" },
    { HAND_STRAIGHT_FLUSH, 1, 100,  8, 40, 4, "Straight Flush" },
    { HAND_FIVE_OF_A_KIND, 1, 120, 12, 35, 3, "Five of a Kind" },
    { HAND_FLUSH_HOUSE,    1, 140, 14, 40, 4, "Flush House" },
    { HAND_FLUSH_FIVE,     1, 160, 16, 50, 3, "Flush Five" }
};

HandInfo poker_hand_get_info(HandType type, int32_t level) {
    if (level < 1) level = 1;
    for (size_t i = 0; i < sizeof(s_hand_definitions) / sizeof(s_hand_definitions[0]); ++i) {
        if (s_hand_definitions[i].type == type) {
            HandInfo info = s_hand_definitions[i];
            info.level = level;
            info.base_chips += (level - 1) * info.chip_level_gain;
            info.base_mult  += (level - 1) * info.mult_level_gain;
            return info;
        }
    }
    return s_hand_definitions[0];
}

const char* poker_hand_name(HandType type) {
    return poker_hand_get_info(type, 1).name;
}

static int compare_cards_by_rank_desc(const void* a, const void* b) {
    const Card* ca = *(const Card**)a;
    const Card* cb = *(const Card**)b;
    return (int)cb->rank - (int)ca->rank;
}

static bool check_flush(const Card* cards[], int count) {
    if (count < 5) return false;
    CardSuit base = SUIT_NONE;
    for (int i = 0; i < count; ++i) {
        if (cards[i]->enhancement == ENHANCEMENT_STONE) return false;
        if (cards[i]->enhancement == ENHANCEMENT_WILD) continue;
        if (base == SUIT_NONE) {
            base = cards[i]->suit;
        } else if (cards[i]->suit != base) {
            return false;
        }
    }
    return true;
}

static bool check_straight(const Card* sorted[], int count) {
    if (count < 5) return false;
    for (int i = 0; i < count; ++i) {
        if (sorted[i]->enhancement == ENHANCEMENT_STONE) return false;
    }
    // Check standard descending consecutive ranks
    bool regular = true;
    for (int i = 0; i < count - 1; ++i) {
        if (sorted[i]->rank != sorted[i + 1]->rank + 1) {
            regular = false;
            break;
        }
    }
    if (regular) return true;

    // Check Ace-low straight: Ace (14), 5, 4, 3, 2
    if (sorted[0]->rank == RANK_ACE &&
        sorted[1]->rank == RANK_5 &&
        sorted[2]->rank == RANK_4 &&
        sorted[3]->rank == RANK_3 &&
        sorted[4]->rank == RANK_2) {
        return true;
    }
    return false;
}

HandEvaluation poker_evaluate_played(const Card* cards[], int count, const int32_t hand_levels[]) {
    HandEvaluation eval;
    memset(&eval, 0, sizeof(eval));
    if (count == 0) return eval;

    const Card* sorted[MAX_PLAYED_CARDS];
    for (int i = 0; i < count; ++i) sorted[i] = cards[i];
    qsort((void*)sorted, count, sizeof(const Card*), compare_cards_by_rank_desc);

    // Count rank frequencies
    int rank_counts[15] = {0};
    for (int i = 0; i < count; ++i) {
        if (sorted[i]->enhancement != ENHANCEMENT_STONE) {
            rank_counts[sorted[i]->rank]++;
        }
    }

    int max_freq = 0;
    int second_freq = 0;
    CardRank max_rank = RANK_NONE;
    CardRank second_rank = RANK_NONE;

    for (int r = 14; r >= 2; --r) {
        int c = rank_counts[r];
        if (c > max_freq) {
            second_freq = max_freq;
            second_rank = max_rank;
            max_freq = c;
            max_rank = (CardRank)r;
        } else if (c > second_freq) {
            second_freq = c;
            second_rank = (CardRank)r;
        }
    }

    bool is_flush = check_flush(sorted, count);
    bool is_straight = check_straight(sorted, count);

    // Determine hand type & scoring cards
    if (max_freq == 5) {
        if (is_flush) eval.type = HAND_FLUSH_FIVE;
        else eval.type = HAND_FIVE_OF_A_KIND;
        for (int i = 0; i < count; ++i) eval.scoring_cards[eval.scoring_count++] = sorted[i];
    } else if (max_freq == 4) {
        eval.type = HAND_FOUR_OF_A_KIND;
        for (int i = 0; i < count; ++i) {
            if (sorted[i]->rank == max_rank) eval.scoring_cards[eval.scoring_count++] = sorted[i];
        }
    } else if (max_freq == 3 && second_freq >= 2) {
        if (is_flush) eval.type = HAND_FLUSH_HOUSE;
        else eval.type = HAND_FULL_HOUSE;
        for (int i = 0; i < count; ++i) {
            if (sorted[i]->rank == max_rank || sorted[i]->rank == second_rank) {
                eval.scoring_cards[eval.scoring_count++] = sorted[i];
            }
        }
    } else if (is_straight && is_flush) {
        eval.type = HAND_STRAIGHT_FLUSH;
        for (int i = 0; i < count; ++i) eval.scoring_cards[eval.scoring_count++] = sorted[i];
    } else if (is_flush) {
        eval.type = HAND_FLUSH;
        for (int i = 0; i < count; ++i) eval.scoring_cards[eval.scoring_count++] = sorted[i];
    } else if (is_straight) {
        eval.type = HAND_STRAIGHT;
        for (int i = 0; i < count; ++i) eval.scoring_cards[eval.scoring_count++] = sorted[i];
    } else if (max_freq == 3) {
        eval.type = HAND_THREE_OF_A_KIND;
        for (int i = 0; i < count; ++i) {
            if (sorted[i]->rank == max_rank) eval.scoring_cards[eval.scoring_count++] = sorted[i];
        }
    } else if (max_freq == 2 && second_freq == 2) {
        eval.type = HAND_TWO_PAIR;
        for (int i = 0; i < count; ++i) {
            if (sorted[i]->rank == max_rank || sorted[i]->rank == second_rank) {
                eval.scoring_cards[eval.scoring_count++] = sorted[i];
            }
        }
    } else if (max_freq == 2) {
        eval.type = HAND_PAIR;
        for (int i = 0; i < count; ++i) {
            if (sorted[i]->rank == max_rank) eval.scoring_cards[eval.scoring_count++] = sorted[i];
        }
    } else {
        eval.type = HAND_HIGH_CARD;
        // High card scores only the highest card
        if (count > 0) eval.scoring_cards[eval.scoring_count++] = sorted[0];
    }

    // Stone cards always score (+50 chips) alongside any played hand
    for (int i = 0; i < count; ++i) {
        if (sorted[i]->enhancement == ENHANCEMENT_STONE) {
            bool already_present = false;
            for (int s = 0; s < eval.scoring_count; ++s) {
                if (eval.scoring_cards[s] == sorted[i]) {
                    already_present = true;
                    break;
                }
            }
            if (!already_present && eval.scoring_count < MAX_PLAYED_CARDS) {
                eval.scoring_cards[eval.scoring_count++] = sorted[i];
            }
        }
    }

    int32_t lvl = (hand_levels != NULL) ? hand_levels[eval.type] : 1;
    HandInfo info = poker_hand_get_info(eval.type, lvl);
    eval.initial_chips = info.base_chips;
    eval.initial_mult  = info.base_mult;

    return eval;
}
