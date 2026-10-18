#include "core/consumable.h"
#include "core/game_state.h"
#include <string.h>
#include <stdlib.h>

static const struct {
    TarotId id;
    const char* name;
    const char* desc;
    int min_sel;
    int max_sel;
} s_tarot_info[TAROT_COUNT] = {
    {TAROT_FOOL, "The Fool", "Spawns last used Tarot/Planet", 0, 0},
    {TAROT_MAGICIAN, "The Magician", "Enhances up to 2 cards to Lucky", 1, 2},
    {TAROT_HIGH_PRIESTESS, "The High Priestess", "Spawns up to 2 Planet cards", 0, 0},
    {TAROT_EMPRESS, "The Empress", "Enhances up to 2 cards to Mult (+4)", 1, 2},
    {TAROT_EMPEROR, "The Emperor", "Spawns up to 2 Tarot cards", 0, 0},
    {TAROT_HIEROPHANT, "The Hierophant", "Enhances up to 2 cards to Bonus (+30)", 1, 2},
    {TAROT_LOVERS, "The Lovers", "Enhances 1 card to Wild Card", 1, 1},
    {TAROT_CHARIOT, "The Chariot", "Enhances 1 card to Steel (x1.5)", 1, 1},
    {TAROT_JUSTICE, "Justice", "Enhances 1 card to Glass (x2)", 1, 1},
    {TAROT_HERMIT, "The Hermit", "Doubles money (max +$20)", 0, 0},
    {TAROT_WHEEL_OF_FORTUNE, "Wheel of Fortune", "1 in 4 chance for Foil/Holo/Poly", 0, 0},
    {TAROT_STRENGTH, "Strength", "Increases rank of up to 2 cards by 1", 1, 2},
    {TAROT_HANGED_MAN, "The Hanged Man", "Destroys up to 2 selected cards", 1, 2},
    {TAROT_DEATH, "Death", "Converts left card into right card", 2, 2},
    {TAROT_TEMPERANCE, "Temperance", "Grants sell value of all Jokers", 0, 0},
    {TAROT_DEVIL, "The Devil", "Enhances 1 card to Gold (+$3)", 1, 1},
    {TAROT_TOWER, "The Tower", "Enhances 1 card to Stone (+50 Chips)", 1, 1},
    {TAROT_STAR, "The Star", "Converts up to 3 cards to Diamonds", 1, 3},
    {TAROT_MOON, "The Moon", "Converts up to 3 cards to Clubs", 1, 3},
    {TAROT_SUN, "The Sun", "Converts up to 3 cards to Hearts", 1, 3},
    {TAROT_JUDGEMENT, "Judgement", "Spawns a random Joker", 0, 0},
    {TAROT_WORLD, "The World", "Converts up to 3 cards to Spades", 1, 3}
};

static const struct {
    PlanetId id;
    const char* name;
    HandType hand;
    const char* desc;
} s_planet_info[PLANET_COUNT] = {
    {PLANET_MERCURY, "Mercury", HAND_PAIR, "Pair: +15 Chips, +1 Mult"},
    {PLANET_VENUS, "Venus", HAND_THREE_OF_A_KIND, "Three of a Kind: +30 Chips, +2 Mult"},
    {PLANET_EARTH, "Earth", HAND_FULL_HOUSE, "Full House: +25 Chips, +2 Mult"},
    {PLANET_MARS, "Mars", HAND_FOUR_OF_A_KIND, "Four of a Kind: +30 Chips, +3 Mult"},
    {PLANET_JUPITER, "Jupiter", HAND_FLUSH, "Flush: +15 Chips, +2 Mult"},
    {PLANET_SATURN, "Saturn", HAND_STRAIGHT, "Straight: +30 Chips, +3 Mult"},
    {PLANET_URANUS, "Uranus", HAND_TWO_PAIR, "Two Pair: +20 Chips, +1 Mult"},
    {PLANET_NEPTUNE, "Neptune", HAND_STRAIGHT_FLUSH, "Straight Flush: +40 Chips, +4 Mult"},
    {PLANET_PLUTO, "Pluto", HAND_HIGH_CARD, "High Card: +10 Chips, +1 Mult"},
    {PLANET_PLANET_X, "Planet X", HAND_FIVE_OF_A_KIND, "Five of a Kind: +35 Chips, +3 Mult"},
    {PLANET_CERES, "Ceres", HAND_FLUSH_HOUSE, "Flush House: +40 Chips, +4 Mult"},
    {PLANET_ERIS, "Eris", HAND_FLUSH_FIVE, "Flush Five: +50 Chips, +3 Mult"},
    {PLANET_BLACK_HOLE, "Black Hole", HAND_NONE, "All poker hands +1 level"}
};

void consumable_create_tarot(Consumable* c, TarotId id) {
    if (!c || id >= TAROT_COUNT) return;
    c->type = CONSUMABLE_TAROT;
    c->id = id;
    c->name = s_tarot_info[id].name;
    c->description = s_tarot_info[id].desc;
    c->cost = 3;
    c->sell_value = 1;
    c->min_selected = s_tarot_info[id].min_sel;
    c->max_selected = s_tarot_info[id].max_sel;
    c->gfx_index = (int)id; // 0 to 21
}

void consumable_create_planet(Consumable* c, PlanetId id) {
    if (!c || id >= PLANET_COUNT) return;
    c->type = CONSUMABLE_PLANET;
    c->id = id;
    c->name = s_planet_info[id].name;
    c->description = s_planet_info[id].desc;
    c->cost = 3;
    c->sell_value = 1;
    c->min_selected = 0;
    c->max_selected = 0;
    c->gfx_index = 22 + (int)id; // 22 to 34
}

void consumable_create_random(Consumable* c, uint32_t seed) {
    if (!c) return;
    if (seed % 2 == 0) {
        TarotId tid = (TarotId)(seed % TAROT_COUNT);
        consumable_create_tarot(c, tid);
    } else {
        PlanetId pid = (PlanetId)(seed % 9); // Mercury to Pluto
        consumable_create_planet(c, pid);
    }
}

bool consumable_can_use(const Consumable* c, int selected_count) {
    if (!c || c->type == CONSUMABLE_NONE) return false;
    if (c->min_selected == 0 && c->max_selected == 0) return true;
    return (selected_count >= c->min_selected && selected_count <= c->max_selected);
}

bool consumable_use(GameSession* game, int slot_index) {
    if (!game || slot_index < 0 || slot_index >= MAX_CONSUMABLES_SLOTS) return false;
    Consumable* c = &game->consumables[slot_index];
    if (c->type == CONSUMABLE_NONE) return false;

    // Check selected cards count
    int sel_indices[8];
    int sel_count = 0;
    for (int i = 0; i < game->deck.hand.count; ++i) {
        if (game->deck.hand.cards[i].is_selected) {
            sel_indices[sel_count++] = i;
        }
    }

    if (!consumable_can_use(c, sel_count)) return false;

    // Execute Planet effect
    if (c->type == CONSUMABLE_PLANET) {
        if (c->id == PLANET_BLACK_HOLE) {
            for (int i = 1; i <= 12; ++i) {
                game->hand_levels[i]++;
            }
        } else if (c->id < PLANET_COUNT) {
            HandType ht = s_planet_info[c->id].hand;
            if (ht >= 1 && ht <= 12) {
                game->hand_levels[ht]++;
            }
        }
    }
    // Execute Tarot effect
    else if (c->type == CONSUMABLE_TAROT) {
        switch ((TarotId)c->id) {
            case TAROT_FOOL: {
                if (game->last_used_consumable.type != CONSUMABLE_NONE &&
                    game->last_used_consumable.id != TAROT_FOOL) {
                    *c = game->last_used_consumable;
                    return true;
                }
                return false;
            }
            case TAROT_MAGICIAN: {
                for (int i = 0; i < sel_count; ++i) {
                    game->deck.hand.cards[sel_indices[i]].enhancement = ENHANCEMENT_LUCKY;
                }
                break;
            }
            case TAROT_HIGH_PRIESTESS: {
                // Spawns up to 2 Planet cards
                for (int s = 0; s < MAX_CONSUMABLES_SLOTS; ++s) {
                    if (game->consumables[s].type == CONSUMABLE_NONE && s != slot_index) {
                        PlanetId pid = (PlanetId)(rand() % 9);
                        consumable_create_planet(&game->consumables[s], pid);
                        game->consumable_count++;
                    }
                }
                break;
            }
            case TAROT_EMPRESS: {
                for (int i = 0; i < sel_count; ++i) {
                    game->deck.hand.cards[sel_indices[i]].enhancement = ENHANCEMENT_MULT;
                }
                break;
            }
            case TAROT_EMPEROR: {
                // Spawns up to 2 Tarot cards
                for (int s = 0; s < MAX_CONSUMABLES_SLOTS; ++s) {
                    if (game->consumables[s].type == CONSUMABLE_NONE && s != slot_index) {
                        TarotId tid = (TarotId)(rand() % TAROT_COUNT);
                        consumable_create_tarot(&game->consumables[s], tid);
                        game->consumable_count++;
                    }
                }
                break;
            }
            case TAROT_HIEROPHANT: {
                for (int i = 0; i < sel_count; ++i) {
                    game->deck.hand.cards[sel_indices[i]].enhancement = ENHANCEMENT_BONUS;
                }
                break;
            }
            case TAROT_LOVERS: {
                if (sel_count > 0) {
                    game->deck.hand.cards[sel_indices[0]].enhancement = ENHANCEMENT_WILD;
                }
                break;
            }
            case TAROT_CHARIOT: {
                if (sel_count > 0) {
                    game->deck.hand.cards[sel_indices[0]].enhancement = ENHANCEMENT_STEEL;
                }
                break;
            }
            case TAROT_JUSTICE: {
                if (sel_count > 0) {
                    game->deck.hand.cards[sel_indices[0]].enhancement = ENHANCEMENT_GLASS;
                }
                break;
            }
            case TAROT_HERMIT: {
                int gain = game->dollars;
                if (gain > 20) gain = 20;
                game->dollars += gain;
                break;
            }
            case TAROT_WHEEL_OF_FORTUNE: {
                if (game->joker_count > 0 && (rand() % 4 == 0)) {
                    int j_idx = rand() % game->joker_count;
                    if (game->jokers[j_idx]) {
                        int ed = 1 + (rand() % 3); // FOIL, HOLO, POLY
                        game->jokers[j_idx]->edition = (CardEdition)ed;
                    }
                }
                break;
            }
            case TAROT_STRENGTH: {
                for (int i = 0; i < sel_count; ++i) {
                    Card* card = &game->deck.hand.cards[sel_indices[i]];
                    if (card->rank == RANK_ACE) card->rank = RANK_2;
                    else card->rank = (CardRank)(card->rank + 1);
                }
                break;
            }
            case TAROT_HANGED_MAN: {
                deck_remove_selected_from_hand(&game->deck);
                break;
            }
            case TAROT_DEATH: {
                if (sel_count == 2) {
                    // Left becomes exact copy of Right
                    Card target = game->deck.hand.cards[sel_indices[1]];
                    target.is_selected = true;
                    game->deck.hand.cards[sel_indices[0]] = target;
                }
                break;
            }
            case TAROT_TEMPERANCE: {
                int total_sell = 0;
                for (int j = 0; j < game->joker_count; ++j) {
                    if (game->jokers[j]) total_sell += game->jokers[j]->sell_value;
                }
                if (total_sell > 50) total_sell = 50;
                game->dollars += total_sell;
                break;
            }
            case TAROT_DEVIL: {
                if (sel_count > 0) {
                    game->deck.hand.cards[sel_indices[0]].enhancement = ENHANCEMENT_GOLD;
                }
                break;
            }
            case TAROT_TOWER: {
                if (sel_count > 0) {
                    game->deck.hand.cards[sel_indices[0]].enhancement = ENHANCEMENT_STONE;
                }
                break;
            }
            case TAROT_STAR: {
                for (int i = 0; i < sel_count; ++i) {
                    game->deck.hand.cards[sel_indices[i]].suit = SUIT_DIAMONDS;
                }
                break;
            }
            case TAROT_MOON: {
                for (int i = 0; i < sel_count; ++i) {
                    game->deck.hand.cards[sel_indices[i]].suit = SUIT_CLUBS;
                }
                break;
            }
            case TAROT_SUN: {
                for (int i = 0; i < sel_count; ++i) {
                    game->deck.hand.cards[sel_indices[i]].suit = SUIT_HEARTS;
                }
                break;
            }
            case TAROT_JUDGEMENT: {
                if (game->joker_count < MAX_JOKERS_SLOTS) {
                    JokerId jids[] = {JOKER_JOKER, JOKER_GREEDY, JOKER_LUSTY, JOKER_WRATHFUL, JOKER_GLUTTONOUS,
                                      JOKER_HALF, JOKER_BANNER, JOKER_BLUE_JOKER, JOKER_CAVENDISH};
                    JokerId pick = jids[rand() % 9];
                    joker_create(&game->jokers_storage[game->joker_count], pick, EDITION_NONE);
                    game->jokers[game->joker_count] = &game->jokers_storage[game->joker_count];
                    game->joker_count++;
                }
                break;
            }
            case TAROT_WORLD: {
                for (int i = 0; i < sel_count; ++i) {
                    game->deck.hand.cards[sel_indices[i]].suit = SUIT_SPADES;
                }
                break;
            }
            default:
                break;
        }
    }

    // Save as last used for The Fool
    if (c->id != TAROT_FOOL) {
        game->last_used_consumable = *c;
    }

    // Clear consumed slot
    memset(c, 0, sizeof(Consumable));
    if (game->consumable_count > 0) game->consumable_count--;
    game->selected_consumable_slot = -1;

    // Update live evaluation
    game_update_selection_preview(game);
    return true;
}
