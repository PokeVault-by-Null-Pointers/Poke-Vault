/*******************************************************************************
 * @file        Driver.c
 * @brief       Starts the PokeVault C application
 *******************************************************************************/

#include <stdio.h>
#include "Card.h"
#include "Data.h"
#include "UI.h"

int main(void){
    CardArrayList catalog;

    if(!initializeDataFiles()){
        fprintf(stderr, "Error: could not create the data files.\n");
        return 1;
    }

    if(!loadCardCatalog(&catalog)){
        fprintf(stderr, "Error: could not load data/cards.csv.\n");
        return 1;
    }

    runPokeVault(&catalog);
    freeCardList(&catalog);
    return 0;
}
