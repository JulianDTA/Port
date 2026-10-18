#include "core/jokers.h"
#include <string.h>

// -------------------------------------------------------------
// Callbacks for Joker Hand Evaluation
// -------------------------------------------------------------

static void cb_joker_hand_eval(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)count; (void)ctx;
    score_state_add_mult(score, 4.0);
}

static void cb_greedy_card(Joker* self, const Card* card, ScoreState* score) {
    (void)self;
    if (card->suit == SUIT_DIAMONDS || card->enhancement == ENHANCEMENT_WILD) {
        score_state_add_mult(score, 3.0);
    }
}

static void cb_lusty_card(Joker* self, const Card* card, ScoreState* score) {
    (void)self;
    if (card->suit == SUIT_HEARTS || card->enhancement == ENHANCEMENT_WILD) {
        score_state_add_mult(score, 3.0);
    }
}

static void cb_wrathful_card(Joker* self, const Card* card, ScoreState* score) {
    (void)self;
    if (card->suit == SUIT_SPADES || card->enhancement == ENHANCEMENT_WILD) {
        score_state_add_mult(score, 3.0);
    }
}

static void cb_gluttonous_card(Joker* self, const Card* card, ScoreState* score) {
    (void)self;
    if (card->suit == SUIT_CLUBS || card->enhancement == ENHANCEMENT_WILD) {
        score_state_add_mult(score, 3.0);
    }
}

static void cb_jolly_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)count; (void)ctx;
    if (hand == HAND_PAIR || hand == HAND_TWO_PAIR || hand == HAND_FULL_HOUSE) {
        score_state_add_mult(score, 8.0);
    }
}

static void cb_zany_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)count; (void)ctx;
    if (hand == HAND_THREE_OF_A_KIND || hand == HAND_FULL_HOUSE) {
        score_state_add_mult(score, 12.0);
    }
}

static void cb_mad_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)count; (void)ctx;
    if (hand == HAND_TWO_PAIR) {
        score_state_add_mult(score, 10.0);
    }
}

static void cb_crazy_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)count; (void)ctx;
    if (hand == HAND_STRAIGHT || hand == HAND_STRAIGHT_FLUSH) {
        score_state_add_mult(score, 12.0);
    }
}

static void cb_droll_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)count; (void)ctx;
    if (hand == HAND_FLUSH || hand == HAND_STRAIGHT_FLUSH || hand == HAND_FLUSH_HOUSE || hand == HAND_FLUSH_FIVE) {
        score_state_add_mult(score, 10.0);
    }
}

static void cb_sly_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)count; (void)ctx;
    if (hand == HAND_PAIR || hand == HAND_TWO_PAIR || hand == HAND_FULL_HOUSE) {
        score_state_add_chips(score, 50);
    }
}

static void cb_wily_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)count; (void)ctx;
    if (hand == HAND_THREE_OF_A_KIND || hand == HAND_FULL_HOUSE) {
        score_state_add_chips(score, 100);
    }
}

static void cb_clever_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)count; (void)ctx;
    if (hand == HAND_TWO_PAIR) {
        score_state_add_chips(score, 80);
    }
}

static void cb_devious_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)count; (void)ctx;
    if (hand == HAND_STRAIGHT || hand == HAND_STRAIGHT_FLUSH) {
        score_state_add_chips(score, 100);
    }
}

static void cb_crafty_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)count; (void)ctx;
    if (hand == HAND_FLUSH || hand == HAND_STRAIGHT_FLUSH || hand == HAND_FLUSH_HOUSE || hand == HAND_FLUSH_FIVE) {
        score_state_add_chips(score, 80);
    }
}

static void cb_half_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)ctx;
    if (count <= 3) {
        score_state_add_mult(score, 20.0);
    }
}

static void cb_banner_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)count;
    if (ctx && ctx->remaining_discards > 0) {
        score_state_add_chips(score, ctx->remaining_discards * 30);
    }
}

static void cb_mystic_summit_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)count;
    if (ctx && ctx->remaining_discards == 0) {
        score_state_add_mult(score, 15.0);
    }
}

static void cb_baron_held(Joker* self, const Card* card, ScoreState* score) {
    (void)self;
    if (card->rank == RANK_KING && card->enhancement != ENHANCEMENT_STONE) {
        score_state_multiply_mult(score, 1.5);
    }
}

static void cb_even_steven_card(Joker* self, const Card* card, ScoreState* score) {
    (void)self;
    if (card->enhancement == ENHANCEMENT_STONE) return;
    if (card->rank == RANK_2 || card->rank == RANK_4 || card->rank == RANK_6 ||
        card->rank == RANK_8 || card->rank == RANK_10) {
        score_state_add_mult(score, 4.0);
    }
}

static void cb_odd_todd_card(Joker* self, const Card* card, ScoreState* score) {
    (void)self;
    if (card->enhancement == ENHANCEMENT_STONE) return;
    if (card->rank == RANK_ACE || card->rank == RANK_9 || card->rank == RANK_7 ||
        card->rank == RANK_5 || card->rank == RANK_3) {
        score_state_add_chips(score, 31);
    }
}

static void cb_scholar_card(Joker* self, const Card* card, ScoreState* score) {
    (void)self;
    if (card->rank == RANK_ACE && card->enhancement != ENHANCEMENT_STONE) {
        score_state_add_chips(score, 20);
        score_state_add_mult(score, 4.0);
    }
}

static void cb_fibonacci_card(Joker* self, const Card* card, ScoreState* score) {
    (void)self;
    if (card->enhancement == ENHANCEMENT_STONE) return;
    if (card->rank == RANK_ACE || card->rank == RANK_2 || card->rank == RANK_3 ||
        card->rank == RANK_5 || card->rank == RANK_8) {
        score_state_add_mult(score, 8.0);
    }
}

static void cb_scary_face_card(Joker* self, const Card* card, ScoreState* score) {
    (void)self;
    if (card->is_face && card->enhancement != ENHANCEMENT_STONE) {
        score_state_add_chips(score, 30);
    }
}

static void cb_smiley_face_card(Joker* self, const Card* card, ScoreState* score) {
    (void)self;
    if (card->is_face && card->enhancement != ENHANCEMENT_STONE) {
        score_state_add_mult(score, 5.0);
    }
}

static void cb_abstract_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)count;
    if (ctx && ctx->jokers_count > 0) {
        score_state_add_mult(score, ctx->jokers_count * 3.0);
    }
}

static void cb_blue_joker_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)count;
    if (ctx && ctx->cards_remaining_in_deck > 0) {
        score_state_add_chips(score, ctx->cards_remaining_in_deck * 2);
    }
}

static void cb_stencil_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)count;
    if (ctx) {
        int empty_slots = ctx->max_joker_slots - ctx->jokers_count;
        if (empty_slots > 0) {
            score_state_multiply_mult(score, 1.0 + empty_slots);
        }
    }
}

static void cb_blackboard_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)count;
    if (!ctx) return;
    bool all_black = true;
    for (int i = 0; i < ctx->cards_in_hand_count; ++i) {
        const Card* c = ctx->cards_in_hand[i];
        if (c->enhancement == ENHANCEMENT_WILD) continue;
        if (c->suit != SUIT_SPADES && c->suit != SUIT_CLUBS) {
            all_black = false;
            break;
        }
    }
    if (all_black && ctx->cards_in_hand_count > 0) {
        score_state_multiply_mult(score, 3.0);
    }
}

static void cb_ice_cream_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)count; (void)ctx;
    if (self->custom_counter > 0) {
        score_state_add_chips(score, self->custom_counter);
    }
}

static void cb_ice_cream_played(Joker* self, const RoundContext* ctx) {
    (void)ctx;
    self->custom_counter -= 5;
    if (self->custom_counter < 0) self->custom_counter = 0;
}

static void cb_popcorn_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)count; (void)ctx;
    if (self->custom_counter > 0) {
        score_state_add_mult(score, (double)self->custom_counter);
    }
}

static void cb_popcorn_round_end(Joker* self, bool* out_destroy) {
    self->custom_counter -= 4;
    if (self->custom_counter <= 0 && out_destroy) {
        *out_destroy = true;
    }
}

static void cb_gros_michel_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)count; (void)ctx;
    score_state_add_mult(score, 15.0);
}

static void cb_cavendish_hand(Joker* self, HandType hand, int count, const RoundContext* ctx, ScoreState* score) {
    (void)self; (void)hand; (void)count; (void)ctx;
    score_state_multiply_mult(score, 3.0);
}

// -------------------------------------------------------------
// Joker Factory
// -------------------------------------------------------------

void joker_create(Joker* joker, JokerId id, CardEdition edition) {
    if (!joker) return;
    memset(joker, 0, sizeof(Joker));
    joker->id = id;
    joker->edition = edition;
    joker->cost = 4;
    joker->rarity = RARITY_COMMON;

    switch (id) {
        case JOKER_JOKER:
            joker->name = "Joker";
            joker->cost = 2;
            joker->on_hand_evaluated = cb_joker_hand_eval;
            break;
        case JOKER_GREEDY:
            joker->name = "Greedy Joker";
            joker->cost = 5;
            joker->on_card_scored = cb_greedy_card;
            break;
        case JOKER_LUSTY:
            joker->name = "Lusty Joker";
            joker->cost = 5;
            joker->on_card_scored = cb_lusty_card;
            break;
        case JOKER_WRATHFUL:
            joker->name = "Wrathful Joker";
            joker->cost = 5;
            joker->on_card_scored = cb_wrathful_card;
            break;
        case JOKER_GLUTTONOUS:
            joker->name = "Gluttonous Joker";
            joker->cost = 5;
            joker->on_card_scored = cb_gluttonous_card;
            break;
        case JOKER_JOLLY:
            joker->name = "Jolly Joker";
            joker->cost = 3;
            joker->on_hand_evaluated = cb_jolly_hand;
            break;
        case JOKER_ZANY:
            joker->name = "Zany Joker";
            joker->cost = 4;
            joker->on_hand_evaluated = cb_zany_hand;
            break;
        case JOKER_MAD:
            joker->name = "Mad Joker";
            joker->cost = 4;
            joker->on_hand_evaluated = cb_mad_hand;
            break;
        case JOKER_CRAZY:
            joker->name = "Crazy Joker";
            joker->cost = 4;
            joker->on_hand_evaluated = cb_crazy_hand;
            break;
        case JOKER_DROLL:
            joker->name = "Droll Joker";
            joker->cost = 4;
            joker->on_hand_evaluated = cb_droll_hand;
            break;
        case JOKER_SLY:
            joker->name = "Sly Joker";
            joker->cost = 3;
            joker->on_hand_evaluated = cb_sly_hand;
            break;
        case JOKER_WILY:
            joker->name = "Wily Joker";
            joker->cost = 4;
            joker->on_hand_evaluated = cb_wily_hand;
            break;
        case JOKER_CLEVER:
            joker->name = "Clever Joker";
            joker->cost = 4;
            joker->on_hand_evaluated = cb_clever_hand;
            break;
        case JOKER_DEVIOUS:
            joker->name = "Devious Joker";
            joker->cost = 4;
            joker->on_hand_evaluated = cb_devious_hand;
            break;
        case JOKER_CRAFTY:
            joker->name = "Crafty Joker";
            joker->cost = 4;
            joker->on_hand_evaluated = cb_crafty_hand;
            break;
        case JOKER_HALF:
            joker->name = "Half Joker";
            joker->cost = 5;
            joker->on_hand_evaluated = cb_half_hand;
            break;
        case JOKER_BANNER:
            joker->name = "Banner";
            joker->cost = 5;
            joker->on_hand_evaluated = cb_banner_hand;
            break;
        case JOKER_MYSTIC_SUMMIT:
            joker->name = "Mystic Summit";
            joker->cost = 5;
            joker->on_hand_evaluated = cb_mystic_summit_hand;
            break;
        case JOKER_BARON:
            joker->name = "Baron";
            joker->cost = 8;
            joker->rarity = RARITY_RARE;
            joker->on_held_in_hand = cb_baron_held;
            break;
        case JOKER_EVEN_STEVEN:
            joker->name = "Even Steven";
            joker->cost = 4;
            joker->on_card_scored = cb_even_steven_card;
            break;
        case JOKER_ODD_TODD:
            joker->name = "Odd Todd";
            joker->cost = 4;
            joker->on_card_scored = cb_odd_todd_card;
            break;
        case JOKER_SCHOLAR:
            joker->name = "Scholar";
            joker->cost = 4;
            joker->on_card_scored = cb_scholar_card;
            break;
        case JOKER_FIBONACCI:
            joker->name = "Fibonacci";
            joker->cost = 8;
            joker->rarity = RARITY_UNCOMMON;
            joker->on_card_scored = cb_fibonacci_card;
            break;
        case JOKER_SCARY_FACE:
            joker->name = "Scary Face";
            joker->cost = 4;
            joker->on_card_scored = cb_scary_face_card;
            break;
        case JOKER_SMILEY_FACE:
            joker->name = "Smiley Face";
            joker->cost = 4;
            joker->on_card_scored = cb_smiley_face_card;
            break;
        case JOKER_ABSTRACT:
            joker->name = "Abstract Joker";
            joker->cost = 4;
            joker->on_hand_evaluated = cb_abstract_hand;
            break;
        case JOKER_BLUE_JOKER:
            joker->name = "Blue Joker";
            joker->cost = 5;
            joker->on_hand_evaluated = cb_blue_joker_hand;
            break;
        case JOKER_STENCIL:
            joker->name = "Joker Stencil";
            joker->cost = 8;
            joker->rarity = RARITY_UNCOMMON;
            joker->on_hand_evaluated = cb_stencil_hand;
            break;
        case JOKER_BLACKBOARD:
            joker->name = "Blackboard";
            joker->cost = 6;
            joker->rarity = RARITY_UNCOMMON;
            joker->on_hand_evaluated = cb_blackboard_hand;
            break;
        case JOKER_ICE_CREAM:
            joker->name = "Ice Cream";
            joker->cost = 5;
            joker->custom_counter = 100; // 100 chips initially
            joker->on_hand_evaluated = cb_ice_cream_hand;
            joker->on_hand_played = cb_ice_cream_played;
            break;
        case JOKER_POPCORN:
            joker->name = "Popcorn";
            joker->cost = 5;
            joker->custom_counter = 20; // 20 mult initially
            joker->on_hand_evaluated = cb_popcorn_hand;
            joker->on_round_end = cb_popcorn_round_end;
            break;
        case JOKER_GROS_MICHEL:
            joker->name = "Gros Michel";
            joker->cost = 5;
            joker->on_hand_evaluated = cb_gros_michel_hand;
            break;
        case JOKER_CAVENDISH:
            joker->name = "Cavendish";
            joker->cost = 5;
            joker->on_hand_evaluated = cb_cavendish_hand;
            break;
        default:
            joker->name = "Blank Joker";
            break;
    }
}

static void apply_joker_edition(const Joker* j, ScoreState* score) {
    if (!j) return;
    switch (j->edition) {
        case EDITION_FOIL:
            score_state_add_chips(score, 50);
            break;
        case EDITION_HOLOGRAPHIC:
            score_state_add_mult(score, 10.0);
            break;
        case EDITION_POLYCHROME:
            score_state_multiply_mult(score, 1.5);
            break;
        default:
            break;
    }
}

void joker_trigger_card_scored(Joker* jokers[], int count, const Card* card, ScoreState* score) {
    for (int i = 0; i < count; ++i) {
        if (!jokers[i] || jokers[i]->is_debuffed) continue;
        if (jokers[i]->on_card_scored) {
            jokers[i]->on_card_scored(jokers[i], card, score);
        }
    }
}

void joker_trigger_held_in_hand(Joker* jokers[], int count, const Card* card, ScoreState* score) {
    for (int i = 0; i < count; ++i) {
        if (!jokers[i] || jokers[i]->is_debuffed) continue;
        if (jokers[i]->on_held_in_hand) {
            jokers[i]->on_held_in_hand(jokers[i], card, score);
        }
    }
}

void joker_trigger_hand_evaluated(Joker* jokers[], int count, HandType hand_type, int played_card_count, const RoundContext* ctx, ScoreState* score) {
    for (int i = 0; i < count; ++i) {
        if (!jokers[i] || jokers[i]->is_debuffed) continue;
        if (jokers[i]->on_hand_evaluated) {
            jokers[i]->on_hand_evaluated(jokers[i], hand_type, played_card_count, ctx, score);
        }
        apply_joker_edition(jokers[i], score);
    }
}

void joker_trigger_hand_played(Joker* jokers[], int count, const RoundContext* ctx) {
    for (int i = 0; i < count; ++i) {
        if (!jokers[i] || jokers[i]->is_debuffed) continue;
        if (jokers[i]->on_hand_played) {
            jokers[i]->on_hand_played(jokers[i], ctx);
        }
    }
}

void joker_trigger_round_end(Joker* jokers[], int* inout_count) {
    if (!inout_count) return;
    int count = *inout_count;
    int new_count = 0;
    for (int i = 0; i < count; ++i) {
        if (!jokers[i]) continue;
        bool destroy = false;
        if (jokers[i]->on_round_end) {
            jokers[i]->on_round_end(jokers[i], &destroy);
        }
        if (!destroy) {
            jokers[new_count++] = jokers[i];
        }
    }
    *inout_count = new_count;
}
