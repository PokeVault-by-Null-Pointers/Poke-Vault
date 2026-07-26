#include <stdio.h>
#include <string.h>
#include "Profile.h"

#define USERS_FILE "data/users.csv"

/*
 * Small FNV-1a hash.
 *
 * This prevents plain-text passwords from appearing in users.csv, but it is
 * only for a local class prototype. A production application must use a
 * password library such as Argon2 or bcrypt.
 */
static unsigned long long hashPassword(const char* username, const char* password){
    const unsigned long long offset = 1469598103934665603ULL;
    const unsigned long long prime = 1099511628211ULL;
    unsigned long long hash = offset;
    const unsigned char* current;

    for(current = (const unsigned char*)username; *current != '\0'; current++){
        hash ^= *current;
        hash *= prime;
    }
    hash ^= ':';
    hash *= prime;
    for(current = (const unsigned char*)password; *current != '\0'; current++){
        hash ^= *current;
        hash *= prime;
    }
    return hash;
}

bool profileExists(const char* username){
    FILE* file = fopen(USERS_FILE, "r");
    char line[128];
    char savedUsername[USERNAME_SIZE];
    unsigned long long savedHash;

    if(file == NULL){
        return false;
    }

    while(fgets(line, sizeof(line), file) != NULL){
        if(line[0] == '#' ||
           sscanf(line, "%31[^,],%llu", savedUsername, &savedHash) != 2){
            continue;
        }
        if(strcmp(savedUsername, username) == 0){
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

bool createProfile(const char* username, const char* password){
    FILE* file;
    unsigned long long passwordHash;

    if(username == NULL || password == NULL ||
       username[0] == '\0' || password[0] == '\0' ||
       strchr(username, ',') != NULL){
        return false;
    }

    if(profileExists(username)){
        return false;
    }

    file = fopen(USERS_FILE, "a");
    if(file == NULL){
        return false;
    }

    passwordHash = hashPassword(username, password);
    fprintf(file, "%s,%llu\n", username, passwordHash);
    fclose(file);
    return true;
}

bool loginProfile(const char* username, const char* password){
    FILE* file = fopen(USERS_FILE, "r");
    char line[128];
    char savedUsername[USERNAME_SIZE];
    unsigned long long savedHash;
    unsigned long long enteredHash;

    if(file == NULL){
        return false;
    }

    enteredHash = hashPassword(username, password);
    while(fgets(line, sizeof(line), file) != NULL){
        if(line[0] == '#' ||
           sscanf(line, "%31[^,],%llu", savedUsername, &savedHash) != 2){
            continue;
        }
        if(strcmp(savedUsername, username) == 0 && savedHash == enteredHash){
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}
