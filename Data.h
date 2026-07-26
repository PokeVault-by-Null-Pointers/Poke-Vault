#ifndef DATA_H
#define DATA_H

#include <stdbool.h>
#include "Card.h"

bool initializeDataFiles(void);
bool loadCardCatalog(CardArrayList* list);
bool loadOwnedCards(const char* username, OwnedCardArrayList* list);
bool appendOwnedCard(const char* username, const OwnedCard* card);
bool removeOwnedCardRecord(const char* username, size_t userCardIndex);
void freeCardList(CardArrayList* list);
void freeOwnedCardList(OwnedCardArrayList* list);

#endif
