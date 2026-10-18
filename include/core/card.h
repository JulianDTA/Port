#ifndef CARD_H
#define CARD_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SUIT_SPADES = 0,
    SUIT_HEARTS = 1,
    SUIT_CLUBS = 2,
    SUIT_DIAMONDS = 3,
    SUIT_NONE = 4 // For Stone cards
} CardSuit;

typedef enum {
    RANK_NONE = 0,
    RANK_2 = 2,
    RANK_3 = 3,
    RANK_4 = 4,
    RANK_5 = 5,
    RANK_6 = 6,
    RANK_7 = 7,
    RANK_8 = 8,
    RANK_9 = 9,
    RANK_10 = 10,
    RANK_JACK = 11,
    RANK_QUEEN = 12,
    RANK_KING = 13,
    RANK_ACE = 14
} CardRank;

typedef enum {
    ENHANCEMENT_NONE = 0,
    ENHANCEMENT_BONUS,   // +30 chips
    ENHANCEMENT_MULT,    // +4 mult
    ENHANCEMENT_WILD,    // Counts as any suit
    ENHANCEMENT_GLASS,   // x2 mult, 1 in 4 chance to break
    ENHANCEMENT_STEEL,   // x1.5 mult when held in hand
    ENHANCEMENT_STONE,   // +50 chips, no rank/suit
    ENHANCEMENT_GOLD,    // $3 if held in hand at end of round
    ENHANCEMENT_LUCKY    // 1/5 for +20 mult, 1/15 for $20
} CardEnhancement;

typedef enum {
    EDITION_NONE = 0,
    EDITION_FOIL,        // +50 chips
    EDITION_HOLOGRAPHIC, // +10 mult
    EDITION_POLYCHROME,  // x1.5 mult
    EDITION_NEGATIVE     // +1 slot (for Jokers)
} CardEdition;

typedef enum {
    SEAL_NONE = 0,
    SEAL_GOLD,   // $3 when played and scored
    SEAL_RED,    // Retrigger 1 time
    SEAL_BLUE,   // Planet card if held in hand
    SEAL_PURPLE  // Tarot card if discarded
} CardSeal;

typedef struct {
    uint8_t id;
    CardSuit suit;
    CardRank rank;
    CardEnhancement enhancement;
    CardEdition edition;
    CardSeal seal;
    bool is_face;
    bool is_debuffed;
    bool is_selected;
} Card;

// Helper functions
void card_init(Card* card, uint8_t id, CardSuit suit, CardRank rank);
int32_t card_get_base_chips(const Card* card);
const char* card_suit_name(CardSuit suit);
const char* card_rank_name(CardRank rank);

#endif // CARD_H
