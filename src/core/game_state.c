#include "core/game_state.h"
#include <string.h>

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

int64_t game_calculate_blind_target(int ante, BlindType blind) {
    int64_t base = 0;
    if (ante <= 8) {
        base = s_ante_base_scores[ante];
    } else {
        // Endless scaling
        base = s_ante_base_scores[8];
        for (int i = 9; i <= ante; ++i) {
            base = (int64_t)(base * 2.5);
        }
    }

    switch (blind) {
        case BLIND_SMALL: return base;
        case BLIND_BIG:   return (int64_t)(base * 1.5);
        case BLIND_BOSS:  return (int64_t)(base * 2.0);
        default:          return base;
    }
}

void game_session_init(GameSession* game) {
    if (!game) return;
    memset(game, 0, sizeof(GameSession));
    game->current_state = STATE_TITLE;
    game->max_hands = 4;
    game->max_discards = 3;
    game->dollars = 4; // Starting money

    for (int i = 0; i < 13; ++i) {
        game->hand_levels[i] = 1; // All hands start at level 1
    }
}

void game_session_start_run(GameSession* game) {
    if (!game) return;
    game->ante = 1;
    game->dollars = 4;
    game->current_round_score = 0;

    deck_init_standard(&game->deck);
    deck_shuffle(&game->deck.draw_pile, 0x12345678);

    game_session_select_blind(game, BLIND_SMALL);
}

void game_session_select_blind(GameSession* game, BlindType blind) {
    if (!game) return;
    game->current_blind = blind;
    game->target_score = game_calculate_blind_target(game->ante, blind);
    game->current_round_score = 0;
    game->hands_left = game->max_hands;
    game->discards_left = game->max_discards;

    // Shuffle draw pile & reset discard pile into deck
    for (int i = 0; i < game->deck.discard_pile.count; ++i) {
        game->deck.draw_pile.cards[game->deck.draw_pile.count++] = game->deck.discard_pile.cards[i];
    }
    game->deck.discard_pile.count = 0;
    for (int i = 0; i < game->deck.hand.count; ++i) {
        game->deck.draw_pile.cards[game->deck.draw_pile.count++] = game->deck.hand.cards[i];
    }
    game->deck.hand.count = 0;

    deck_shuffle(&game->deck.draw_pile, 0);
    deck_draw_up_to_limit(&game->deck);

    game->current_state = STATE_ROUND_ACTIVE;
}

bool game_play_selected_hand(GameSession* game) {
    if (!game || game->hands_left <= 0) return false;

    const Card* selected[5];
    int count = deck_get_selected_cards(&game->deck, selected);
    if (count == 0) return false;

    // 1. Evaluate Poker Hand
    game->last_eval = poker_evaluate_played(selected, count, game->hand_levels);

    // 2. Initialize Score State
    score_state_init(&game->last_score, game->last_eval.initial_chips, (double)game->last_eval.initial_mult);

    // Context for round
    RoundContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.remaining_discards = game->discards_left;
    ctx.remaining_hands = game->hands_left;
    ctx.current_dollars = game->dollars;
    ctx.cards_remaining_in_deck = game->deck.draw_pile.count;
    ctx.jokers_count = game->joker_count;
    ctx.max_joker_slots = MAX_JOKERS_SLOTS;
    for (int i = 0; i < game->deck.hand.count; ++i) {
        if (!game->deck.hand.cards[i].is_selected) {
            ctx.cards_in_hand[ctx.cards_in_hand_count++] = &game->deck.hand.cards[i];
        }
    }

    // 3. Score played cards (rank chips + card enhancements + editions + seals)
    for (int i = 0; i < game->last_eval.scoring_count; ++i) {
        const Card* c = game->last_eval.scoring_cards[i];
        int repeat_count = (c->seal == SEAL_RED) ? 2 : 1;

        for (int r = 0; r < repeat_count; ++r) {
            score_state_add_chips(&game->last_score, card_get_base_chips(c));

            if (c->enhancement == ENHANCEMENT_BONUS) {
                score_state_add_chips(&game->last_score, 30);
            } else if (c->enhancement == ENHANCEMENT_MULT) {
                score_state_add_mult(&game->last_score, 4.0);
            } else if (c->enhancement == ENHANCEMENT_GLASS) {
                score_state_multiply_mult(&game->last_score, 2.0);
            }

            if (c->edition == EDITION_FOIL) {
                score_state_add_chips(&game->last_score, 50);
            } else if (c->edition == EDITION_HOLOGRAPHIC) {
                score_state_add_mult(&game->last_score, 10.0);
            } else if (c->edition == EDITION_POLYCHROME) {
                score_state_multiply_mult(&game->last_score, 1.5);
            }

            if (c->seal == SEAL_GOLD) {
                game->dollars += 3;
            }

            // Joker triggers on scored card
            joker_trigger_card_scored(game->jokers, game->joker_count, c, &game->last_score);
        }
    }

    // 4. Cards held in hand triggers (e.g. Steel cards, Baron)
    for (int i = 0; i < game->deck.hand.count; ++i) {
        if (!game->deck.hand.cards[i].is_selected) {
            const Card* c = &game->deck.hand.cards[i];
            if (c->enhancement == ENHANCEMENT_STEEL) {
                score_state_multiply_mult(&game->last_score, 1.5);
            }
            joker_trigger_held_in_hand(game->jokers, game->joker_count, c, &game->last_score);
        }
    }

    // 5. Jokers independent evaluation triggers
    joker_trigger_hand_evaluated(game->jokers, game->joker_count, game->last_eval.type, count, &ctx, &game->last_score);

    // 6. Calculate total score for this hand
    int64_t hand_points = score_state_calculate_final(&game->last_score);
    game->current_round_score += hand_points;

    // Trigger after-hand effects (e.g. Ice Cream -5 chips)
    joker_trigger_hand_played(game->jokers, game->joker_count, &ctx);

    // 7. Cleanup played hand & redraw
    deck_remove_selected_from_hand(&game->deck);
    game->hands_left--;

    // 8. Check victory or defeat conditions
    if (game->current_round_score >= game->target_score) {
        game->current_state = STATE_ROUND_VICTORY;
        // Payout money: $3 for Small Blind, $4 for Big, $5 for Boss + leftover hands
        int base_reward = (game->current_blind == BLIND_SMALL) ? 3 : (game->current_blind == BLIND_BIG ? 4 : 5);
        int bonus_hands = game->hands_left;
        game->round_payout = base_reward + bonus_hands;
        game->dollars += game->round_payout;

        // Trigger end of round effects (e.g. Popcorn decay, Gros Michel)
        joker_trigger_round_end(game->jokers, &game->joker_count);
    } else if (game->hands_left <= 0) {
        game->current_state = STATE_GAME_OVER;
    } else {
        // Redraw cards to refill hand
        deck_draw_up_to_limit(&game->deck);
    }

    game_update_selection_preview(game);
    return true;
}

bool game_discard_selected_cards(GameSession* game) {
    if (!game || game->discards_left <= 0) return false;

    const Card* selected[5];
    int count = deck_get_selected_cards(&game->deck, selected);
    if (count == 0) return false;

    deck_discard_selected(&game->deck);
    game->discards_left--;
    deck_draw_up_to_limit(&game->deck);

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

    // Add scoring card base chips
    for (int i = 0; i < game->preview_eval.scoring_count; ++i) {
        score_state_add_chips(&game->preview_score, card_get_base_chips(game->preview_eval.scoring_cards[i]));
    }
}

void game_advance_next_blind(GameSession* game) {
    if (!game) return;
    if (game->current_blind == BLIND_SMALL) {
        game_session_select_blind(game, BLIND_BIG);
    } else if (game->current_blind == BLIND_BIG) {
        game_session_select_blind(game, BLIND_BOSS);
    } else {
        game->ante++;
        game_session_select_blind(game, BLIND_SMALL);
    }
    game_update_selection_preview(game);
}
