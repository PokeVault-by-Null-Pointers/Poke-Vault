#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <termios.h>
#include <unistd.h>
#include "Card.h"
#include "Data.h"
#include "Misc.h"
#include "Profile.h"
#include "Search.h"
#include "UI.h"

#define INPUT_SIZE 128

static const char* rarityNames[] = {
    "Common", "Uncommon", "Rare", "Holofoil Rare",
    "Double Rare", "Ultra Rare", "Secret Rare"
};

static const char* stageNames[] = {
    "Basic", "Stage 1", "Stage 2"
};

static const char* typeNames[] = {
    "Grass", "Fire", "Water", "Lightning", "Fighting", "Psychic",
    "Colorless", "Darkness", "Metal", "Dragon", "Fairy"
};

static const char* conditionNames[] = {
    "Near Mint", "Lightly Played", "Moderately Played",
    "Heavily Played", "Damaged"
};

static void printLine(void){
    printf("------------------------------------------------------------\n");
}

static void printHeader(const char* title){
    printf("\n");
    printLine();
    printf("                       P O K E V A U L T\n");
    printLine();
    printf("%s\n", title);
    printLine();
}

static int readLine(char* output, size_t size){
    int character;
    size_t length;

    if(fgets(output, (int)size, stdin) == NULL){
        return -1;
    }

    length = strlen(output);
    if(length > 0 && output[length - 1] == '\n'){
        output[length - 1] = '\0';
        return 1;
    }

    character = getchar();
    if(character == '\n' || character == EOF){
        return 1;
    }
    while((character = getchar()) != '\n' && character != EOF){
        /* Discard characters that did not fit in the input buffer. */
    }
    return 0;
}

static void closeOnInputEnd(void){
    printf("\n\nInput ended. Closing PokeVault.\n");
    exit(0);
}

static void readText(const char* prompt, char* output, size_t size){
    int result;

    while(true){
        printf("%s", prompt);
        result = readLine(output, size);
        if(result == 1){
            return;
        }
        if(result == -1){
            closeOnInputEnd();
        }
        printf("That entry is too long. Please use at most %zu characters.\n",
               size - 1);
    }
}

static void readPassword(const char* prompt, char* output, size_t size){
    struct termios originalSettings;
    struct termios hiddenSettings;
    bool hideInput = isatty(STDIN_FILENO) &&
                     tcgetattr(STDIN_FILENO, &originalSettings) == 0;
    int result;

    while(true){
        printf("%s", prompt);
        fflush(stdout);

        if(hideInput){
            hiddenSettings = originalSettings;
            hiddenSettings.c_lflag &= (tcflag_t)~ECHO;
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &hiddenSettings);
        }

        result = readLine(output, size);

        if(hideInput){
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalSettings);
            printf("\n");
        }

        if(result == 1){
            return;
        }
        if(result == -1){
            closeOnInputEnd();
        }
        printf("That password is too long. Please use at most %zu characters.\n",
               size - 1);
    }
}

static bool isBlank(const char* text){
    while(*text != '\0'){
        if(*text != ' ' && *text != '\t'){
            return false;
        }
        text++;
    }
    return true;
}

static int readInt(const char* prompt, int minimum, int maximum){
    char input[INPUT_SIZE];
    char* end;
    long value;

    while(true){
        readText(prompt, input, sizeof(input));
        value = strtol(input, &end, 10);
        if(input[0] != '\0' && *end == '\0' &&
           value >= minimum && value <= maximum){
            return (int)value;
        }
        printf("Please enter a number from %d to %d.\n", minimum, maximum);
    }
}

static double readMoney(const char* prompt){
    char input[INPUT_SIZE];
    char* end;
    double value;

    while(true){
        readText(prompt, input, sizeof(input));
        value = strtod(input, &end);
        if(input[0] != '\0' && *end == '\0' &&
           value >= 0.0 && value <= 1000000.0){
            return value;
        }
        printf("Please enter a price from 0.00 to 1000000.00.\n");
    }
}

static int readOptionalInt(const char* prompt, int minimum, int maximum,
                           int blankValue){
    char input[INPUT_SIZE];
    char* end;
    long value;

    while(true){
        readText(prompt, input, sizeof(input));
        if(input[0] == '\0'){
            return blankValue;
        }
        value = strtol(input, &end, 10);
        if(*end == '\0' && value >= minimum && value <= maximum){
            return (int)value;
        }
        printf("Enter a number from %d to %d, or leave it blank.\n",
               minimum, maximum);
    }
}

static void waitForEnter(void){
    char input[INPUT_SIZE];
    readText("\nPress Enter to continue...", input, sizeof(input));
}

static bool readYesNo(const char* prompt){
    char input[INPUT_SIZE];

    while(true){
        readText(prompt, input, sizeof(input));
        if(strcmp(input, "y") == 0 || strcmp(input, "Y") == 0 ||
           strcmp(input, "yes") == 0 || strcmp(input, "YES") == 0){
            return true;
        }
        if(strcmp(input, "n") == 0 || strcmp(input, "N") == 0 ||
           strcmp(input, "no") == 0 || strcmp(input, "NO") == 0){
            return false;
        }
        printf("Please enter yes or no.\n");
    }
}

static const char* safeRarity(int value){
    return value >= 0 && value <= 6 ? rarityNames[value] : "Unknown";
}

static const char* safeStage(int value){
    return value >= 0 && value <= 2 ? stageNames[value] : "Unknown";
}

static const char* safeType(int value){
    return value >= 0 && value <= 10 ? typeNames[value] : "Unknown";
}

static const char* safeCondition(int value){
    return value >= 0 && value <= 4 ? conditionNames[value] : "Unknown";
}

static void printCardRow(size_t number, const Card* card){
    printf("%2zu. %-22s | %-20s | $%8.2f\n",
           number, card->name, card->set, card->value);
    printf("    %-15s | %-8s | %s\n",
           safeRarity(card->rarity),
           safeStage(card->stage),
           safeType(card->type));
    printf("    HP %-4d | Card no. %s\n", card->hp, card->cardNumber);
}

static void printOwnedCardRow(size_t number, const OwnedCard* card){
    double change = card->value - card->purchasePrice;
    printf("%2zu. %-22s | %-20s | %s | Grade %d\n",
           number, card->name, card->set, safeCondition(card->condition),
           card->grade);
    printf("    Paid $%7.2f | Value $%7.2f | Change %+.2f\n",
           card->purchasePrice, card->value, change);
    printf("    %-10s | HP %-4d | Card no. %s\n",
           safeType(card->type), card->hp, card->cardNumber);
}

static FilterBounds readCardFilters(void){
    FilterBounds bounds = {
        .minPrice = 0.0, .maxPrice = DBL_MAX,
        .minValue = 0.0, .maxValue = DBL_MAX,
        .minRarity = 0, .maxRarity = MAX_RARITY,
        .minGrade = 1, .maxGrade = 10,
        .stage = -1, .type = -1,
        .minHp = 0, .maxHp = INT_MAX,
        .cardNumber = "",
        .owned = false, .unowned = false
    };
    int typeChoice;

    printf("Type:\n");
    printf(" 0. Any\n");
    for(int index = 0; index <= 10; index++){
        printf("%2d. %s\n", index + 1, typeNames[index]);
    }
    typeChoice = readInt("Choose a type: ", 0, 11);
    bounds.type = (int8_t)(typeChoice - 1);
    bounds.minHp = readOptionalInt("Minimum HP (blank for any): ",
                                   0, 1000, 0);
    bounds.maxHp = readOptionalInt("Maximum HP (blank for any): ",
                                   bounds.minHp, 1000, INT_MAX);
    readText("Card number (example 4/102; blank for any): ",
             bounds.cardNumber, sizeof(bounds.cardNumber));
    return bounds;
}

static bool createAccountScreen(char* loggedInUser){
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    char confirmPassword[PASSWORD_SIZE];

    printHeader("CREATE ACCOUNT");
    printf("Leave the username blank to return to the welcome menu.\n\n");
    readText("Choose a username: ", username, sizeof(username));

    if(username[0] == '\0'){
        return false;
    }
    if(isBlank(username) || strchr(username, ',') != NULL){
        printf("\nUse a visible username without commas.\n");
        waitForEnter();
        return false;
    }
    if(profileExists(username)){
        printf("\nAn account with that username already exists. Please log in.\n");
        waitForEnter();
        return false;
    }

    readPassword("Choose a password: ", password, sizeof(password));
    readPassword("Enter the password again: ", confirmPassword, sizeof(confirmPassword));

    if(strlen(password) < 4){
        printf("\nThe password must contain at least 4 characters.\n");
        waitForEnter();
        return false;
    }

    if(strcmp(password, confirmPassword) != 0){
        printf("\nThe passwords did not match.\n");
        waitForEnter();
        return false;
    }

    if(createProfile(username, password)){
        copyString(loggedInUser, username, USERNAME_SIZE);
        printf("\nAccount created. You are now logged in.\n");
        waitForEnter();
        return true;
    }

    printf("\nCould not create the account. Username and password are required,\n");
    printf("and the username cannot contain a comma.\n");
    waitForEnter();
    return false;
}

static bool loginScreen(char* loggedInUser){
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];

    printHeader("LOG IN");
    printf("Leave the username blank to return to the welcome menu.\n\n");
    readText("Username: ", username, sizeof(username));

    if(username[0] == '\0'){
        return false;
    }
    readPassword("Password: ", password, sizeof(password));

    if(loginProfile(username, password)){
        copyString(loggedInUser, username, USERNAME_SIZE);
        printf("\nWelcome back, %s.\n", loggedInUser);
        waitForEnter();
        return true;
    }

    printf("\nIncorrect username or password.\n");
    waitForEnter();
    return false;
}

static bool accountScreen(char* loggedInUser){
    int choice;

    while(true){
        printHeader("WELCOME");
        printf("1. Log in\n");
        printf("2. Create account\n");
        printf("3. Exit\n\n");
        choice = readInt("Choose an option: ", 1, 3);

        if(choice == 1 && loginScreen(loggedInUser)){
            return true;
        }
        if(choice == 2 && createAccountScreen(loggedInUser)){
            return true;
        }
        if(choice == 3){
            return false;
        }
    }
}

static void browseCatalogScreen(CardArrayList* catalog){
    int matchCount = 0;
    int* results;
    int choice;
    int index;

    printHeader("CARD CATALOG");
    if(catalog->size == 0){
        printf("No cards were loaded from data/cards.csv.\n");
        waitForEnter();
        return;
    }

    printf("0. Return to home\n");
    printf("1. Catalog order\n");
    printf("2. Name (A-Z)\n");
    printf("3. Name (Z-A)\n");
    printf("4. Value (low-high)\n");
    printf("5. Value (high-low)\n");
    printf("6. Rarity (low-high)\n\n");
    choice = readInt("Display order: ", 0, 6);
    if(choice == 0){
        return;
    }

    results = searchCards(catalog, "", &matchCount);
    if(choice == 2){
        sortAllCards(results, &matchCount, NAME, false);
    } else if(choice == 3){
        sortAllCards(results, &matchCount, NAME, true);
    } else if(choice == 4){
        sortAllCards(results, &matchCount, VALUE, false);
    } else if(choice == 5){
        sortAllCards(results, &matchCount, VALUE, true);
    } else if(choice == 6){
        sortAllCards(results, &matchCount, RARITY, false);
    }

    printf("\n");
    for(index = 0; index < matchCount; index++){
        printCardRow((size_t)results[index] + 1, &catalog->cards[results[index]]);
        printf("\n");
    }
    free(results);
    waitForEnter();
}

static void searchCatalogScreen(CardArrayList* catalog){
    char searchText[INPUT_SIZE];
    int matchCount = 0;
    int* results;
    int index;

    printHeader("SEARCH CATALOG");
    printf("Leave the search blank to return to the home menu.\n\n");
    readText("Card name: ", searchText, sizeof(searchText));
    if(searchText[0] == '\0'){
        return;
    }
    results = searchCards(catalog, searchText, &matchCount);

    printf("\n%d match%s found.\n\n", matchCount, matchCount == 1 ? "" : "es");
    for(index = 0; index < matchCount; index++){
        printCardRow((size_t)results[index] + 1, &catalog->cards[results[index]]);
        printf("\n");
    }
    free(results);
    waitForEnter();
}

static void filterCatalogScreen(CardArrayList* catalog){
    FilterBounds bounds;
    int allCount = 0;
    int filteredCount = 0;
    int* allResults;
    int* filteredResults;

    printHeader("FILTER CARD CATALOG");
    if(catalog->size == 0){
        printf("No cards were loaded from data/cards.csv.\n");
        waitForEnter();
        return;
    }

    bounds = readCardFilters();
    allResults = searchCards(catalog, "", &allCount);
    filteredResults = changeFilter(
        catalog, allResults, bounds, allCount, &filteredCount
    );
    free(allResults);

    printf("\n%d matching card%s found.\n\n",
           filteredCount, filteredCount == 1 ? "" : "s");
    for(int index = 0; index < filteredCount; index++){
        printCardRow((size_t)filteredResults[index] + 1,
                     &catalog->cards[filteredResults[index]]);
        printf("\n");
    }
    free(filteredResults);
    waitForEnter();
}

static void vaultScreen(const char* username){
    OwnedCardArrayList list;
    int matchCount = 0;
    int* results;
    int choice;
    int index;
    double totalValue = 0.0;
    double totalPaid = 0.0;

    printHeader("MY VAULT");
    if(!loadOwnedCards(username, &list)){
        printf("Could not load your collection.\n");
        waitForEnter();
        return;
    }

    if(list.size == 0){
        printf("Your vault is empty.\n");
        freeOwnedCardList(&list);
        waitForEnter();
        return;
    }

    printf("0. Return to home\n");
    printf("1. Date added\n");
    printf("2. Name (A-Z)\n");
    printf("3. Value (high-low)\n");
    printf("4. Purchase price (high-low)\n");
    printf("5. Rarity (high-low)\n\n");
    choice = readInt("Display order: ", 0, 5);
    if(choice == 0){
        freeOwnedCardList(&list);
        return;
    }

    results = searchOwnedCards(&list, "", &matchCount);
    if(choice == 2){
        sortOwnedCards(results, &matchCount, NAME, false);
    } else if(choice == 3){
        sortOwnedCards(results, &matchCount, VALUE, true);
    } else if(choice == 4){
        sortOwnedCards(results, &matchCount, PRICE, true);
    } else if(choice == 5){
        sortOwnedCards(results, &matchCount, RARITY, true);
    }

    printf("\n");
    for(index = 0; index < matchCount; index++){
        printOwnedCardRow((size_t)results[index] + 1, &list.cards[results[index]]);
        printf("\n");
        totalValue += list.cards[results[index]].value;
        totalPaid += list.cards[results[index]].purchasePrice;
    }

    if(matchCount > 0){
        printLine();
        printf("Cards: %d | Paid: $%.2f | Value: $%.2f | Change: %+.2f\n",
               matchCount, totalPaid, totalValue, totalValue - totalPaid);
    } else {
        printf("Your vault is empty.\n");
    }

    free(results);
    freeOwnedCardList(&list);
    waitForEnter();
}

static void filterVaultScreen(const char* username){
    OwnedCardArrayList list;
    FilterBounds bounds;
    int allCount = 0;
    int filteredCount = 0;
    int* allResults;
    int* filteredResults;

    printHeader("FILTER MY VAULT");
    if(!loadOwnedCards(username, &list)){
        printf("Could not load your collection.\n");
        waitForEnter();
        return;
    }
    if(list.size == 0){
        printf("Your vault is empty.\n");
        freeOwnedCardList(&list);
        waitForEnter();
        return;
    }

    bounds = readCardFilters();
    allResults = searchOwnedCards(&list, "", &allCount);
    filteredResults = changeOwnedCardFilter(
        &list, allResults, bounds, allCount, &filteredCount
    );
    free(allResults);

    printf("\n%d matching owned card%s found.\n\n",
           filteredCount, filteredCount == 1 ? "" : "s");
    for(int index = 0; index < filteredCount; index++){
        printOwnedCardRow((size_t)filteredResults[index] + 1,
                          &list.cards[filteredResults[index]]);
        printf("\n");
    }

    free(filteredResults);
    freeOwnedCardList(&list);
    waitForEnter();
}

static void searchVaultScreen(const char* username){
    OwnedCardArrayList list;
    char searchText[INPUT_SIZE];
    int matchCount = 0;
    int* results;
    int index;

    printHeader("SEARCH MY VAULT");
    if(!loadOwnedCards(username, &list)){
        printf("Could not load your collection.\n");
        waitForEnter();
        return;
    }
    if(list.size == 0){
        printf("Your vault is empty.\n");
        freeOwnedCardList(&list);
        waitForEnter();
        return;
    }

    printf("Leave the search blank to return to the home menu.\n\n");
    readText("Card name: ", searchText, sizeof(searchText));
    if(searchText[0] == '\0'){
        freeOwnedCardList(&list);
        return;
    }

    results = searchOwnedCards(&list, searchText, &matchCount);
    printf("\n%d match%s found.\n\n", matchCount, matchCount == 1 ? "" : "es");
    for(index = 0; index < matchCount; index++){
        printOwnedCardRow((size_t)results[index] + 1, &list.cards[results[index]]);
        printf("\n");
    }

    free(results);
    freeOwnedCardList(&list);
    waitForEnter();
}

static void addCardScreen(const char* username, CardArrayList* catalog){
    int choice;
    double purchasePrice;
    int grade;
    int condition;
    OwnedCard newCard;

    printHeader("ADD CARD TO VAULT");
    if(catalog->size == 0){
        printf("No cards are available to add. Check data/cards.csv.\n");
        waitForEnter();
        return;
    }

    printf(" 0. Return to home\n");
    for(size_t index = 0; index < catalog->size; index++){
        printf("%2zu. %-24s | %-20s | $%.2f\n",
               index + 1, catalog->cards[index].name,
               catalog->cards[index].set, catalog->cards[index].value);
    }

    choice = readInt("\nChoose a card number: ", 0, (int)catalog->size);
    if(choice == 0){
        return;
    }
    purchasePrice = readMoney("Purchase price: $");
    grade = readInt("Grade (1-10): ", 1, 10);
    printf("\nCondition:\n");
    for(int index = 0; index <= 4; index++){
        printf("%d. %s\n", index + 1, conditionNames[index]);
    }
    condition = readInt("Choose the card's condition: ", 1, 5) - 1;

    newCard = OwnedCard_init(
        &catalog->cards[choice - 1], "", purchasePrice, (int8_t)grade,
        (int8_t)condition
    );

    if(appendOwnedCard(username, &newCard)){
        printf("\n%s was added to your vault.\n", newCard.name);
    } else {
        printf("\nThe card could not be saved.\n");
    }
    waitForEnter();
}

static void removeCardScreen(const char* username){
    OwnedCardArrayList list;
    int choice;

    printHeader("REMOVE CARD");
    if(!loadOwnedCards(username, &list)){
        printf("Could not load your collection.\n");
        waitForEnter();
        return;
    }

    if(list.size == 0){
        printf("Your vault is empty.\n");
        freeOwnedCardList(&list);
        waitForEnter();
        return;
    }

    for(size_t index = 0; index < list.size; index++){
        printOwnedCardRow(index + 1, &list.cards[index]);
        printf("\n");
    }

    printf(" 0. Return to home\n");
    choice = readInt("Choose a card number to remove: ", 0, (int)list.size);
    if(choice == 0){
        freeOwnedCardList(&list);
        return;
    }
    printf("\nSelected: %s from %s\n",
           list.cards[choice - 1].name, list.cards[choice - 1].set);
    if(!readYesNo("Remove this card? (yes/no): ")){
        printf("\nRemoval canceled.\n");
        freeOwnedCardList(&list);
        waitForEnter();
        return;
    }

    if(removeOwnedCardRecord(username, (size_t)(choice - 1))){
        printf("\nCard removed.\n");
    } else {
        printf("\nThe card could not be removed.\n");
    }

    freeOwnedCardList(&list);
    waitForEnter();
}

static void profileScreen(const char* username){
    printHeader("PROFILE");
    printf("Username: %s\n", username);
    printf("Storage: local CSV files in the data folder\n");
    printf("Password: stored as a classroom prototype hash\n");
    waitForEnter();
}

static bool mainMenu(const char* username, CardArrayList* catalog){
    int choice;

    printHeader("HOME");
    printf("Logged in as: %s\n\n", username);
    printf("1. Browse card catalog\n");
    printf("2. Search cards\n");
    printf("3. Filter card catalog\n");
    printf("4. View my vault\n");
    printf("5. Search my vault\n");
    printf("6. Filter my vault\n");
    printf("7. Add a card\n");
    printf("8. Remove a card\n");
    printf("9. View profile\n");
    printf("10. Log out\n\n");

    choice = readInt("Choose an option: ", 1, 10);
    switch(choice){
        case 1: browseCatalogScreen(catalog); break;
        case 2: searchCatalogScreen(catalog); break;
        case 3: filterCatalogScreen(catalog); break;
        case 4: vaultScreen(username); break;
        case 5: searchVaultScreen(username); break;
        case 6: filterVaultScreen(username); break;
        case 7: addCardScreen(username, catalog); break;
        case 8: removeCardScreen(username); break;
        case 9: profileScreen(username); break;
        case 10: return false;
    }
    return true;
}

void runPokeVault(CardArrayList* catalog){
    char loggedInUser[USERNAME_SIZE];
    bool running = true;

    while(running){
        loggedInUser[0] = '\0';
        if(!accountScreen(loggedInUser)){
            running = false;
            continue;
        }

        while(mainMenu(loggedInUser, catalog)){
            /* The selected screen is handled by mainMenu. */
        }
    }

    printf("\nThank you for using PokeVault.\n");
}
