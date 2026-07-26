/*
 * MODEL: Stores the basic information for the person using the app.
 * More profile fields can be added here later.
 */
package com.pokevault.app;

public class UserProfile {
    private final String username;

    public UserProfile(String username) {
        this.username = username;
    }

    public String getUsername() {
        return username;
    }
}
