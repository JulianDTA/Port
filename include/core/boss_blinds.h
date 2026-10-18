#ifndef BOSS_BLINDS_H
#define BOSS_BLINDS_H

#include <stdint.h>
#include <stdbool.h>
#include "core/card.h"
#include "core/poker_hand.h"

typedef enum {
    BOSS_NONE = 0,
    BOSS_THE_HOOK,      // Discards 2 random cards per hand played
    BOSS_THE_OX,        // Playing most played hand sets money to $0
    BOSS_THE_CLUB,      // All Clubs debuffed
    BOSS_THE_GOAD,      // All Spades debuffed
    BOSS_THE_WINDOW,    // All Diamonds debuffed
    BOSS_THE_HEAD,      // All Hearts debuffed
    BOSS_THE_PSYCHIC,   // Must play exactly 5 cards
    BOSS_THE_ARM,       // Decreases level of played poker hand
    BOSS_THE_WATER,     // Starts with 0 discards
    BOSS_THE_EYE,       // No repeat hand types this round
    BOSS_THE_MOUTH,     // Play only 1 hand type this round
    BOSS_THE_PLANT,     // All face cards debuffed
    BOSS_VIOLET_VESSEL, // Very large blind (3x base)
    BOSS_COUNT
} BossBlindId;

typedef struct {
    BossBlindId id;
    const char* name;
    const char* description;
    double score_mult; // Usually 2.0, or 3.0 for Violet Vessel
    int chip_frame;    // Frame index in BlindChips.png
} BossBlindInfo;

BossBlindInfo boss_blind_get_info(BossBlindId id);
BossBlindId boss_blind_pick_for_ante(int ante, uint32_t seed);
bool boss_blind_is_card_debuffed(BossBlindId id, const Card* card);

#endif // BOSS_BLINDS_H
