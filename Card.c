/*******************************************************************************
 * @file        Card.c
 * @brief       Defines Card structs and usage methods
 *
 * @author      John Rosenberger
 * @date        2026-07-15
 * @copyright   me
 *******************************************************************************/

#include <stdint.h> //for int8_t
#include <stdbool.h> //for bool
#include <string.h> //for strdup
#include "Misc.h" //for copyString
#include "Card.h" //needs this to be used

//initialize a new Card. Card is defined in Card.h
Card card_init(const char* name, const char* set, double value, int8_t rarity,
               int8_t stage, int8_t type, int hp, const char* cardNumber){
    Card card;
    copyString(card.name, name, 50);//defined in Misc.c
    copyString(card.set, set, 100);//defined in Misc.c
    card.value = value;
    card.rarity = rarity;
    card.stage = stage;
    card.type = type;
    card.hp = hp;
    copyString(card.cardNumber, cardNumber, CARD_NUMBER_SIZE);
    card.owned = false;

    return card;
}

//initialize a new OwnedCard. OwnedCard is defined in Card.h
OwnedCard OwnedCard_init(Card* card, const char* set, double purchasePrice,
                         int8_t grade, int8_t condition){
    OwnedCard newCard;

    copyString(newCard.name, card->name, 50);//defined in Misc.c
    //if no set is given, pass an empty string and we'll assign it from the card
    if(set[0] == '\0'){
        copyString(newCard.set, card->set, 100);//defined in Misc.c
    } else{
        copyString(newCard.set, set, 100);//defined in Misc.c
    }
    newCard.value = (*card).value;
    newCard.rarity = (*card).rarity;
    newCard.stage = (*card).stage;
    newCard.type = (*card).type;
    newCard.hp = (*card).hp;
    copyString(newCard.cardNumber, card->cardNumber, CARD_NUMBER_SIZE);
    newCard.purchasePrice = purchasePrice;
    newCard.grade = grade;
    newCard.condition = condition;

    //set the generic card's owned field to true
    card->owned = true;

    return newCard;
}

/*
String name 
String set  
Double current value (a generic value for a certain print of a card at a certain grade)
int rarity (https://bulbapedia.bulbagarden.net/wiki/Rarity) 
8 bit int evolution stage 
8 bit int typing 
bool owned
Double purchase price (specific) 
8 bit int grade (specific) 
*/
