/*******************************************************************************
 * @file        Search.h
 * @brief       Defines searching and filtering methods for the cards
 *
 * @author      John Rosenberger
 * @date        2026-07-15
 * @copyright   me
 *******************************************************************************/
#ifndef SEARCH_H
#define SEARCH_H

#include <stdint.h> //for int8_t
#include <stdbool.h> //for bool
#include "Card.h" //for Card

typedef enum {
    NAME,
    PRICE,
    VALUE,
    RARITY,
    SET
} SortValue;

typedef struct {
    double minPrice;  double maxPrice;
    double minValue;  double maxValue;
    int8_t minRarity; int8_t maxRarity;
    int8_t minGrade;  int8_t maxGrade;
    int8_t stage;
    int8_t type;
    int minHp;
    int maxHp;
    char cardNumber[CARD_NUMBER_SIZE];
    bool owned;
    bool unowned;
} FilterBounds;

bool filterCard(const Card* card, FilterBounds bounds); //returns a filtered int array of the matches
bool filterOwnedCard(const OwnedCard* card, FilterBounds bounds); //returns a filtered int array of the matches
int* changeFilter(const CardArrayList* library, int* searchResults, FilterBounds bounds, int matchCount, int* filteredCount);
int* changeOwnedCardFilter(const OwnedCardArrayList* library, int* searchResults, FilterBounds bounds, int matchCount, int* filteredCount);
int* searchCards(const CardArrayList* library, const char* name, int* matchCount);//returns an int array for the indexes of matches
int* searchOwnedCards(const OwnedCardArrayList* library, const char* name, int* matchCount);//returns an int array for the indexes of matches
int* sortAllCards(int* searchResults, int* matchCount, SortValue sort, bool descending);//sorts and returns a global int array
int* sortOwnedCards(int* searchResults, int* matchCount, SortValue sort, bool descending);//sorts and returns a global int array

#endif
