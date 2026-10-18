#include "core/shop.h"
#include "core/game_state.h"
#include <string.h>
#include <stdlib.h>

void shop_generate(ShopState* shop, uint32_t seed, int ante) {
    if (!shop) return;
    memset(shop, 0, sizeof(ShopState));
    shop->reroll_cost = 5;

    // Slot 0: Joker
    shop->slots[0].type = SHOP_ITEM_JOKER;
    shop->slots[0].is_sold = false;
    JokerId jids[] = {
        JOKER_JOKER, JOKER_GREEDY, JOKER_LUSTY, JOKER_WRATHFUL, JOKER_GLUTTONOUS,
        JOKER_JOLLY, JOKER_HALF, JOKER_BANNER, JOKER_MYSTIC_SUMMIT, JOKER_BARON,
        JOKER_SCARY_FACE, JOKER_ABSTRACT, JOKER_GROS_MICHEL, JOKER_EVEN_STEVEN,
        JOKER_ODD_TODD, JOKER_SCHOLAR, JOKER_BLUE_JOKER, JOKER_CAVENDISH, JOKER_FIBONACCI
    };
    int j_count = sizeof(jids) / sizeof(jids[0]);
    JokerId pick_j = jids[seed % j_count];
    joker_create(&shop->slots[0].data.joker, pick_j, EDITION_NONE);
    shop->slots[0].cost = shop->slots[0].data.joker.cost;

    // Slot 1: Consumable (Tarot or Planet)
    if ((seed / 3) % 2 == 0) {
        shop->slots[1].type = SHOP_ITEM_TAROT;
        shop->slots[1].is_sold = false;
        TarotId tid = (TarotId)((seed / 5) % TAROT_COUNT);
        consumable_create_tarot(&shop->slots[1].data.consumable, tid);
        shop->slots[1].cost = 3;
    } else {
        shop->slots[1].type = SHOP_ITEM_PLANET;
        shop->slots[1].is_sold = false;
        PlanetId pid = (PlanetId)((seed / 7) % 9);
        consumable_create_planet(&shop->slots[1].data.consumable, pid);
        shop->slots[1].cost = 3;
    }

    // Pack Slot: Booster Pack ($4)
    shop->pack_slot.type = SHOP_ITEM_PACK;
    shop->pack_slot.is_sold = false;
    shop->pack_slot.cost = 4;
    PackType ptypes[] = {PACK_ARCANA, PACK_CELESTIAL, PACK_BUFFOON, PACK_STANDARD};
    shop->pack_slot.data.pack_type = ptypes[(seed / 11) % 4];

    // Voucher Slot ($10)
    shop->voucher_slot.type = SHOP_ITEM_VOUCHER;
    shop->voucher_slot.is_sold = false;
    shop->voucher_slot.cost = 10;
    shop->voucher_slot.data.voucher_id = (VoucherId)(1 + ((seed + ante) % (VOUCHER_COUNT - 1)));
}

bool shop_buy_card_slot(GameSession* game, int slot_index) {
    if (!game || slot_index < 0 || slot_index > 1) return false;
    ShopSlot* slot = &game->shop.slots[slot_index];
    if (slot->is_sold || game->dollars < slot->cost) return false;

    if (slot->type == SHOP_ITEM_JOKER) {
        if (game->joker_count >= MAX_JOKERS_SLOTS) return false; // Full
        game->dollars -= slot->cost;
        slot->is_sold = true;

        int idx = game->joker_count;
        game->jokers_storage[idx] = slot->data.joker;
        game->jokers[idx] = &game->jokers_storage[idx];
        game->joker_count++;
        return true;
    } else if (slot->type == SHOP_ITEM_TAROT || slot->type == SHOP_ITEM_PLANET) {
        if (game->consumable_count >= MAX_CONSUMABLES_SLOTS) return false; // Full
        game->dollars -= slot->cost;
        slot->is_sold = true;

        for (int s = 0; s < MAX_CONSUMABLES_SLOTS; ++s) {
            if (game->consumables[s].type == CONSUMABLE_NONE) {
                game->consumables[s] = slot->data.consumable;
                game->consumable_count++;
                break;
            }
        }
        return true;
    }
    return false;
}

bool shop_buy_pack(GameSession* game) {
    if (!game || game->shop.pack_slot.is_sold || game->dollars < game->shop.pack_slot.cost) return false;
    game->dollars -= game->shop.pack_slot.cost;
    game->shop.pack_slot.is_sold = true;
    game->shop.pack_opened = true;
    game->shop.active_pack = game->shop.pack_slot.data.pack_type;
    game->current_state = STATE_BOOSTER_OPEN;

    // Generate 3 choices based on pack type
    game->shop.pack_choice_count = 3;
    uint32_t s = game->rng_state;
    for (int i = 0; i < 3; ++i) {
        s ^= s << 13; s ^= s >> 17; s ^= s << 5;
        ShopSlot* choice = &game->shop.pack_cards[i];
        choice->is_sold = false;
        choice->cost = 0;

        if (game->shop.active_pack == PACK_ARCANA) {
            choice->type = SHOP_ITEM_TAROT;
            TarotId tid = (TarotId)(s % TAROT_COUNT);
            consumable_create_tarot(&choice->data.consumable, tid);
        } else if (game->shop.active_pack == PACK_CELESTIAL) {
            choice->type = SHOP_ITEM_PLANET;
            PlanetId pid = (PlanetId)(s % 12);
            consumable_create_planet(&choice->data.consumable, pid);
        } else if (game->shop.active_pack == PACK_BUFFOON) {
            choice->type = SHOP_ITEM_JOKER;
            JokerId jids[] = {JOKER_JOKER, JOKER_HALF, JOKER_BANNER, JOKER_BLUE_JOKER, JOKER_CAVENDISH, JOKER_SCHOLAR};
            joker_create(&choice->data.joker, jids[s % 6], EDITION_NONE);
        } else {
            // Standard enhanced card
            choice->type = SHOP_ITEM_TAROT;
            consumable_create_tarot(&choice->data.consumable, TAROT_FOOL);
        }
    }
    game->rng_state = s;
    return true;
}

bool shop_choose_pack_card(GameSession* game, int card_index) {
    if (!game || card_index < 0 || card_index >= 3) return false;
    ShopSlot* choice = &game->shop.pack_cards[card_index];
    if (choice->is_sold) return false;

    if (choice->type == SHOP_ITEM_JOKER) {
        if (game->joker_count >= MAX_JOKERS_SLOTS) return false;
        int idx = game->joker_count;
        game->jokers_storage[idx] = choice->data.joker;
        game->jokers[idx] = &game->jokers_storage[idx];
        game->joker_count++;
    } else if (choice->type == SHOP_ITEM_TAROT || choice->type == SHOP_ITEM_PLANET) {
        if (game->consumable_count >= MAX_CONSUMABLES_SLOTS) return false;
        for (int s = 0; s < MAX_CONSUMABLES_SLOTS; ++s) {
            if (game->consumables[s].type == CONSUMABLE_NONE) {
                game->consumables[s] = choice->data.consumable;
                game->consumable_count++;
                break;
            }
        }
    }

    choice->is_sold = true;
    game->shop.pack_opened = false;
    game->current_state = STATE_SHOP; // Return to shop
    return true;
}

bool shop_buy_voucher(GameSession* game) {
    if (!game || game->shop.voucher_slot.is_sold || game->dollars < game->shop.voucher_slot.cost) return false;
    game->dollars -= game->shop.voucher_slot.cost;
    game->shop.voucher_slot.is_sold = true;

    VoucherId vid = game->shop.voucher_slot.data.voucher_id;
    if (vid == VOUCHER_GRABBER) {
        game->max_hands++;
        game->hands_left++;
    } else if (vid == VOUCHER_WASTEful) {
        game->max_discards++;
        game->discards_left++;
    }
    return true;
}

bool shop_reroll(GameSession* game) {
    if (!game || game->dollars < game->shop.reroll_cost) return false;
    game->dollars -= game->shop.reroll_cost;
    game->shop.reroll_cost += 1;

    game->rng_state ^= game->rng_state << 13;
    game->rng_state ^= game->rng_state >> 17;
    game->rng_state ^= game->rng_state << 5;

    shop_generate(&game->shop, game->rng_state, game->ante);
    return true;
}
