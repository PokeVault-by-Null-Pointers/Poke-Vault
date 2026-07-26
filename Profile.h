#ifndef PROFILE_H
#define PROFILE_H

#include <stdbool.h>

#define USERNAME_SIZE 32
#define PASSWORD_SIZE 64

/*
 * A Profile is the model for one local PokeVault user.
 *
 * passwordHash is used instead of saving the password itself.
 * This classroom hash is not strong enough for a real public website.
 */
typedef struct {
    char username[USERNAME_SIZE];
    unsigned long long passwordHash;
} Profile;

bool createProfile(const char* username, const char* password);
bool loginProfile(const char* username, const char* password);
bool profileExists(const char* username);

#endif
