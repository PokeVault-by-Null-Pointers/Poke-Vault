#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "Card.h"
#include "Data.h"
#include "Misc.h"
#include "Profile.h"

#define CARDS_FILE "data/cards.csv"
#define OWNED_FILE "data/owned_cards.csv"
#define USERS_FILE "data/users.csv"
#define LINE_SIZE 512
#define START_CAPACITY 8

static bool ensureDirectory(void){
    if(mkdir("data", 0755) == 0 || errno == EEXIST){
        return true;
    }
    return false;
}

static bool ensureFile(const char* path){
    FILE* file = fopen(path, "a");
    if(file == NULL){
        return false;
    }
    fclose(file);
    return true;
}

bool initializeDataFiles(void){
    if(!ensureDirectory()){
        return false;
    }
    return ensureFile(USERS_FILE) && ensureFile(OWNED_FILE);
}

static bool growCardList(CardArrayList* list){
    size_t newCapacity = list->capacity == 0 ? START_CAPACITY : list->capacity * 2;
    Card* newCards = realloc(list->cards, newCapacity * sizeof(Card));
    if(newCards == NULL){
        return false;
    }
    list->cards = newCards;
    list->capacity = newCapacity;
    return true;
}

static bool growOwnedCardList(OwnedCardArrayList* list){
    size_t newCapacity = list->capacity == 0 ? START_CAPACITY : list->capacity * 2;
    OwnedCard* newCards = realloc(list->cards, newCapacity * sizeof(OwnedCard));
    if(newCards == NULL){
        return false;
    }
    list->cards = newCards;
    list->capacity = newCapacity;
    return true;
}

bool loadCardCatalog(CardArrayList* list){
    FILE* file = fopen(CARDS_FILE, "r");
    char line[LINE_SIZE];
    char name[50];
    char set[100];
    double value;
    int rarity;
    int stage;
    int type;
    int hp;
    char cardNumber[CARD_NUMBER_SIZE];
    int fields;

    list->cards = NULL;
    list->size = 0;
    list->capacity = 0;

    if(file == NULL){
        return false;
    }

    while(fgets(line, sizeof(line), file) != NULL){
        if(line[0] == '#' || line[0] == '\n'){
            continue;
        }
        fields = sscanf(line, "%49[^,],%99[^,],%lf,%d,%d,%d,%d,%19[^\r\n]",
                        name, set, &value, &rarity, &stage, &type,
                        &hp, cardNumber);
        if(fields != 8){
            if(sscanf(line, "%49[^,],%99[^,],%lf,%d,%d,%d",
                      name, set, &value, &rarity, &stage, &type) != 6){
                continue;
            }
            hp = 0;
            copyString(cardNumber, "Unknown", CARD_NUMBER_SIZE);
        }
        if(list->size == list->capacity && !growCardList(list)){
            fclose(file);
            freeCardList(list);
            return false;
        }
        list->cards[list->size] = card_init(
            name, set, value, (int8_t)rarity, (int8_t)stage, (int8_t)type,
            hp, cardNumber
        );
        list->size++;
    }

    fclose(file);
    return true;
}

bool loadOwnedCards(const char* username, OwnedCardArrayList* list){
    FILE* file = fopen(OWNED_FILE, "r");
    char line[LINE_SIZE];
    char savedUsername[USERNAME_SIZE];
    OwnedCard card;
    int rarity;
    int stage;
    int type;
    int grade;
    int condition;
    int fields;

    list->cards = NULL;
    list->size = 0;
    list->capacity = 0;

    if(file == NULL){
        return false;
    }

    while(fgets(line, sizeof(line), file) != NULL){
        fields = sscanf(line,
            "%31[^,],%49[^,],%99[^,],%lf,%d,%d,%d,%d,%19[^,],%lf,%d,%d",
            savedUsername, card.name, card.set, &card.value,
            &rarity, &stage, &type, &card.hp, card.cardNumber,
            &card.purchasePrice, &grade, &condition);
        if(fields != 12){
            if(sscanf(line, "%31[^,],%49[^,],%99[^,],%lf,%d,%d,%d,%lf,%d",
                      savedUsername, card.name, card.set, &card.value,
                      &rarity, &stage, &type, &card.purchasePrice, &grade) != 9){
                continue;
            }
            card.hp = 0;
            copyString(card.cardNumber, "Unknown", CARD_NUMBER_SIZE);
            condition = 0;
        }
        if(strcmp(savedUsername, username) != 0){
            continue;
        }
        if(list->size == list->capacity && !growOwnedCardList(list)){
            fclose(file);
            freeOwnedCardList(list);
            return false;
        }
        card.rarity = (int8_t)rarity;
        card.stage = (int8_t)stage;
        card.type = (int8_t)type;
        card.grade = (int8_t)grade;
        card.condition = (int8_t)condition;
        list->cards[list->size] = card;
        list->size++;
    }

    fclose(file);
    return true;
}

bool appendOwnedCard(const char* username, const OwnedCard* card){
    FILE* file = fopen(OWNED_FILE, "a");
    if(file == NULL){
        return false;
    }

    fprintf(file, "%s,%s,%s,%.2f,%d,%d,%d,%d,%s,%.2f,%d,%d\n",
            username, card->name, card->set, card->value,
            card->rarity, card->stage, card->type,
            card->hp, card->cardNumber, card->purchasePrice,
            card->grade, card->condition);
    fclose(file);
    return true;
}

bool removeOwnedCardRecord(const char* username, size_t userCardIndex){
    FILE* input = fopen(OWNED_FILE, "r");
    FILE* output;
    char line[LINE_SIZE];
    char savedUsername[USERNAME_SIZE];
    size_t currentUserIndex = 0;
    bool removed = false;

    if(input == NULL){
        return false;
    }

    output = fopen("data/owned_cards.tmp", "w");
    if(output == NULL){
        fclose(input);
        return false;
    }

    while(fgets(line, sizeof(line), input) != NULL){
        if(sscanf(line, "%31[^,]", savedUsername) != 1){
            fputs(line, output);
            continue;
        }

        if(strcmp(savedUsername, username) == 0){
            if(currentUserIndex == userCardIndex){
                removed = true;
                currentUserIndex++;
                continue;
            }
            currentUserIndex++;
        }
        fputs(line, output);
    }

    fclose(input);
    fclose(output);

    if(rename("data/owned_cards.tmp", OWNED_FILE) != 0){
        return false;
    }
    return removed;
}

void freeCardList(CardArrayList* list){
    free(list->cards);
    list->cards = NULL;
    list->size = 0;
    list->capacity = 0;
}

void freeOwnedCardList(OwnedCardArrayList* list){
    free(list->cards);
    list->cards = NULL;
    list->size = 0;
    list->capacity = 0;
}
