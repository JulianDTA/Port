#ifndef JOKERS_H
#define JOKERS_H

#include "core/card.h"
#include "core/poker_hand.h"
#include "core/scoring.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_JOKERS_SLOTS 5

typedef enum {
    RARITY_COMMON = 0,
    RARITY_UNCOMMON,
    RARITY_RARE,
    RARITY_LEGENDARY
} JokerRarity;

typedef enum {
    JOKER_NONE = 0,
    JOKER_JOKER,             // +4 Mult
    JOKER_GREEDY,            // Played Diamonds give +3 Mult
    JOKER_LUSTY,             // Played Hearts give +3 Mult
    JOKER_WRATHFUL,          // Played Spades give +3 Mult
    JOKER_GLUTTONOUS,        // Played Clubs give +3 Mult
    JOKER_JOLLY,             // +8 Mult if played hand contains a Pair
    JOKER_ZANY,              // +12 Mult if played hand contains Three of a Kind
    JOKER_MAD,               // +10 Mult if played hand contains Two Pair
    JOKER_CRAZY,             // +12 Mult if played hand contains a Straight
    JOKER_DROLL,             // +10 Mult if played hand contains a Flush
    JOKER_SLY,               // +50 Chips if played hand contains a Pair
    JOKER_WILY,              // +100 Chips if played hand contains Three of a Kind
    JOKER_CLEVER,            // +80 Chips if played hand contains Two Pair
    JOKER_DEVIOUS,           // +100 Chips if played hand contains a Straight
    JOKER_CRAFTY,            // +80 Chips if played hand contains a Flush
    JOKER_HALF,              // +20 Mult if played hand contains 3 or fewer cards
    JOKER_BANNER,            // +30 Chips for each remaining discard
    JOKER_MYSTIC_SUMMIT,     // +15 Mult when 0 discards remaining
    JOKER_BARON,             // Each King held in hand gives x1.5 Mult
    JOKER_EVEN_STEVEN,       // Played cards with even rank give +4 Mult
    JOKER_ODD_TODD,          // Played cards with odd rank give +31 Chips
    JOKER_SCHOLAR,           // Played Aces give +20 Chips and +4 Mult
    JOKER_FIBONACCI,         // Played Ace, 2, 3, 5, 8 give +8 Mult
    JOKER_SCARY_FACE,        // Played face cards give +30 Chips
    JOKER_SMILEY_FACE,       // Played face cards give +5 Mult
    JOKER_ABSTRACT,          // +3 Mult for each Joker held
    JOKER_GROS_MICHEL,       // +15 Mult, 1 in 6 chance to go extinct each round
    JOKER_CAVENDISH,         // x3 Mult, 1 in 1000 chance to go extinct each round
    JOKER_BLUE_JOKER,        // +2 Chips for each card remaining in deck
    JOKER_ICE_CREAM,         // +100 Chips, -5 Chips per hand played
    JOKER_POPCORN,           // +20 Mult, -4 Mult per round
    JOKER_STENCIL,           // X1 Mult for each empty Joker slot
    JOKER_BLACKBOARD,        // X3 Mult if all cards held in hand are Spades or Clubs
    JOKER_THE_DUO,           // x2 Mult if played hand contains a Pair
    JOKER_THE_TRIO,          // x3 Mult if played hand contains Three of a Kind
    JOKER_COUNT
} JokerId;

typedef struct Joker Joker;

typedef struct {
    int remaining_discards;
    int remaining_hands;
    int current_dollars;
    int cards_remaining_in_deck;
    int jokers_count;
    int max_joker_slots;
    const Card* cards_in_hand[15];
    int cards_in_hand_count;
} RoundContext;

struct Joker {
    JokerId id;
    const char* name;
    const char* description;
    JokerRarity rarity;
    int32_t cost;
    int32_t sell_value;
    CardEdition edition;
    bool is_debuffed;
    int32_t custom_counter; // For stacking jokers (e.g. Ice Cream, Popcorn)

    // Callbacks
    void (*on_card_scored)(Joker* self, const Card* card, ScoreState* score);
    void (*on_held_in_hand)(Joker* self, const Card* card, ScoreState* score);
    void (*on_hand_evaluated)(Joker* self, HandType hand_type, int played_card_count, const RoundContext* ctx, ScoreState* score);
    void (*on_hand_played)(Joker* self, const RoundContext* ctx);
    void (*on_round_end)(Joker* self, bool* out_destroy);
};

void joker_create(Joker* joker, JokerId id, CardEdition edition);
const char* joker_get_description(JokerId id);
const char* joker_get_rarity_name(JokerRarity rarity);
void joker_trigger_card_scored(Joker* jokers[], int count, const Card* card, ScoreState* score);
void joker_trigger_held_in_hand(Joker* jokers[], int count, const Card* card, ScoreState* score);
void joker_trigger_hand_evaluated(Joker* jokers[], int count, HandType hand_type, int played_card_count, const RoundContext* ctx, ScoreState* score);
void joker_trigger_hand_played(Joker* jokers[], int count, const RoundContext* ctx);
void joker_trigger_round_end(Joker* jokers[], int* inout_count);

#endif // JOKERS_H
