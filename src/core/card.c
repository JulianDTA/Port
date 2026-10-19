#include "core/card.h"

void card_init(Card* card, uint8_t id, CardSuit suit, CardRank rank) {
    if (!card) return;
    card->id = id;
    card->suit = suit;
    card->rank = rank;
    card->enhancement = ENHANCEMENT_NONE;
    card->edition = EDITION_NONE;
    card->seal = SEAL_NONE;
    card->is_face = (rank == RANK_JACK || rank == RANK_QUEEN || rank == RANK_KING);
    card->is_debuffed = false;
    card->is_selected = false;
}

int32_t card_get_base_chips(const Card* card) {
    if (!card) return 0;
    if (card->enhancement == ENHANCEMENT_STONE) return 50;

    switch (card->rank) {
        case RANK_2: return 2;
        case RANK_3: return 3;
        case RANK_4: return 4;
        case RANK_5: return 5;
        case RANK_6: return 6;
        case RANK_7: return 7;
        case RANK_8: return 8;
        case RANK_9: return 9;
        case RANK_10:
        case RANK_JACK:
        case RANK_QUEEN:
        case RANK_KING: return 10;
        case RANK_ACE: return 11;
        default: return 0;
    }
}

const char* card_suit_name(CardSuit suit) {
    switch (suit) {
        case SUIT_SPADES: return "Spades";
        case SUIT_HEARTS: return "Hearts";
        case SUIT_CLUBS: return "Clubs";
        case SUIT_DIAMONDS: return "Diamonds";
        default: return "None";
    }
}

const char* card_rank_name(CardRank rank) {
    switch (rank) {
        case RANK_2: return "2";
        case RANK_3: return "3";
        case RANK_4: return "4";
        case RANK_5: return "5";
        case RANK_6: return "6";
        case RANK_7: return "7";
        case RANK_8: return "8";
        case RANK_9: return "9";
        case RANK_10: return "10";
        case RANK_JACK: return "Jack";
        case RANK_QUEEN: return "Queen";
        case RANK_KING: return "King";
        case RANK_ACE: return "Ace";
        default: return "?";
    }
}

const char* card_get_enhancement_name(CardEnhancement enhancement) {
    switch (enhancement) {
        case ENHANCEMENT_BONUS: return "Bonus Card";
        case ENHANCEMENT_MULT: return "Mult Card";
        case ENHANCEMENT_WILD: return "Wild Card";
        case ENHANCEMENT_GLASS: return "Glass Card";
        case ENHANCEMENT_STEEL: return "Steel Card";
        case ENHANCEMENT_STONE: return "Stone Card";
        case ENHANCEMENT_GOLD: return "Gold Card";
        case ENHANCEMENT_LUCKY: return "Lucky Card";
        default: return "Standard";
    }
}

const char* card_get_enhancement_description(CardEnhancement enhancement) {
    switch (enhancement) {
        case ENHANCEMENT_BONUS: return "+30 extra Chips when played and scored.";
        case ENHANCEMENT_MULT: return "+4 Mult when played and scored.";
        case ENHANCEMENT_WILD: return "Can be used as any suit for Flushes.";
        case ENHANCEMENT_GLASS: return "x2 Mult when scored. 1 in 4 chance to destroy card.";
        case ENHANCEMENT_STEEL: return "x1.5 Mult while this card is held in hand.";
        case ENHANCEMENT_STONE: return "+50 Chips. Has no rank or suit.";
        case ENHANCEMENT_GOLD: return "Earn $3 cash if held in hand at end of round.";
        case ENHANCEMENT_LUCKY: return "1 in 5 chance for +20 Mult; 1 in 15 chance for $20.";
        default: return "No special enhancement applied.";
    }
}

const char* card_get_edition_name(CardEdition edition) {
    switch (edition) {
        case EDITION_FOIL: return "Foil";
        case EDITION_HOLOGRAPHIC: return "Holographic";
        case EDITION_POLYCHROME: return "Polychrome";
        case EDITION_NEGATIVE: return "Negative";
        default: return "Base";
    }
}

const char* card_get_edition_description(CardEdition edition) {
    switch (edition) {
        case EDITION_FOIL: return "+50 extra Chips.";
        case EDITION_HOLOGRAPHIC: return "+10 extra Mult.";
        case EDITION_POLYCHROME: return "x1.5 Mult.";
        case EDITION_NEGATIVE: return "+1 extra slot.";
        default: return "Base edition.";
    }
}

const char* card_get_seal_name(CardSeal seal) {
    switch (seal) {
        case SEAL_GOLD: return "Gold Seal";
        case SEAL_RED: return "Red Seal";
        case SEAL_BLUE: return "Blue Seal";
        case SEAL_PURPLE: return "Purple Seal";
        default: return "No Seal";
    }
}

const char* card_get_seal_description(CardSeal seal) {
    switch (seal) {
        case SEAL_GOLD: return "Earn $3 cash when played and scored.";
        case SEAL_RED: return "Retrigger this card 1 additional time.";
        case SEAL_BLUE: return "Creates a Planet card if held in hand at end of round.";
        case SEAL_PURPLE: return "Creates a Tarot card when discarded.";
        default: return "No seal attached.";
    }
}
