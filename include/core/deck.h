#ifndef DECK_H
#define DECK_H

#include "core/card.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_DECK_SIZE 100
#define MAX_HAND_SIZE 15

typedef struct {
    Card cards[MAX_DECK_SIZE];
    int count;
} CardCollection;

typedef struct {
    CardCollection draw_pile;
    CardCollection hand;
    CardCollection discard_pile;
    int hand_size_limit; // Default 8
} DeckManager;

void deck_init_standard(DeckManager* dm);
void deck_shuffle(CardCollection* col, uint32_t seed);
bool deck_draw_card(DeckManager* dm);
void deck_draw_up_to_limit(DeckManager* dm);
void deck_discard_selected(DeckManager* dm);
int deck_get_selected_cards(DeckManager* dm, const Card* out_selected[5]);
void deck_remove_selected_from_hand(DeckManager* dm);
void deck_destroy_selected(DeckManager* dm);
void deck_sort_by_rank(DeckManager* dm);
void deck_sort_by_suit(DeckManager* dm);

#endif // DECK_H
