#include "core/boss_blinds.h"
#include <string.h>

static const BossBlindInfo s_boss_blinds[BOSS_COUNT] = {
    {BOSS_NONE, "None", "No special effect", 2.0, 0},
    {BOSS_THE_HOOK, "The Hook", "Discards 2 random cards per hand played", 2.0, 2},
    {BOSS_THE_OX, "The Ox", "Playing your most played hand sets money to $0", 2.0, 3},
    {BOSS_THE_CLUB, "The Club", "All Club cards are debuffed", 2.0, 4},
    {BOSS_THE_GOAD, "The Goad", "All Spade cards are debuffed", 2.0, 5},
    {BOSS_THE_WINDOW, "The Window", "All Diamond cards are debuffed", 2.0, 6},
    {BOSS_THE_HEAD, "The Head", "All Heart cards are debuffed", 2.0, 7},
    {BOSS_THE_PSYCHIC, "The Psychic", "Must play exactly 5 cards", 2.0, 8},
    {BOSS_THE_ARM, "The Arm", "Decreases level of played poker hand", 2.0, 9},
    {BOSS_THE_WATER, "The Water", "Starts with 0 discards", 2.0, 10},
    {BOSS_THE_EYE, "The Eye", "No repeat hand types this round", 2.0, 11},
    {BOSS_THE_MOUTH, "The Mouth", "Play only 1 hand type this round", 2.0, 12},
    {BOSS_THE_PLANT, "The Plant", "All face cards are debuffed", 2.0, 13},
    {BOSS_VIOLET_VESSEL, "Violet Vessel", "Very large blind target (3X base)", 3.0, 14}
};

BossBlindInfo boss_blind_get_info(BossBlindId id) {
    if (id >= BOSS_COUNT) return s_boss_blinds[0];
    return s_boss_blinds[id];
}

BossBlindId boss_blind_pick_for_ante(int ante, uint32_t seed) {
    (void)ante;
    // Pick a boss between 1 and BOSS_COUNT-1
    if (BOSS_COUNT <= 1) return BOSS_NONE;
    int idx = 1 + (int)(seed % (BOSS_COUNT - 1));
    return (BossBlindId)idx;
}

bool boss_blind_is_card_debuffed(BossBlindId id, const Card* card) {
    if (!card) return false;
    switch (id) {
        case BOSS_THE_CLUB:
            return card->suit == SUIT_CLUBS || card->enhancement == ENHANCEMENT_WILD;
        case BOSS_THE_GOAD:
            return card->suit == SUIT_SPADES || card->enhancement == ENHANCEMENT_WILD;
        case BOSS_THE_WINDOW:
            return card->suit == SUIT_DIAMONDS || card->enhancement == ENHANCEMENT_WILD;
        case BOSS_THE_HEAD:
            return card->suit == SUIT_HEARTS || card->enhancement == ENHANCEMENT_WILD;
        case BOSS_THE_PLANT:
            return card_is_face(card);
        default:
            return false;
    }
}
