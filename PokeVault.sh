#!/bin/sh

# Compile every C file, then start PokeVault.
# This works from the project folder in Linux/VirtualBox.

gcc -std=c11 -Wall -Wextra -Wpedantic \
    -o pokevault \
    Driver.c Card.c Search.c Misc.c Profile.c Data.c UI.c

if [ $? -eq 0 ]; then
    ./pokevault
fi
