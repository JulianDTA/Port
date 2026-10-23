#include "core/game_state.h"
#include <string.h>
#include <stdlib.h>

static const int64_t s_ante_base_scores[9] = {
    0,
    300,    // Ante 1
    800,    // Ante 2
    2000,   // Ante 3
    5000,   // Ante 4
    11000,  // Ante 5
    20000,  // Ante 6
    35000,  // Ante 7
    50000   // Ante 8
};

int64_t game_calculate_blind_target(int ante, BlindType blind, BossBlindId boss) {
    int64_t base = 0;
    if (ante <= 8) {
        base = s_ante_base_scores[ante];
    } else {
        base = s_ante_base_scores[8];
        for (int i = 9; i <= ante; ++i) {
            base = (int64_t)(base * 2.5);
        }
    }

    switch (blind) {
        case BLIND_SMALL: return base;
        case BLIND_BIG:   return (int64_t)(base * 1.5);
        case BLIND_BOSS: {
            BossBlindInfo binfo = boss_blind_get_info(boss);
            return (int64_t)(base * binfo.score_mult);
        }
        default: return base;
    }
}

void game_session_init(GameSession* game) {
    if (!game) return;
    memset(game, 0, sizeof(GameSession));
    game->current_state = STATE_TITLE;
    game->max_hands = 4;
    game->max_discards = 3;
    game->dollars = 4;
    game->rng_state = 0x87654321;
    game->selected_consumable_slot = -1;
    game->inspecting_jokers = false;
    game->inspected_joker_idx = 0;

    for (int i = 0; i < 13; ++i) {
        game->hand_levels[i] = 1;
        game->hand_play_counts[i] = 0;
    }
}

void game_session_start_run(GameSession* game) {
    if (!game) return;
    game->ante = 1;
    game->dollars = 4;
    game->current_round_score = 0;
    game->joker_count = 0;
    game->consumable_count = 0;
    game->selected_consumable_slot = -1;

    memset(game->jokers, 0, sizeof(game->jokers));
    memset(game->consumables, 0, sizeof(game->consumables));
    memset(&game->last_used_consumable, 0, sizeof(Consumable));

    for (int i = 0; i < 13; ++i) {
        game->hand_levels[i] = 1;
        game->hand_play_counts[i] = 0;
    }

    deck_init_standard(&game->deck);
    deck_shuffle(&game->deck.draw_pile, game->rng_state);

    // Pick Boss Blind for Ante 1
    game->current_boss = boss_blind_pick_for_ante(1, game->rng_state);

    game_session_select_blind(game, BLIND_SMALL);
}

void game_session_select_blind(GameSession* game, BlindType blind) {
    if (!game) return;
    game->current_blind = blind;
    game->target_score = game_calculate_blind_target(game->ante, blind, game->current_boss);
    game->current_round_score = 0;
    game->hands_left = game->max_hands;
    game->discards_left = game->max_discards;

    // Boss effect: The Water starts with 0 discards
    if (blind == BLIND_BOSS && game->current_boss == BOSS_THE_WATER) {
        game->discards_left = 0;
    }

    // Reset draw & discard piles
    for (int i = 0; i < game->deck.discard_pile.count; ++i) {
        game->deck.draw_pile.cards[game->deck.draw_pile.count++] = game->deck.discard_pile.cards[i];
    }
    game->deck.discard_pile.count = 0;
    for (int i = 0; i < game->deck.hand.count; ++i) {
        game->deck.draw_pile.cards[game->deck.draw_pile.count++] = game->deck.hand.cards[i];
    }
    game->deck.hand.count = 0;

    game->rng_state ^= game->rng_state << 13;
    game->rng_state ^= game->rng_state >> 17;
    game->rng_state ^= game->rng_state << 5;
    deck_shuffle(&game->deck.draw_pile, game->rng_state);
    deck_draw_up_to_limit(&game->deck);
    if (game->active_sort_mode == SORT_MODE_RANK) {
        deck_sort_by_rank(&game->deck);
    } else if (game->active_sort_mode == SORT_MODE_SUIT) {
        deck_sort_by_suit(&game->deck);
    }

    game->current_state = STATE_ROUND_ACTIVE;
}

bool game_play_selected_hand(GameSession* game) {
    if (!game || game->hands_left <= 0) return false;

    const Card* selected[5];
    int count = deck_get_selected_cards(&game->deck, selected);
    if (count == 0) return false;

    // Boss effect: The Psychic requires exactly 5 cards
    if (game->current_blind == BLIND_BOSS && game->current_boss == BOSS_THE_PSYCHIC && count < 5) {
        return false;
    }

    // 1. Evaluate Poker Hand
    game->last_eval = poker_evaluate_played(selected, count, game->hand_levels);

    // 2. Initialize Score State to base chips and mult
    score_state_init(&game->last_score, game->last_eval.initial_chips, (double)game->last_eval.initial_mult);

    // Enter scoring state
    game->current_state = STATE_ROUND_SCORING;
    game->scoring_step = 0;
    game->scoring_timer = 0;
    return true;
}

void game_tick_scoring(GameSession* game) {
    if (!game || game->current_state != STATE_ROUND_SCORING) return;

    game->scoring_timer++;

    // Step 0: Hand announcement delay (10 frames)
    if (game->scoring_step == 0) {
        if (game->scoring_timer >= 10) {
            game->scoring_step = 1;
            game->scoring_timer = 0;
        }
        return;
    }

    // Steps 1..scoring_count: Score each played card in sequence
    if (game->scoring_step >= 1 && game->scoring_step <= game->last_eval.scoring_count) {
        int c_idx = game->scoring_step - 1;
        if (game->scoring_timer == 1) {
            const Card* c = game->last_eval.scoring_cards[c_idx];
            bool debuffed = (game->current_blind == BLIND_BOSS) && boss_blind_is_card_debuffed(game->current_boss, c);

            if (!debuffed) {
                score_state_add_chips(&game->last_score, card_get_base_chips(c));
                if (c->enhancement == ENHANCEMENT_BONUS) score_state_add_chips(&game->last_score, 30);
                else if (c->enhancement == ENHANCEMENT_MULT) score_state_add_mult(&game->last_score, 4.0);
                else if (c->enhancement == ENHANCEMENT_GLASS) score_state_multiply_mult(&game->last_score, 2.0);

                if (c->edition == EDITION_FOIL) score_state_add_chips(&game->last_score, 50);
                else if (c->edition == EDITION_HOLOGRAPHIC) score_state_add_mult(&game->last_score, 10.0);
                else if (c->edition == EDITION_POLYCHROME) score_state_multiply_mult(&game->last_score, 1.5);

                if (c->seal == SEAL_GOLD) game->dollars += 3;

                joker_trigger_card_scored(game->jokers, game->joker_count, c, &game->last_score);
            }
        }
        if (game->scoring_timer >= 8) {
            game->scoring_step++;
            game->scoring_timer = 0;
        }
        return;
    }

    // Step scoring_count + 1: Held in hand triggers (Steel cards, Jokers)
    int step_held = game->last_eval.scoring_count + 1;
    if (game->scoring_step == step_held) {
        if (game->scoring_timer == 1) {
            for (int i = 0; i < game->deck.hand.count; ++i) {
                if (!game->deck.hand.cards[i].is_selected) {
                    const Card* c = &game->deck.hand.cards[i];
                    if (c->enhancement == ENHANCEMENT_STEEL) {
                        score_state_multiply_mult(&game->last_score, 1.5);
                    }
                    joker_trigger_held_in_hand(game->jokers, game->joker_count, c, &game->last_score);
                }
            }
        }
        if (game->scoring_timer >= 8) {
            game->scoring_step++;
            game->scoring_timer = 0;
        }
        return;
    }

    // Step scoring_count + 2: Jokers independent evaluation
    int step_jokers = game->last_eval.scoring_count + 2;
    if (game->scoring_step == step_jokers) {
        if (game->scoring_timer == 1) {
            RoundContext ctx;
            memset(&ctx, 0, sizeof(ctx));
            ctx.remaining_discards = game->discards_left;
            ctx.remaining_hands = game->hands_left;
            ctx.current_dollars = game->dollars;
            ctx.cards_remaining_in_deck = game->deck.draw_pile.count;
            ctx.jokers_count = game->joker_count;
            ctx.max_joker_slots = MAX_JOKERS_SLOTS;

            const Card* selected[5];
            int count = deck_get_selected_cards(&game->deck, selected);
            joker_trigger_hand_evaluated(game->jokers, game->joker_count, game->last_eval.type, count, &ctx, &game->last_score);
        }
        if (game->scoring_timer >= 10) {
            game->scoring_step++;
            game->scoring_timer = 0;
        }
        return;
    }

    // Step scoring_count + 3: Slam Chips x Mult into Round Score
    int step_slam = game->last_eval.scoring_count + 3;
    if (game->scoring_step == step_slam) {
        if (game->scoring_timer == 1) {
            int64_t hand_points = score_state_calculate_final(&game->last_score);
            game->current_round_score += hand_points;
        }
        if (game->scoring_timer >= 14) {
            game->scoring_step++;
            game->scoring_timer = 0;
        }
        return;
    }

    // Step scoring_count + 4: Conclude Hand & transition state
    int step_finish = game->last_eval.scoring_count + 4;
    if (game->scoring_step >= step_finish) {
        game_finish_scoring(game);
    }
}

void game_finish_scoring(GameSession* game) {
    if (!game || game->current_state != STATE_ROUND_SCORING) return;

    // Fast-forward any remaining card scoring
    int start_c = (game->scoring_step >= 1) ? (game->scoring_step - 1) : 0;
    if (game->scoring_timer == 0) start_c = (game->scoring_step >= 1) ? game->scoring_step : 0;

    for (int i = start_c; i < game->last_eval.scoring_count; ++i) {
        const Card* c = game->last_eval.scoring_cards[i];
        bool debuffed = (game->current_blind == BLIND_BOSS) && boss_blind_is_card_debuffed(game->current_boss, c);

        if (!debuffed) {
            score_state_add_chips(&game->last_score, card_get_base_chips(c));
            if (c->enhancement == ENHANCEMENT_BONUS) score_state_add_chips(&game->last_score, 30);
            else if (c->enhancement == ENHANCEMENT_MULT) score_state_add_mult(&game->last_score, 4.0);
            else if (c->enhancement == ENHANCEMENT_GLASS) score_state_multiply_mult(&game->last_score, 2.0);

            if (c->edition == EDITION_FOIL) score_state_add_chips(&game->last_score, 50);
            else if (c->edition == EDITION_HOLOGRAPHIC) score_state_add_mult(&game->last_score, 10.0);
            else if (c->edition == EDITION_POLYCHROME) score_state_multiply_mult(&game->last_score, 1.5);

            if (c->seal == SEAL_GOLD) game->dollars += 3;

            joker_trigger_card_scored(game->jokers, game->joker_count, c, &game->last_score);
        }
    }

    int step_held = game->last_eval.scoring_count + 1;
    if (game->scoring_step < step_held || (game->scoring_step == step_held && game->scoring_timer == 0)) {
        for (int i = 0; i < game->deck.hand.count; ++i) {
            if (!game->deck.hand.cards[i].is_selected) {
                const Card* c = &game->deck.hand.cards[i];
                if (c->enhancement == ENHANCEMENT_STEEL) {
                    score_state_multiply_mult(&game->last_score, 1.5);
                }
                joker_trigger_held_in_hand(game->jokers, game->joker_count, c, &game->last_score);
            }
        }
    }

    int step_jokers = game->last_eval.scoring_count + 2;
    if (game->scoring_step < step_jokers || (game->scoring_step == step_jokers && game->scoring_timer == 0)) {
        RoundContext ctx;
        memset(&ctx, 0, sizeof(ctx));
        ctx.remaining_discards = game->discards_left;
        ctx.remaining_hands = game->hands_left;
        ctx.current_dollars = game->dollars;
        ctx.cards_remaining_in_deck = game->deck.draw_pile.count;
        ctx.jokers_count = game->joker_count;
        ctx.max_joker_slots = MAX_JOKERS_SLOTS;

        const Card* selected[5];
        int count = deck_get_selected_cards(&game->deck, selected);
        joker_trigger_hand_evaluated(game->jokers, game->joker_count, game->last_eval.type, count, &ctx, &game->last_score);
    }

    int step_slam = game->last_eval.scoring_count + 3;
    if (game->scoring_step < step_slam || (game->scoring_step == step_slam && game->scoring_timer == 0)) {
        int64_t hand_points = score_state_calculate_final(&game->last_score);
        game->current_round_score += hand_points;
    }

    // Trigger after-hand effects
    RoundContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.remaining_discards = game->discards_left;
    ctx.remaining_hands = game->hands_left;
    ctx.current_dollars = game->dollars;
    ctx.cards_remaining_in_deck = game->deck.draw_pile.count;
    ctx.jokers_count = game->joker_count;
    ctx.max_joker_slots = MAX_JOKERS_SLOTS;
    joker_trigger_hand_played(game->jokers, game->joker_count, &ctx);

    if (game->current_blind == BLIND_BOSS && game->current_boss == BOSS_THE_ARM) {
        if (game->hand_levels[game->last_eval.type] > 1) {
            game->hand_levels[game->last_eval.type]--;
        }
    }
    // Boss effect: The Ox sets money to $0 if playing most played hand
    if (game->current_blind == BLIND_BOSS && game->current_boss == BOSS_THE_OX) {
        int most_played_type = 1;
        int max_plays = game->hand_play_counts[1];
        for (int h = 2; h <= 12; ++h) {
            if (game->hand_play_counts[h] > max_plays) {
                max_plays = game->hand_play_counts[h];
                most_played_type = h;
            }
        }
        if (game->last_eval.type == most_played_type && max_plays > 0) {
            game->dollars = 0;
        }
    }
    if (game->last_eval.type >= 1 && game->last_eval.type <= 12) {
        game->hand_play_counts[game->last_eval.type]++;
    }

    deck_remove_selected_from_hand(&game->deck);

    // Boss effect: The Hook discards 2 random cards from remaining hand
    if (game->current_blind == BLIND_BOSS && game->current_boss == BOSS_THE_HOOK) {
        for (int d = 0; d < 2; ++d) {
            if (game->deck.hand.count > 0) {
                int r_idx = (int)(rand() % game->deck.hand.count);
                if (game->deck.discard_pile.count < MAX_DECK_SIZE) {
                    game->deck.discard_pile.cards[game->deck.discard_pile.count++] = game->deck.hand.cards[r_idx];
                }
                for (int m = r_idx; m < game->deck.hand.count - 1; ++m) {
                    game->deck.hand.cards[m] = game->deck.hand.cards[m + 1];
                }
                game->deck.hand.count--;
            }
        }
    }
    game->hands_left--;

    if (game->current_round_score >= game->target_score) {
        game->current_state = STATE_ROUND_VICTORY;
        int base_reward = (game->current_blind == BLIND_SMALL) ? 3 : (game->current_blind == BLIND_BIG ? 4 : 5);
        int bonus_hands = game->hands_left;
        int interest = game->dollars / 5;
        if (interest > 5) interest = 5;

        game->round_payout = base_reward + bonus_hands + interest;
        game->dollars += game->round_payout;

        joker_trigger_round_end(game->jokers, &game->joker_count);
    } else if (game->hands_left <= 0) {
        game->current_state = STATE_GAME_OVER;
    } else {
        deck_draw_up_to_limit(&game->deck);
        if (game->active_sort_mode == SORT_MODE_RANK) {
            deck_sort_by_rank(&game->deck);
        } else if (game->active_sort_mode == SORT_MODE_SUIT) {
            deck_sort_by_suit(&game->deck);
        }
        game->current_state = STATE_ROUND_ACTIVE;
    }

    game_update_selection_preview(game);
}

bool game_discard_selected_cards(GameSession* game) {
    if (!game || game->discards_left <= 0) return false;

    const Card* selected[5];
    int count = deck_get_selected_cards(&game->deck, selected);
    if (count == 0) return false;

    deck_discard_selected(&game->deck);
    game->discards_left--;
    deck_draw_up_to_limit(&game->deck);
    if (game->active_sort_mode == SORT_MODE_RANK) {
        deck_sort_by_rank(&game->deck);
    } else if (game->active_sort_mode == SORT_MODE_SUIT) {
        deck_sort_by_suit(&game->deck);
    }

    game_update_selection_preview(game);
    return true;
}

void game_update_selection_preview(GameSession* game) {
    if (!game) return;
    const Card* selected[5];
    int count = deck_get_selected_cards(&game->deck, selected);
    if (count == 0) {
        memset(&game->preview_eval, 0, sizeof(HandEvaluation));
        score_state_init(&game->preview_score, 0, 0.0);
        return;
    }

    game->preview_eval = poker_evaluate_played(selected, count, game->hand_levels);
    score_state_init(&game->preview_score, game->preview_eval.initial_chips, (double)game->preview_eval.initial_mult);

    for (int i = 0; i < game->preview_eval.scoring_count; ++i) {
        score_state_add_chips(&game->preview_score, card_get_base_chips(game->preview_eval.scoring_cards[i]));
    }
}

void game_advance_to_shop(GameSession* game) {
    if (!game) return;
    game->current_state = STATE_SHOP;
    shop_generate(&game->shop, game->rng_state, game->ante);
}

void game_advance_next_blind(GameSession* game) {
    if (!game) return;
    if (game->current_blind == BLIND_SMALL) {
        game_session_select_blind(game, BLIND_BIG);
    } else if (game->current_blind == BLIND_BIG) {
        game_session_select_blind(game, BLIND_BOSS);
    } else {
        game->ante++;
        // Pick new Boss Blind for the next Ante
        game->current_boss = boss_blind_pick_for_ante(game->ante, game->rng_state);
        game_session_select_blind(game, BLIND_SMALL);
    }
    game_update_selection_preview(game);
}

bool game_sell_joker(GameSession* game, int joker_index) {
    if (!game || joker_index < 0 || joker_index >= game->joker_count) return false;
    Joker* j = game->jokers[joker_index];
    if (!j) return false;

    game->dollars += j->sell_value;
    for (int i = joker_index; i < game->joker_count - 1; ++i) {
        game->jokers_storage[i] = game->jokers_storage[i + 1];
        game->jokers[i] = &game->jokers_storage[i];
    }
    game->joker_count--;
    game->jokers[game->joker_count] = NULL;
    return true;
}

bool game_sell_consumable(GameSession* game, int consumable_index) {
    if (!game || consumable_index < 0 || consumable_index >= MAX_CONSUMABLES_SLOTS) return false;
    Consumable* c = &game->consumables[consumable_index];
    if (c->type == CONSUMABLE_NONE) return false;

    game->dollars += c->sell_value;
    memset(c, 0, sizeof(Consumable));
    if (game->consumable_count > 0) game->consumable_count--;
    game->selected_consumable_slot = -1;
    return true;
}

bool game_swap_jokers(GameSession* game, int idx1, int idx2) {
    if (!game) return false;
    if (idx1 < 0 || idx1 >= game->joker_count || idx2 < 0 || idx2 >= game->joker_count) {
        return false;
    }
    if (idx1 == idx2) return true;
    Joker temp = game->jokers_storage[idx1];
    game->jokers_storage[idx1] = game->jokers_storage[idx2];
    game->jokers_storage[idx2] = temp;
    for (int i = 0; i < game->joker_count; ++i) {
        game->jokers[i] = &game->jokers_storage[i];
    }
    return true;
}

