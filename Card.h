/*******************************************************************************
 * @file        Card.h
 * @brief       Defines Card structs and usage methods
 *
 * @author      John Rosenberger
 * @date        2026-07-15
 * @copyright   me
 *******************************************************************************/
#ifndef CARDS_H
#define CARDS_H

#define MAX_RARITY 6
#define CARD_NUMBER_SIZE 20
#include <stdint.h> //for int8_t
#include <stdbool.h> //for bool

typedef struct{
    char name[50]; //longest real card name is like 30 characters long
    char set[100]; //longest is 58... tracks the original set
    double value; //the market value for this card at grade 9
    int8_t rarity; //0 = common,   1 = uncommon,   2 = rare,   3 = holofoil rare,   4 = double rare,   5 = ultra rare,   6 = secret rare 
    int8_t stage; //0 = basic,   1 = stage 1,   2 = stage 2
    int8_t type; //0=grass, 1=fire, 2=water, 3=lightning, 4=fighting, 5=psychic, 6=colorless, 7=darkenss, 8=metal, 9=dragon, 10=fairy
    int hp;
    char cardNumber[CARD_NUMBER_SIZE];
    bool owned;
}Card;
typedef struct{
    char name[50]; //longest real card name is like 30 characters long
    char set[100]; //longest is 58... tracks the set where you got this particular card.
    double value; //the market value for this card at grade 9
    int8_t rarity; //0 = common,   1 = uncommon,   2 = rare,   3 = holofoil rare,   4 = double rare,   5 = ultra rare,   6 = secret rare 
    int8_t stage; //0 = basic,   1 = stage 1,   2 = stage 2
    int8_t type; //0=grass, 1=fire, 2=water, 3=lightning, 4=fighting, 5=psychic, 6=colorless, 7=darkenss, 8=metal, 9=dragon, 10=fairy
    int hp;
    char cardNumber[CARD_NUMBER_SIZE];
    double purchasePrice;
    int8_t grade; //1-10
    int8_t condition; //0=near mint, 1=lightly played, 2=moderately played, 3=heavily played, 4=damaged
}OwnedCard;

typedef struct {
    Card* cards;      // Pointer to the dynamically allocated array
    size_t size;    // Number of elements currently in the list
    size_t capacity; // Total allocated space
} CardArrayList;
typedef struct {
    OwnedCard* cards;      // Pointer to the dynamically allocated array
    size_t size;    // Number of elements currently in the list
    size_t capacity; // Total allocated space
} OwnedCardArrayList;

Card card_init(const char* name, const char* set, double value, int8_t rarity,
               int8_t stage, int8_t type, int hp, const char* cardNumber);
OwnedCard OwnedCard_init(Card* card, const char* set, double purchasePrice,
                         int8_t grade, int8_t condition);//if no set is given, pass an empty string and we'll assign it from the card

/*
String name 
String set  
Double current value (a generic value for a certain print of a card at a certain grade)
8 bit int rarity (https://bulbapedia.bulbagarden.net/wiki/Rarity) 
8 bit int evolution stage 
8 bit int typing 
bool owned
Double purchase price (specific) 
8 bit int grade (specific) 
*/

#endif
