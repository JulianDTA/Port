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
