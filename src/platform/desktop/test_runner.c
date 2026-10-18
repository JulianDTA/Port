#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "core/card.h"
#include "core/poker_hand.h"
#include "core/scoring.h"
#include "core/jokers.h"
#include "core/deck.h"
#include "core/game_state.h"

static int tests_passed = 0;
static int tests_total = 0;

#define TEST(name) do { printf("[TEST] %s...\n", name); tests_total++; } while(0)
#define PASS() do { printf("[PASS]\n\n"); tests_passed++; } while(0)

/* ========================================================================= */
/*  1. Hand Detection Tests                                                  */
/* ========================================================================= */

void test_high_card(void) {
    TEST("High Card detection");
    Card c1, c2, c3, c4, c5;
    card_init(&c1, 1, SUIT_SPADES,   RANK_ACE);
    card_init(&c2, 2, SUIT_HEARTS,   RANK_KING);
    card_init(&c3, 3, SUIT_CLUBS,    RANK_9);
    card_init(&c4, 4, SUIT_DIAMONDS, RANK_5);
    card_init(&c5, 5, SUIT_HEARTS,   RANK_3);
    const Card* cards[5] = { &c1, &c2, &c3, &c4, &c5 };
    HandEvaluation eval = poker_evaluate_played(cards, 5, NULL);
    printf("  Detected: %s (scoring %d cards)\n", poker_hand_name(eval.type), eval.scoring_count);
    assert(eval.type == HAND_HIGH_CARD);
    assert(eval.scoring_count == 1);
    // base: 5 chips, 1 mult
    assert(eval.initial_chips == 5);
    assert(eval.initial_mult == 1);
    PASS();
}

void test_pair(void) {
    TEST("Pair detection");
    Card c1, c2, c3, c4, c5;
    card_init(&c1, 1, SUIT_SPADES,   RANK_JACK);
    card_init(&c2, 2, SUIT_HEARTS,   RANK_JACK);
    card_init(&c3, 3, SUIT_CLUBS,    RANK_9);
    card_init(&c4, 4, SUIT_DIAMONDS, RANK_5);
    card_init(&c5, 5, SUIT_HEARTS,   RANK_3);
    const Card* cards[5] = { &c1, &c2, &c3, &c4, &c5 };
    HandEvaluation eval = poker_evaluate_played(cards, 5, NULL);
    printf("  Detected: %s (scoring %d cards)\n", poker_hand_name(eval.type), eval.scoring_count);
    assert(eval.type == HAND_PAIR);
    assert(eval.scoring_count == 2);
    assert(eval.initial_chips == 10);
    assert(eval.initial_mult == 2);
    PASS();
}

void test_two_pair(void) {
    TEST("Two Pair detection");
    Card c1, c2, c3, c4, c5;
    card_init(&c1, 1, SUIT_SPADES,   RANK_ACE);
    card_init(&c2, 2, SUIT_HEARTS,   RANK_ACE);
    card_init(&c3, 3, SUIT_CLUBS,    RANK_KING);
    card_init(&c4, 4, SUIT_DIAMONDS, RANK_KING);
    card_init(&c5, 5, SUIT_HEARTS,   RANK_2);
    const Card* cards[5] = { &c1, &c2, &c3, &c4, &c5 };
    HandEvaluation eval = poker_evaluate_played(cards, 5, NULL);
    printf("  Detected: %s (scoring %d cards)\n", poker_hand_name(eval.type), eval.scoring_count);
    assert(eval.type == HAND_TWO_PAIR);
    assert(eval.scoring_count == 4);
    assert(eval.initial_chips == 20);
    assert(eval.initial_mult == 2);
    PASS();
}

void test_three_of_a_kind(void) {
    TEST("Three of a Kind detection");
    Card c1, c2, c3, c4, c5;
    card_init(&c1, 1, SUIT_SPADES,   RANK_7);
    card_init(&c2, 2, SUIT_HEARTS,   RANK_7);
    card_init(&c3, 3, SUIT_CLUBS,    RANK_7);
    card_init(&c4, 4, SUIT_DIAMONDS, RANK_KING);
    card_init(&c5, 5, SUIT_HEARTS,   RANK_2);
    const Card* cards[5] = { &c1, &c2, &c3, &c4, &c5 };
    HandEvaluation eval = poker_evaluate_played(cards, 5, NULL);
    printf("  Detected: %s (scoring %d cards)\n", poker_hand_name(eval.type), eval.scoring_count);
    assert(eval.type == HAND_THREE_OF_A_KIND);
    assert(eval.scoring_count == 3);
    assert(eval.initial_chips == 30);
    assert(eval.initial_mult == 3);
    PASS();
}

void test_straight(void) {
    TEST("Straight detection (regular)");
    Card c1, c2, c3, c4, c5;
    card_init(&c1, 1, SUIT_SPADES,   RANK_10);
    card_init(&c2, 2, SUIT_HEARTS,   RANK_9);
    card_init(&c3, 3, SUIT_CLUBS,    RANK_8);
    card_init(&c4, 4, SUIT_DIAMONDS, RANK_7);
    card_init(&c5, 5, SUIT_SPADES,   RANK_6);
    const Card* cards[5] = { &c1, &c2, &c3, &c4, &c5 };
    HandEvaluation eval = poker_evaluate_played(cards, 5, NULL);
    printf("  Detected: %s\n", poker_hand_name(eval.type));
    assert(eval.type == HAND_STRAIGHT);
    assert(eval.scoring_count == 5);
    assert(eval.initial_chips == 30);
    assert(eval.initial_mult == 4);
    PASS();
}

void test_straight_ace_low(void) {
    TEST("Straight detection (Ace-low: A-2-3-4-5)");
    Card c1, c2, c3, c4, c5;
    card_init(&c1, 1, SUIT_SPADES,   RANK_ACE);
    card_init(&c2, 2, SUIT_HEARTS,   RANK_2);
    card_init(&c3, 3, SUIT_CLUBS,    RANK_3);
    card_init(&c4, 4, SUIT_DIAMONDS, RANK_4);
    card_init(&c5, 5, SUIT_SPADES,   RANK_5);
    const Card* cards[5] = { &c1, &c2, &c3, &c4, &c5 };
    HandEvaluation eval = poker_evaluate_played(cards, 5, NULL);
    printf("  Detected: %s\n", poker_hand_name(eval.type));
    assert(eval.type == HAND_STRAIGHT);
    PASS();
}

void test_flush(void) {
    TEST("Flush detection");
    Card c1, c2, c3, c4, c5;
    card_init(&c1, 1, SUIT_HEARTS, RANK_ACE);
    card_init(&c2, 2, SUIT_HEARTS, RANK_QUEEN);
    card_init(&c3, 3, SUIT_HEARTS, RANK_10);
    card_init(&c4, 4, SUIT_HEARTS, RANK_7);
    card_init(&c5, 5, SUIT_HEARTS, RANK_2);
    const Card* cards[5] = { &c1, &c2, &c3, &c4, &c5 };
    HandEvaluation eval = poker_evaluate_played(cards, 5, NULL);
    printf("  Detected: %s\n", poker_hand_name(eval.type));
    assert(eval.type == HAND_FLUSH);
    assert(eval.scoring_count == 5);
    assert(eval.initial_chips == 35);
    assert(eval.initial_mult == 4);
    PASS();
}

void test_full_house(void) {
    TEST("Full House detection");
    Card c1, c2, c3, c4, c5;
    card_init(&c1, 1, SUIT_SPADES,   RANK_QUEEN);
    card_init(&c2, 2, SUIT_HEARTS,   RANK_QUEEN);
    card_init(&c3, 3, SUIT_CLUBS,    RANK_QUEEN);
    card_init(&c4, 4, SUIT_DIAMONDS, RANK_4);
    card_init(&c5, 5, SUIT_HEARTS,   RANK_4);
    const Card* cards[5] = { &c1, &c2, &c3, &c4, &c5 };
    HandEvaluation eval = poker_evaluate_played(cards, 5, NULL);
    printf("  Detected: %s (scoring %d cards)\n", poker_hand_name(eval.type), eval.scoring_count);
    assert(eval.type == HAND_FULL_HOUSE);
    assert(eval.scoring_count == 5);
    assert(eval.initial_chips == 40);
    assert(eval.initial_mult == 4);
    PASS();
}

void test_four_of_a_kind(void) {
    TEST("Four of a Kind detection");
    Card c1, c2, c3, c4, c5;
    card_init(&c1, 1, SUIT_SPADES,   RANK_8);
    card_init(&c2, 2, SUIT_HEARTS,   RANK_8);
    card_init(&c3, 3, SUIT_CLUBS,    RANK_8);
    card_init(&c4, 4, SUIT_DIAMONDS, RANK_8);
    card_init(&c5, 5, SUIT_HEARTS,   RANK_ACE);
    const Card* cards[5] = { &c1, &c2, &c3, &c4, &c5 };
    HandEvaluation eval = poker_evaluate_played(cards, 5, NULL);
    printf("  Detected: %s (scoring %d cards)\n", poker_hand_name(eval.type), eval.scoring_count);
    assert(eval.type == HAND_FOUR_OF_A_KIND);
    assert(eval.scoring_count == 4);
    assert(eval.initial_chips == 60);
    assert(eval.initial_mult == 7);
    PASS();
}

void test_straight_flush(void) {
    TEST("Straight Flush detection");
    Card c1, c2, c3, c4, c5;
    card_init(&c1, 1, SUIT_DIAMONDS, RANK_9);
    card_init(&c2, 2, SUIT_DIAMONDS, RANK_8);
    card_init(&c3, 3, SUIT_DIAMONDS, RANK_7);
    card_init(&c4, 4, SUIT_DIAMONDS, RANK_6);
    card_init(&c5, 5, SUIT_DIAMONDS, RANK_5);
    const Card* cards[5] = { &c1, &c2, &c3, &c4, &c5 };
    HandEvaluation eval = poker_evaluate_played(cards, 5, NULL);
    printf("  Detected: %s\n", poker_hand_name(eval.type));
    assert(eval.type == HAND_STRAIGHT_FLUSH);
    assert(eval.scoring_count == 5);
    assert(eval.initial_chips == 100);
    assert(eval.initial_mult == 8);
    PASS();
}

/* ========================================================================= */
/*  2. Scoring Pipeline Tests                                                */
/* ========================================================================= */

void test_scoring_basic(void) {
    TEST("Basic scoring: Pair of Aces with no Jokers");
    Card c1, c2, c3;
    card_init(&c1, 1, SUIT_SPADES, RANK_ACE);
    card_init(&c2, 2, SUIT_HEARTS, RANK_ACE);
    card_init(&c3, 3, SUIT_CLUBS,  RANK_5);
    const Card* cards[3] = { &c1, &c2, &c3 };
    HandEvaluation eval = poker_evaluate_played(cards, 3, NULL);
    assert(eval.type == HAND_PAIR);

    ScoreState score;
    score_state_init(&score, eval.initial_chips, (double)eval.initial_mult);
    // Add card chips for scored cards (2 Aces)
    for (int i = 0; i < eval.scoring_count; ++i) {
        score_state_add_chips(&score, card_get_base_chips(eval.scoring_cards[i]));
    }
    int64_t final = score_state_calculate_final(&score);
    // Pair base: 10 chips + 2 * Ace(11) = 32 chips, x 2 mult = 64
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 64)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 64);
    PASS();
}

void test_scoring_with_enhancements(void) {
    TEST("Scoring with card enhancements: Bonus + Mult cards");
    Card c1, c2;
    card_init(&c1, 1, SUIT_SPADES, RANK_ACE);
    c1.enhancement = ENHANCEMENT_BONUS; // +30 chips
    card_init(&c2, 2, SUIT_HEARTS, RANK_ACE);
    c2.enhancement = ENHANCEMENT_MULT; // +4 mult
    const Card* cards[2] = { &c1, &c2 };
    HandEvaluation eval = poker_evaluate_played(cards, 2, NULL);
    assert(eval.type == HAND_PAIR);

    ScoreState score;
    score_state_init(&score, eval.initial_chips, (double)eval.initial_mult);
    // Score each card
    for (int i = 0; i < eval.scoring_count; ++i) {
        score_state_add_chips(&score, card_get_base_chips(eval.scoring_cards[i]));
        if (eval.scoring_cards[i]->enhancement == ENHANCEMENT_BONUS) {
            score_state_add_chips(&score, 30);
        } else if (eval.scoring_cards[i]->enhancement == ENHANCEMENT_MULT) {
            score_state_add_mult(&score, 4.0);
        }
    }
    int64_t final = score_state_calculate_final(&score);
    // Pair base: 10 chips + Ace(11)+Bonus(30) + Ace(11) = 62 chips, mult = 2 + 4 = 6
    // 62 * 6 = 372
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 372)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 372);
    PASS();
}

void test_scoring_glass_card(void) {
    TEST("Scoring with Glass card (x2 Mult)");
    Card c1, c2;
    card_init(&c1, 1, SUIT_SPADES, RANK_KING);
    c1.enhancement = ENHANCEMENT_GLASS;
    card_init(&c2, 2, SUIT_HEARTS, RANK_KING);
    const Card* cards[2] = { &c1, &c2 };
    HandEvaluation eval = poker_evaluate_played(cards, 2, NULL);
    assert(eval.type == HAND_PAIR);

    ScoreState score;
    score_state_init(&score, eval.initial_chips, (double)eval.initial_mult);
    for (int i = 0; i < eval.scoring_count; ++i) {
        score_state_add_chips(&score, card_get_base_chips(eval.scoring_cards[i]));
        if (eval.scoring_cards[i]->enhancement == ENHANCEMENT_GLASS) {
            score_state_multiply_mult(&score, 2.0);
        }
    }
    int64_t final = score_state_calculate_final(&score);
    // Pair: 10 + King(10) + King(10) = 30 chips, mult = 2 * 2 (glass) = 4
    // 30 * 4 = 120
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 120)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 120);
    PASS();
}

/* ========================================================================= */
/*  3. Joker Effect Tests                                                    */
/* ========================================================================= */

void test_joker_basic_mult(void) {
    TEST("Joker (+4 Mult) effect");
    Joker j;
    joker_create(&j, JOKER_JOKER, EDITION_NONE);

    ScoreState score;
    score_state_init(&score, 30, 3.0);
    RoundContext ctx = { .remaining_discards = 3, .remaining_hands = 4, .current_dollars = 5 };
    Joker* jokers[1] = { &j };
    joker_trigger_hand_evaluated(jokers, 1, HAND_PAIR, 2, &ctx, &score);

    int64_t final = score_state_calculate_final(&score);
    // 30 * (3 + 4) = 210
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 210)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 210);
    PASS();
}

void test_joker_cavendish_x3(void) {
    TEST("Cavendish (x3 Mult) effect");
    Joker j;
    joker_create(&j, JOKER_CAVENDISH, EDITION_NONE);

    ScoreState score;
    score_state_init(&score, 50, 4.0);
    RoundContext ctx = { .remaining_discards = 3, .remaining_hands = 4, .current_dollars = 5 };
    Joker* jokers[1] = { &j };
    joker_trigger_hand_evaluated(jokers, 1, HAND_FLUSH, 5, &ctx, &score);

    int64_t final = score_state_calculate_final(&score);
    // 50 * (4 * 3) = 50 * 12 = 600
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 600)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 600);
    PASS();
}

void test_joker_banner_discards(void) {
    TEST("Banner (+30 Chips per remaining discard)");
    Joker j;
    joker_create(&j, JOKER_BANNER, EDITION_NONE);

    ScoreState score;
    score_state_init(&score, 10, 2.0);
    RoundContext ctx = { .remaining_discards = 3, .remaining_hands = 4, .current_dollars = 5 };
    Joker* jokers[1] = { &j };
    joker_trigger_hand_evaluated(jokers, 1, HAND_HIGH_CARD, 1, &ctx, &score);

    int64_t final = score_state_calculate_final(&score);
    // chips = 10 + (3 * 30) = 100, mult = 2, final = 200
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 200)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 200);
    PASS();
}

void test_joker_baron_kings_in_hand(void) {
    TEST("Baron (x1.5 Mult per King held in hand)");
    Joker j;
    joker_create(&j, JOKER_BARON, EDITION_NONE);

    Card k1, k2;
    card_init(&k1, 1, SUIT_SPADES, RANK_KING);
    card_init(&k2, 2, SUIT_HEARTS, RANK_KING);

    ScoreState score;
    score_state_init(&score, 40, 4.0);
    Joker* jokers[1] = { &j };
    joker_trigger_held_in_hand(jokers, 1, &k1, &score);
    joker_trigger_held_in_hand(jokers, 1, &k2, &score);

    int64_t final = score_state_calculate_final(&score);
    // 40 * (4 * 1.5 * 1.5) = 40 * 9 = 360
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 360)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 360);
    PASS();
}

void test_joker_edition_foil(void) {
    TEST("Joker with Foil Edition (+50 Chips)");
    Joker j;
    joker_create(&j, JOKER_JOKER, EDITION_FOIL);

    ScoreState score;
    score_state_init(&score, 10, 2.0);
    RoundContext ctx = { .remaining_discards = 3, .remaining_hands = 4, .current_dollars = 5 };
    Joker* jokers[1] = { &j };
    joker_trigger_hand_evaluated(jokers, 1, HAND_PAIR, 2, &ctx, &score);

    int64_t final = score_state_calculate_final(&score);
    // chips = 10 + 50 (foil) = 60, mult = 2 + 4 (joker) = 6, final = 360
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 360)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 360);
    PASS();
}

void test_multi_joker_combo(void) {
    TEST("Multiple Jokers combo: Joker + Cavendish + Foil Joker");
    Joker j1, j2, j3;
    joker_create(&j1, JOKER_JOKER, EDITION_NONE);       // +4 mult
    joker_create(&j2, JOKER_CAVENDISH, EDITION_NONE);    // x3 mult
    joker_create(&j3, JOKER_JOKER, EDITION_FOIL);        // +4 mult, +50 chips (foil)

    ScoreState score;
    score_state_init(&score, 30, 3.0);
    RoundContext ctx = { .remaining_discards = 2, .remaining_hands = 3, .current_dollars = 10 };
    Joker* jokers[3] = { &j1, &j2, &j3 };
    joker_trigger_hand_evaluated(jokers, 3, HAND_STRAIGHT, 5, &ctx, &score);

    int64_t final = score_state_calculate_final(&score);
    // j1: +4 mult -> mult=7, foil=none so +0 chips
    // j2: x3 mult -> mult=21, no edition
    // j3: +4 mult -> mult=25, foil +50 chips -> chips=80
    // final = 80 * 25 = 2000
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 2000)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 2000);
    PASS();
}

void test_joker_odd_todd(void) {
    TEST("Odd Todd (+31 Chips for odd rank cards)");
    Joker j;
    joker_create(&j, JOKER_ODD_TODD, EDITION_NONE);

    Card c1, c2;
    card_init(&c1, 1, SUIT_SPADES, RANK_ACE); // 14 -> in Balatro Ace is odd nominal 11
    card_init(&c2, 2, SUIT_HEARTS, RANK_7);   // 7 is odd
    ScoreState score;
    score_state_init(&score, 10, 2.0);

    Joker* jokers[1] = { &j };
    joker_trigger_card_scored(jokers, 1, &c1, &score);
    joker_trigger_card_scored(jokers, 1, &c2, &score);

    // Initial 10 chips + 31 (Ace) + 31 (7) = 72 chips. Mult = 2. Total = 144.
    int64_t final = score_state_calculate_final(&score);
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 144)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 144);
    PASS();
}

void test_joker_scary_and_smiley(void) {
    TEST("Scary Face (+30 Chips) & Smiley Face (+5 Mult) on face cards");
    Joker scary, smiley;
    joker_create(&scary, JOKER_SCARY_FACE, EDITION_NONE);
    joker_create(&smiley, JOKER_SMILEY_FACE, EDITION_NONE);

    Card j1, q1;
    card_init(&j1, 1, SUIT_SPADES, RANK_JACK);
    card_init(&q1, 2, SUIT_HEARTS, RANK_QUEEN);
    ScoreState score;
    score_state_init(&score, 10, 2.0);

    Joker* jokers[2] = { &scary, &smiley };
    joker_trigger_card_scored(jokers, 2, &j1, &score);
    joker_trigger_card_scored(jokers, 2, &q1, &score);

    // Initial: 10 chips, 2 mult
    // j1 scored: scary adds +30 chips, smiley adds +5 mult
    // q1 scored: scary adds +30 chips, smiley adds +5 mult
    // Total chips = 10 + 60 = 70. Total mult = 2 + 10 = 12. Final = 70 * 12 = 840.
    int64_t final = score_state_calculate_final(&score);
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 840)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 840);
    PASS();
}

void test_joker_abstract(void) {
    TEST("Abstract Joker (+3 Mult per Joker held)");
    Joker abs_j;
    joker_create(&abs_j, JOKER_ABSTRACT, EDITION_NONE);

    ScoreState score;
    score_state_init(&score, 20, 2.0);
    RoundContext ctx = { .jokers_count = 4 }; // 4 Jokers equipped
    Joker* jokers[1] = { &abs_j };
    joker_trigger_hand_evaluated(jokers, 1, HAND_PAIR, 2, &ctx, &score);

    // Mult = 2 + (4 * 3) = 14. Chips = 20. Total = 280.
    int64_t final = score_state_calculate_final(&score);
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 280)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 280);
    PASS();
}

void test_joker_stencil(void) {
    TEST("Joker Stencil (X1 Mult per empty Joker slot)");
    Joker stencil;
    joker_create(&stencil, JOKER_STENCIL, EDITION_NONE);

    ScoreState score;
    score_state_init(&score, 20, 4.0);
    // 5 max slots, 2 jokers held -> 3 empty slots -> (1.0 + 3) = x4 Mult
    RoundContext ctx = { .max_joker_slots = 5, .jokers_count = 2 };
    Joker* jokers[1] = { &stencil };
    joker_trigger_hand_evaluated(jokers, 1, HAND_PAIR, 2, &ctx, &score);

    // Mult = 4 * 4 = 16. Chips = 20. Total = 320.
    int64_t final = score_state_calculate_final(&score);
    printf("  Chips: %lld, Mult: %.1f, Final: %lld (expected 320)\n",
           (long long)score.chips, score.mult, (long long)final);
    assert(final == 320);
    PASS();
}

void test_joker_ice_cream_decay(void) {
    TEST("Ice Cream (+100 Chips, -5 Chips per hand played)");
    Joker ice;
    joker_create(&ice, JOKER_ICE_CREAM, EDITION_NONE);
    assert(ice.custom_counter == 100);

    ScoreState score;
    score_state_init(&score, 10, 2.0);
    RoundContext ctx = { 0 };
    Joker* jokers[1] = { &ice };
    joker_trigger_hand_evaluated(jokers, 1, HAND_PAIR, 2, &ctx, &score);

    // Chips = 10 + 100 = 110. Mult = 2. Total = 220.
    int64_t final = score_state_calculate_final(&score);
    assert(final == 220);

    // Hand finishes
    joker_trigger_hand_played(jokers, 1, &ctx);
    printf("  Ice Cream counter after 1 hand: %d (expected 95)\n", ice.custom_counter);
    assert(ice.custom_counter == 95);
    PASS();
}

/* ========================================================================= */
/*  4. Blind Target Tests                                                    */
/* ========================================================================= */

void test_blind_targets(void) {
    TEST("Blind score targets for Antes 1-8");
    // Expected: 300, 800, 2000, 5000, 11000, 20000, 35000, 50000
    int64_t expected_small[] = { 300, 800, 2000, 5000, 11000, 20000, 35000, 50000 };
    for (int ante = 1; ante <= 8; ++ante) {
        int64_t target = game_calculate_blind_target(ante, BLIND_SMALL);
        printf("  Ante %d Small Blind: %lld (expected %lld)\n", ante, (long long)target, (long long)expected_small[ante-1]);
        assert(target == expected_small[ante - 1]);
    }

    // Big blind = 1.5x, Boss = 2x
    int64_t big1 = game_calculate_blind_target(1, BLIND_BIG);
    int64_t boss1 = game_calculate_blind_target(1, BLIND_BOSS);
    printf("  Ante 1 Big Blind: %lld (expected 450), Boss: %lld (expected 600)\n", (long long)big1, (long long)boss1);
    assert(big1 == 450);
    assert(boss1 == 600);
    PASS();
}

/* ========================================================================= */
/*  5. Deck Management Tests                                                 */
/* ========================================================================= */

void test_deck_standard(void) {
    TEST("Standard deck initialization (52 cards)");
    DeckManager dm;
    deck_init_standard(&dm);
    assert(dm.draw_pile.count == 52);
    assert(dm.hand.count == 0);
    assert(dm.discard_pile.count == 0);
    printf("  Draw pile: %d cards\n", dm.draw_pile.count);
    PASS();
}

void test_deck_shuffle_and_draw(void) {
    TEST("Deck shuffle & draw up to hand limit");
    DeckManager dm;
    deck_init_standard(&dm);
    deck_shuffle(&dm.draw_pile, 0xDEADBEEF);
    deck_draw_up_to_limit(&dm);
    assert(dm.hand.count == 8);
    assert(dm.draw_pile.count == 44);
    printf("  Hand: %d cards, Draw pile: %d cards\n", dm.hand.count, dm.draw_pile.count);
    PASS();
}

void test_deck_discard(void) {
    TEST("Deck discard selected cards");
    DeckManager dm;
    deck_init_standard(&dm);
    deck_shuffle(&dm.draw_pile, 0xCAFEBABE);
    deck_draw_up_to_limit(&dm);

    // Select first 3 cards for discard
    dm.hand.cards[0].is_selected = true;
    dm.hand.cards[1].is_selected = true;
    dm.hand.cards[2].is_selected = true;
    deck_discard_selected(&dm);

    assert(dm.hand.count == 5);
    assert(dm.discard_pile.count == 3);
    printf("  After discard: Hand=%d, Discard=%d\n", dm.hand.count, dm.discard_pile.count);
    PASS();
}

/* ========================================================================= */
/*  6. Full Game Round Simulation                                            */
/* ========================================================================= */

void test_full_round(void) {
    TEST("Full round simulation: play until victory or defeat");
    GameSession game;
    game_session_init(&game);
    game_session_start_run(&game);

    assert(game.current_state == STATE_ROUND_ACTIVE);
    assert(game.ante == 1);
    assert(game.target_score == 300);
    assert(game.hands_left == 4);
    assert(game.deck.hand.count == 8);

    printf("  Target: %lld, Starting with %d cards in hand\n",
           (long long)game.target_score, game.deck.hand.count);

    int rounds_played = 0;
    while (game.current_state == STATE_ROUND_ACTIVE && rounds_played < 10) {
        // Select first 5 cards
        for (int i = 0; i < 5 && i < game.deck.hand.count; ++i) {
            game.deck.hand.cards[i].is_selected = true;
        }
        game_play_selected_hand(&game);
        rounds_played++;
        printf("  Hand %d: %s -> +%lld pts (total: %lld / %lld)\n",
               rounds_played, poker_hand_name(game.last_eval.type),
               (long long)game.last_score.final_score,
               (long long)game.current_round_score, (long long)game.target_score);
    }

    printf("  Final state: %s (after %d hands)\n",
           game.current_state == STATE_ROUND_VICTORY ? "VICTORY" : "GAME OVER", rounds_played);
    // We should have either won or lost
    assert(game.current_state == STATE_ROUND_VICTORY || game.current_state == STATE_GAME_OVER);
    PASS();
}

void test_hand_level_scaling(void) {
    TEST("Hand level scaling: Level 3 Pair");
    int32_t levels[13] = { 0 };
    levels[HAND_PAIR] = 3; // Level 3

    Card c1, c2;
    card_init(&c1, 1, SUIT_SPADES, RANK_ACE);
    card_init(&c2, 2, SUIT_HEARTS, RANK_ACE);
    const Card* cards[2] = { &c1, &c2 };
    HandEvaluation eval = poker_evaluate_played(cards, 2, levels);

    // Pair level 3: base 10 + (2 * 15) = 40 chips, mult 2 + (2 * 1) = 4
    printf("  Level 3 Pair: Chips=%lld, Mult=%lld\n", (long long)eval.initial_chips, (long long)eval.initial_mult);
    assert(eval.initial_chips == 40);
    assert(eval.initial_mult == 4);
    PASS();
}

/* ========================================================================= */
/*  Main                                                                     */
/* ========================================================================= */

int main(void) {
    printf("=========================================================\n");
    printf("  Balatro-DSi Core Engine - Comprehensive Test Suite      \n");
    printf("=========================================================\n\n");

    // Hand Detection
    test_high_card();
    test_pair();
    test_two_pair();
    test_three_of_a_kind();
    test_straight();
    test_straight_ace_low();
    test_flush();
    test_full_house();
    test_four_of_a_kind();
    test_straight_flush();

    // Scoring
    test_scoring_basic();
    test_scoring_with_enhancements();
    test_scoring_glass_card();

    // Jokers
    test_joker_basic_mult();
    test_joker_cavendish_x3();
    test_joker_banner_discards();
    test_joker_baron_kings_in_hand();
    test_joker_edition_foil();
    test_multi_joker_combo();
    test_joker_odd_todd();
    test_joker_scary_and_smiley();
    test_joker_abstract();
    test_joker_stencil();
    test_joker_ice_cream_decay();

    // Blinds
    test_blind_targets();

    // Deck
    test_deck_standard();
    test_deck_shuffle_and_draw();
    test_deck_discard();

    // Full Game
    test_full_round();
    test_hand_level_scaling();

    printf("=========================================================\n");
    printf("  %d / %d TESTS PASSED\n", tests_passed, tests_total);
    if (tests_passed == tests_total) {
        printf("  ALL TESTS PASSED SUCCESSFULLY!\n");
    } else {
        printf("  SOME TESTS FAILED!\n");
    }
    printf("=========================================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
