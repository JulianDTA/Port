#include "core/deck.h"
#include <string.h>
#include <stdlib.h>

void deck_init_standard(DeckManager* dm) {
    if (!dm) return;
    memset(dm, 0, sizeof(DeckManager));
    dm->hand_size_limit = 8;

    uint8_t id = 0;
    for (int suit = 0; suit < 4; ++suit) {
        for (int rank = 2; rank <= 14; ++rank) {
            card_init(&dm->draw_pile.cards[dm->draw_pile.count++], id++, (CardSuit)suit, (CardRank)rank);
        }
    }
}

// Simple xorshift32 PRNG suitable for fast execution on ARM9
static uint32_t s_rng_state = 0x12345678;
static uint32_t xorshift32(void) {
    uint32_t x = s_rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    s_rng_state = x;
    return x;
}

void deck_shuffle(CardCollection* col, uint32_t seed) {
    if (!col || col->count <= 1) return;
    if (seed != 0) s_rng_state = seed;

    // Fisher-Yates shuffle
    for (int i = col->count - 1; i > 0; --i) {
        int j = (int)(xorshift32() % (uint32_t)(i + 1));
        Card temp = col->cards[i];
        col->cards[i] = col->cards[j];
        col->cards[j] = temp;
    }
}

bool deck_draw_card(DeckManager* dm) {
    if (!dm || dm->draw_pile.count <= 0 || dm->hand.count >= MAX_HAND_SIZE) {
        return false;
    }
    // Draw top card from draw pile
    Card c = dm->draw_pile.cards[--dm->draw_pile.count];
    dm->hand.cards[dm->hand.count++] = c;
    return true;
}

void deck_draw_up_to_limit(DeckManager* dm) {
    if (!dm) return;
    while (dm->hand.count < dm->hand_size_limit && dm->draw_pile.count > 0) {
        deck_draw_card(dm);
    }
}

int deck_get_selected_cards(DeckManager* dm, const Card* out_selected[5]) {
    if (!dm) return 0;
    int count = 0;
    for (int i = 0; i < dm->hand.count && count < 5; ++i) {
        if (dm->hand.cards[i].is_selected) {
            out_selected[count++] = &dm->hand.cards[i];
        }
    }
    return count;
}

void deck_discard_selected(DeckManager* dm) {
    if (!dm) return;
    int new_hand_count = 0;
    for (int i = 0; i < dm->hand.count; ++i) {
        if (dm->hand.cards[i].is_selected) {
            dm->hand.cards[i].is_selected = false;
            if (dm->discard_pile.count < MAX_DECK_SIZE) {
                dm->discard_pile.cards[dm->discard_pile.count++] = dm->hand.cards[i];
            }
        } else {
            dm->hand.cards[new_hand_count++] = dm->hand.cards[i];
        }
    }
    dm->hand.count = new_hand_count;
}

void deck_remove_selected_from_hand(DeckManager* dm) {
    if (!dm) return;
    int new_hand_count = 0;
    for (int i = 0; i < dm->hand.count; ++i) {
        if (dm->hand.cards[i].is_selected) {
            dm->hand.cards[i].is_selected = false;
            // Played cards go to discard pile after scoring
            if (dm->discard_pile.count < MAX_DECK_SIZE) {
                dm->discard_pile.cards[dm->discard_pile.count++] = dm->hand.cards[i];
            }
        } else {
            dm->hand.cards[new_hand_count++] = dm->hand.cards[i];
        }
    }
    dm->hand.count = new_hand_count;
}

static int cmp_card_rank(const void* a, const void* b) {
    const Card* ca = (const Card*)a;
    const Card* cb = (const Card*)b;
    if (cb->rank != ca->rank) {
        return (int)cb->rank - (int)ca->rank;
    }
    return (int)ca->suit - (int)cb->suit;
}

static int cmp_card_suit(const void* a, const void* b) {
    const Card* ca = (const Card*)a;
    const Card* cb = (const Card*)b;
    if (ca->suit != cb->suit) {
        return (int)ca->suit - (int)cb->suit;
    }
    return (int)cb->rank - (int)ca->rank;
}

void deck_sort_by_rank(DeckManager* dm) {
    if (!dm || dm->hand.count <= 1) return;
    qsort(dm->hand.cards, dm->hand.count, sizeof(Card), cmp_card_rank);
}

void deck_sort_by_suit(DeckManager* dm) {
    if (!dm || dm->hand.count <= 1) return;
    qsort(dm->hand.cards, dm->hand.count, sizeof(Card), cmp_card_suit);
}
