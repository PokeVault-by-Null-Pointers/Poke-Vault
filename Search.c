/*******************************************************************************
 * @file        Search.c
 * @brief       Defines searching and filtering methods for the cards
 *
 * @author      John Rosenberger
 * @date        2026-07-15
 * @copyright   me
 *******************************************************************************/
#include <stdint.h> //for int8_t
#include <stdbool.h> //for bool stuff
#include <string.h> //for string stuff
#include <stdlib.h> //for qsort and malloc
#include "Card.h" //for Card, OwnedCard, CardArrayList, OwnedArrayList
#include "Search.h" //needs this to be used
#include "Misc.h" //for upperCase

static const CardArrayList* cardLibrary;
static const OwnedCardArrayList* ownedCardLibrary;
static bool sortDescending = false;
int* searchResults;

//returns true if card passes filter
bool filterCard(const Card* card, FilterBounds bounds){
    if (card->value < bounds.minValue || card->value > bounds.maxValue){
        return false;
    }
    if (card->rarity < bounds.minRarity || card->rarity > bounds.maxRarity){
        return false;
    }
    if (bounds.stage != -1 && card->stage != bounds.stage){
        return false;
    }
    if (bounds.type != -1 && card->type != bounds.type){
        return false;
    }
    if (card->hp < bounds.minHp || card->hp > bounds.maxHp){
        return false;
    }
    if (bounds.cardNumber[0] != '\0' &&
        strcmp(card->cardNumber, bounds.cardNumber) != 0){
        return false;
    }
    if (bounds.owned && !(card->owned)){
        return false;
    }
    if (bounds.unowned && (card->owned)){
        return false;
    }
    return true;
}
bool filterOwnedCard(const OwnedCard* card, FilterBounds bounds){
    if (card->purchasePrice < bounds.minPrice || card->purchasePrice > bounds.maxPrice){
        return false;
    }
    if (card->value < bounds.minValue || card->value > bounds.maxValue){
        return false;
    }
    if (card->rarity < bounds.minRarity || card->rarity > bounds.maxRarity){
        return false;
    }
    if (card->grade < bounds.minGrade || card->grade > bounds.maxGrade){
        return false;
    }
    if (bounds.stage != -1 && card->stage != bounds.stage){
        return false;
    }
    if (bounds.type != -1 && card->type != bounds.type){
        return false;
    }
    if (card->hp < bounds.minHp || card->hp > bounds.maxHp){
        return false;
    }
    if (bounds.cardNumber[0] != '\0' &&
        strcmp(card->cardNumber, bounds.cardNumber) != 0){
        return false;
    }
    return true;
}
//returns a filtered int array for the indexes of matches
int* changeFilter(const CardArrayList* library, int* searchResults, FilterBounds bounds, int matchCount, int* filteredCount){
    Card currentCard;
    int* filterResults = malloc(sizeof(int) * matchCount);
    *filteredCount = 0;
    for(int i = 0; i < matchCount; i++){
        currentCard = library->cards[searchResults[i]]; //the index of the {searchResults[i]}'th card in library
        //if the card PASSES the filter
        if(filterCard(&currentCard, bounds)){
            filterResults[*filteredCount] = searchResults[i];
            (*filteredCount)++;
        }
    }
    filterResults = realloc( filterResults, (sizeof(int) * *filteredCount) );
    return filterResults;
}
int* changeOwnedCardFilter(const OwnedCardArrayList* library, int* searchResults, FilterBounds bounds, int matchCount, int* filteredCount){
    OwnedCard currentCard;
    int* filterResults = malloc(sizeof(int) * matchCount);
    *filteredCount = 0;
    for(int i = 0; i < matchCount; i++){
        currentCard = library->cards[searchResults[i]]; //the index of the {searchResults[i]}'th card in library
        //if the card PASSES the filter
        if(filterOwnedCard(&currentCard, bounds)){
            filterResults[*filteredCount] = searchResults[i];
            (*filteredCount)++;
        }
    }
    filterResults = realloc( filterResults, (sizeof(int) * *filteredCount) );
    return filterResults;
}


//linear search, returns an int array for the indexes of matches
int* searchCards(const CardArrayList* library, const char* name, int* matchCount){
    cardLibrary = library;
    Card* cards = library->cards; //library->cards   means   (*library).cards,   which means   library[0].cards
    *matchCount = 0; //same as matchCount[0] = 0.
    char* key = upperCase(name);//defined in Misc.c
    char* currentName;
    searchResults = malloc(sizeof(int) * library->size);
    int size = library->size;

    for(int i = 0; i < size; i++){
        currentName = upperCase(cards[i].name); //defined in Misc.c
        if(strstr(currentName, key) != NULL){
            searchResults[*matchCount] = i;
            (*matchCount)++; //*matchCount++ would be *(matchCount++)
        }
        free(currentName);
    }
    free(key);    
    searchResults = realloc( searchResults, (sizeof(int) * *matchCount) );
    return searchResults;
}
//linear search, returns an int array for the indexes of matches
int* searchOwnedCards(const OwnedCardArrayList* library, const char* name, int* matchCount){
    ownedCardLibrary = library;
    OwnedCard* cards = library->cards; //library->cards   means   (*library).cards,   which means   library[0].cards
    OwnedCard currentCard;
    *matchCount = 0; //same as matchCount[0] = 0.
    char* key = upperCase(name); //defined in Misc.c
    char* currentName;
    searchResults = malloc(sizeof(int) * library->size);
    int size = library->size;

    for(int i = 0; i < size; i++){
        currentCard = cards[i];
        currentName = upperCase(currentCard.name); //defined in Misc.c

        //check if the name matches
        if(strstr(currentName, key) != NULL){
            //add card
            searchResults[*matchCount] = i;
            (*matchCount)++; //*matchCount++ would be *(matchCount++)
        }
        free(currentName);
    }
    free(key);
    searchResults = realloc( searchResults, (sizeof(int) * *matchCount) );
    return searchResults;
}


//***************************************************************************************************************************
//used to sort all Cards
int compName(const void* first, const void* second){
    int a = *(const int*)first; //cast first to int*, then a = the dereferenced version of that
    int b = *(const int*)second;
    const Card* cardA = &(cardLibrary->cards[a]); // { (*cardLibrary).cards[a] }'s address
    const Card* cardB = &(cardLibrary->cards[b]);
    int result = strcmp(cardA->name, cardB->name);
    if(sortDescending == true){
        return result * -1;
    }
    else {
        return result;
    }
}
int compSet(const void* first, const void* second){
    int a = *(const int*)first; //cast first to int*, then a = the dereferenced version of that
    int b = *(const int*)second;
    const Card* cardA = &(cardLibrary->cards[a]); // { (*cardLibrary).cards[a] }'s address
    const Card* cardB = &(cardLibrary->cards[b]);
    int result = strcmp(cardA->set, cardB->set);
    if(sortDescending == true){
        return result * -1;
    }
    else {
        return result;
    }
}
int compValue(const void* first, const void* second){
    int a = *(const int*)first; //cast first to int*, then a = the dereferenced version of that
    int b = *(const int*)second;
    const Card* cardA = &(cardLibrary->cards[a]); // { (*cardLibrary).cards[a] }'s address
    const Card* cardB = &(cardLibrary->cards[b]);
    int result = 0;
    if (cardA->value < cardB->value){
        result = -1;
    }
    else if (cardA->value > cardB->value){
        result = 1;
    }
    if(sortDescending == true){
        return result * -1;
    }
    else {
        return result;
    }
}
int compRarity(const void* first, const void* second){
    int a = *(const int*)first; //cast first to int*, then a = the dereferenced version of that
    int b = *(const int*)second;
    const Card* cardA = &(cardLibrary->cards[a]); // { (*cardLibrary).cards[a] }'s address
    const Card* cardB = &(cardLibrary->cards[b]);
    int result = cardA->rarity - cardB->rarity;
    if(sortDescending == true){
        return result * -1;
    }
    else {
        return result;
    }
}
//returns an int array that is sorted by set for the indexes of matches
int* sortAllCards(int* searchResults, int* matchCount, SortValue sort, bool descending){
    sortDescending = descending;
    if(sort == NAME){
        qsort(searchResults, *matchCount, sizeof(*searchResults), compName);
    }
    else if(sort == VALUE){
        qsort(searchResults, *matchCount, sizeof(*searchResults), compValue);
    }
    else if(sort == RARITY){
        qsort(searchResults, *matchCount, sizeof(*searchResults), compRarity);
    }
    else if(sort == SET){
        qsort(searchResults, *matchCount, sizeof(*searchResults), compSet);
    }
    return searchResults;
}


//***************************************************************************************************************************
//used to sort OwnedCards
int compOwnedName(const void* first, const void* second){
    int a = *(const int*)first; //cast first to int*, then a = the dereferenced version of that
    int b = *(const int*)second;
    const OwnedCard* cardA = &(ownedCardLibrary->cards[a]); // { (*ownedCardLibrary).cards[a] }'s address
    const OwnedCard* cardB = &(ownedCardLibrary->cards[b]);
    int result = strcmp(cardA->name, cardB->name);
    if(sortDescending == true){
        return result * -1;
    }
    else {
        return result;
    }
}
int compOwnedSet(const void* first, const void* second){
    int a = *(const int*)first; //cast first to int*, then a = the dereferenced version of that
    int b = *(const int*)second;
    const OwnedCard* cardA = &(ownedCardLibrary->cards[a]); // { (*ownedCardLibrary).cards[a] }'s address
    const OwnedCard* cardB = &(ownedCardLibrary->cards[b]);
    int result = strcmp(cardA->set, cardB->set);
    if(sortDescending == true){
        return result * -1;
    }
    else {
        return result;
    }
}
int compOwnedPrice(const void* first, const void* second){
    int a = *(const int*)first; //cast first to int*, then a = the dereferenced version of that
    int b = *(const int*)second;
    const OwnedCard* cardA = &(ownedCardLibrary->cards[a]); // { (*ownedCardLibrary).cards[a] }'s address
    const OwnedCard* cardB = &(ownedCardLibrary->cards[b]);
    int result = 0;
    if (cardA->purchasePrice < cardB->purchasePrice){
        result = -1;
    }
    else if (cardA->purchasePrice > cardB->purchasePrice){
        result = 1;
    }
    if(sortDescending == true){
        return result * -1;
    }
    else {
        return result;
    }
}
int compOwnedValue(const void* first, const void* second){
    int a = *(const int*)first; //cast first to int*, then a = the dereferenced version of that
    int b = *(const int*)second;
    const OwnedCard* cardA = &(ownedCardLibrary->cards[a]); // { (*ownedCardLibrary).cards[a] }'s address
    const OwnedCard* cardB = &(ownedCardLibrary->cards[b]);
    int result = 0;
    if (cardA->value < cardB->value){
        result = -1;
    }
    else if (cardA->value > cardB->value){
        result = 1;
    }
    if(sortDescending == true){
        return result * -1;
    }
    else {
        return result;
    }
}
int compOwnedRarity(const void* first, const void* second){
    int a = *(const int*)first; //cast first to int*, then a = the dereferenced version of that
    int b = *(const int*)second;
    const OwnedCard* cardA = &(ownedCardLibrary->cards[a]); // { (*ownedCardLibrary).cards[a] }'s address
    const OwnedCard* cardB = &(ownedCardLibrary->cards[b]);
    int result = cardA->rarity - cardB->rarity;
    if(sortDescending == true){
        return result * -1;
    }
    else {
        return result;
    }
}
//returns an int array that is sorted by set for the indexes of matches
int* sortOwnedCards(int* searchResults, int* matchCount, SortValue sort, bool descending){
    sortDescending = descending;
    //SortValue is defined in Search.h
    if(sort == NAME){
        qsort(searchResults, *matchCount, sizeof(*searchResults), compOwnedName);
    }
    else if(sort == PRICE){
        qsort(searchResults, *matchCount, sizeof(*searchResults), compOwnedPrice);
    }
    else if(sort == VALUE){
        qsort(searchResults, *matchCount, sizeof(*searchResults), compOwnedValue);
    }
    else if(sort == RARITY){
        qsort(searchResults, *matchCount, sizeof(*searchResults), compOwnedRarity);
    }
    else if(sort == SET){
        qsort(searchResults, *matchCount, sizeof(*searchResults), compOwnedSet);
    }
    return searchResults;
}
