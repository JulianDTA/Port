#ifndef CONSUMABLE_H
#define CONSUMABLE_H

#include <stdint.h>
#include <stdbool.h>
#include "core/card.h"
#include "core/poker_hand.h"

// Forward declaration of GameSession
typedef struct GameSession GameSession;

typedef enum {
    CONSUMABLE_NONE = 0,
    CONSUMABLE_TAROT,
    CONSUMABLE_PLANET,
    CONSUMABLE_SPECTRAL
} ConsumableType;

typedef enum {
    TAROT_FOOL = 0,
    TAROT_MAGICIAN,
    TAROT_HIGH_PRIESTESS,
    TAROT_EMPRESS,
    TAROT_EMPEROR,
    TAROT_HIEROPHANT,
    TAROT_LOVERS,
    TAROT_CHARIOT,
    TAROT_JUSTICE,
    TAROT_HERMIT,
    TAROT_WHEEL_OF_FORTUNE,
    TAROT_STRENGTH,
    TAROT_HANGED_MAN,
    TAROT_DEATH,
    TAROT_TEMPERANCE,
    TAROT_DEVIL,
    TAROT_TOWER,
    TAROT_STAR,
    TAROT_MOON,
    TAROT_SUN,
    TAROT_JUDGEMENT,
    TAROT_WORLD,
    TAROT_COUNT
} TarotId;

typedef enum {
    PLANET_MERCURY = 0, // Pair
    PLANET_VENUS,       // Three of a Kind
    PLANET_EARTH,       // Full House
    PLANET_MARS,        // Four of a Kind
    PLANET_JUPITER,     // Flush
    PLANET_SATURN,      // Straight
    PLANET_URANUS,      // Two Pair
    PLANET_NEPTUNE,     // Straight Flush
    PLANET_PLUTO,       // High Card
    PLANET_PLANET_X,    // Five of a Kind
    PLANET_CERES,       // Flush House
    PLANET_ERIS,        // Flush Five
    PLANET_BLACK_HOLE,  // All Hands
    PLANET_COUNT
} PlanetId;

typedef struct {
    ConsumableType type;
    int id;
    const char* name;
    const char* description;
    int cost;
    int sell_value;
    int min_selected;
    int max_selected;
    int gfx_index; // Index in s_balatro_consumables array
} Consumable;

#define MAX_CONSUMABLES_SLOTS 2

void consumable_create_tarot(Consumable* c, TarotId id);
void consumable_create_planet(Consumable* c, PlanetId id);
void consumable_create_random(Consumable* c, uint32_t seed);
bool consumable_can_use(const Consumable* c, int selected_count);
bool consumable_use(GameSession* game, int slot_index);

#endif // CONSUMABLE_H
