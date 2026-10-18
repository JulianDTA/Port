#ifndef SHOP_H
#define SHOP_H

#include <stdint.h>
#include <stdbool.h>
#include "core/jokers.h"
#include "core/consumable.h"

typedef struct GameSession GameSession;

typedef enum {
    SHOP_ITEM_NONE = 0,
    SHOP_ITEM_JOKER,
    SHOP_ITEM_TAROT,
    SHOP_ITEM_PLANET,
    SHOP_ITEM_PACK,
    SHOP_ITEM_VOUCHER
} ShopItemType;

typedef enum {
    PACK_NONE = 0,
    PACK_ARCANA,    // 3 Tarots, pick 1
    PACK_CELESTIAL, // 3 Planets, pick 1
    PACK_BUFFOON,   // 2 Jokers, pick 1
    PACK_STANDARD   // 3 Enhanced Cards, pick 1
} PackType;

typedef enum {
    VOUCHER_NONE = 0,
    VOUCHER_OVERSTOCK,      // +1 card slot in shop
    VOUCHER_CLEARANCE_SALE, // All items 25% off
    VOUCHER_GRABBER,        // Permanently +1 hand per round
    VOUCHER_WASTEful,       // Permanently +1 discard per round
    VOUCHER_SEED_MONEY,     // Max interest raised to $10
    VOUCHER_COUNT
} VoucherId;

typedef struct {
    ShopItemType type;
    int cost;
    bool is_sold;
    union {
        Joker joker;
        Consumable consumable;
        PackType pack_type;
        VoucherId voucher_id;
    } data;
} ShopSlot;

typedef struct {
    ShopSlot slots[2];      // 2 Card slots (Jokers/Consumables)
    ShopSlot pack_slot;     // 1 Booster Pack
    ShopSlot voucher_slot;  // 1 Voucher
    int reroll_cost;        // Starts at $5, increments by $1 per reroll
    bool pack_opened;
    PackType active_pack;
    int pack_choice_count;
    ShopSlot pack_cards[3]; // The 3 cards revealed inside the pack
} ShopState;

void shop_generate(ShopState* shop, uint32_t seed, int ante);
bool shop_buy_card_slot(GameSession* game, int slot_index);
bool shop_buy_pack(GameSession* game);
bool shop_choose_pack_card(GameSession* game, int card_index);
bool shop_buy_voucher(GameSession* game);
bool shop_reroll(GameSession* game);

#endif // SHOP_H
