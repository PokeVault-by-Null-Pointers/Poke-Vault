/*
 * DATA CONTROLLER: Reads the card CSV file and saves app data on the device.
 * Activities call this class instead of handling storage themselves.
 */
package com.pokevault.app;

import android.content.Context;
import android.content.SharedPreferences;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Set;

public class PokeVaultData {
    public static final int MAX_CARD_QUANTITY = 999;
    private static final String FILE_NAME = "pokevault_data";
    private static final String USERS_KEY = "users";
    private static final String CURRENT_USER_KEY = "current_user";

    private final Context context;
    private final SharedPreferences preferences;

    public PokeVaultData(Context context) {
        this.context = context.getApplicationContext();
        preferences = this.context.getSharedPreferences(FILE_NAME, Context.MODE_PRIVATE);
    }

    public boolean createUser(String username, String password) {
        if (username == null || username.trim().isEmpty()
                || password == null || password.isEmpty()) {
            return false;
        }
        username = username.trim();
        Set<String> users = new HashSet<>(preferences.getStringSet(USERS_KEY, new HashSet<String>()));
        if (users.contains(username)) {
            return false;
        }
        users.add(username);
        String passwordHash = PasswordStorage.hash(password);
        if (passwordHash == null) {
            return false;
        }
        preferences.edit()
            .putStringSet(USERS_KEY, users)
            .putString(passwordKey(username), passwordHash)
            .apply();
        return true;
    }

    public boolean login(String username, String password) {
        String savedPassword = preferences.getString(passwordKey(username), null);
        if (!PasswordStorage.verify(password, savedPassword)) {
            return false;
        }
        SharedPreferences.Editor editor = preferences.edit()
            .putString(CURRENT_USER_KEY, username);
        if (PasswordStorage.needsUpgrade(savedPassword)) {
            String upgraded = PasswordStorage.hash(password);
            if (upgraded != null) {
                editor.putString(passwordKey(username), upgraded);
            }
        }
        editor.apply();
        return true;
    }

    public void logout() {
        preferences.edit().remove(CURRENT_USER_KEY).apply();
    }

    public boolean deleteCurrentUser(String password) {
        String username = getCurrentUser().getUsername();
        if (username.isEmpty() || password == null
                || !PasswordStorage.verify(password,
                    preferences.getString(passwordKey(username), null))) {
            return false;
        }

        Set<String> users = new HashSet<>(
            preferences.getStringSet(USERS_KEY, new HashSet<String>())
        );
        users.remove(username);

        SharedPreferences.Editor editor = preferences.edit()
            .putStringSet(USERS_KEY, users)
            .remove(passwordKey(username))
            .remove(CURRENT_USER_KEY);

        for (Card card : loadCards()) {
            editor.remove(selectionKey(username, card));
            editor.remove("quantity_" + username + "_" + card.getName());
            editor.remove("condition_" + username + "_" + card.getName());
            for (CardCondition condition : CardCondition.values()) {
                editor.remove(quantityKey(username, card, condition));
            }
        }
        editor.apply();
        return true;
    }

    public UserProfile getCurrentUser() {
        String username = preferences.getString(CURRENT_USER_KEY, "");
        return new UserProfile(username);
    }

    public List<Card> loadCards() {
        List<Card> cards = new ArrayList<>();
        try {
            BufferedReader reader = new BufferedReader(
                new InputStreamReader(context.getResources().openRawResource(R.raw.cards))
            );
            String line;
            while ((line = reader.readLine()) != null) {
                if (line.trim().isEmpty() || line.startsWith("#")) {
                    continue;
                }
                String[] values = line.split(",", -1);
                if (values.length >= 8) {
                    cards.add(new Card(
                        values[0].trim(),
                        values[1].trim(),
                        values[2].trim(),
                        Double.parseDouble(values[3].trim()),
                        values[4].trim(),
                        values[5].trim(),
                        Integer.parseInt(values[6].trim()),
                        values[7].trim()
                    ));
                }
            }
            reader.close();
        } catch (Exception ignored) {
            // An empty list is shown if the classroom data file cannot be read.
        }
        return cards;
    }

    public List<Card> searchCards(String searchText) {
        return filterCards(loadCards(), searchText, "All", null, null, "");
    }

    public List<Card> filterCards(List<Card> source, String searchText,
                                  String type, Integer minimumHp,
                                  Integer maximumHp, String cardNumber) {
        List<Card> matches = new ArrayList<>();
        String wanted = searchText.toLowerCase(Locale.US).trim();
        String wantedNumber = cardNumber.trim();
        for (Card card : source) {
            boolean nameMatches = card.getName().toLowerCase(Locale.US).contains(wanted);
            boolean typeMatches = type.equals("All") || card.getType().equals(type);
            boolean minimumMatches = minimumHp == null || card.getHp() >= minimumHp;
            boolean maximumMatches = maximumHp == null || card.getHp() <= maximumHp;
            boolean numberMatches = wantedNumber.isEmpty()
                || card.getNumber().equalsIgnoreCase(wantedNumber);
            if (nameMatches && typeMatches && minimumMatches
                    && maximumMatches && numberMatches) {
                matches.add(card);
            }
        }
        return matches;
    }

    public void addCard(Card card) {
        CardCondition condition = effectiveCondition(card);
        String key = quantityKey(getCurrentUser().getUsername(), card, condition);
        int currentQuantity = preferences.getInt(key, 0);
        if (currentQuantity < MAX_CARD_QUANTITY) {
            preferences.edit().putInt(key, currentQuantity + 1).apply();
        }
    }

    public void setQuantity(Card card, int quantity) {
        CardCondition condition = effectiveCondition(card);
        String key = quantityKey(getCurrentUser().getUsername(), card, condition);
        int safeQuantity = Math.max(0, Math.min(MAX_CARD_QUANTITY, quantity));
        if (safeQuantity == 0) {
            preferences.edit().remove(key).apply();
        } else {
            preferences.edit().putInt(key, safeQuantity).apply();
        }
    }

    public void removeCard(Card card) {
        CardCondition condition = effectiveCondition(card);
        String key = quantityKey(getCurrentUser().getUsername(), card, condition);
        int currentQuantity = preferences.getInt(key, 0);
        if (currentQuantity <= 1) {
            preferences.edit().remove(key).apply();
        } else {
            preferences.edit().putInt(key, currentQuantity - 1).apply();
        }
    }

    public void removeAll(Card card) {
        preferences.edit().remove(
            quantityKey(getCurrentUser().getUsername(), card, effectiveCondition(card))
        ).apply();
    }

    public int getQuantity(Card card) {
        migrateLegacyQuantity(card);
        if (card.getCondition() != null) {
            return getQuantity(card, card.getCondition());
        }
        int total = 0;
        for (CardCondition condition : CardCondition.values()) {
            total += quantityForCondition(card, condition);
        }
        return total;
    }

    public int getQuantity(Card card, CardCondition condition) {
        migrateLegacyQuantity(card);
        return quantityForCondition(card, condition);
    }

    public List<Card> getVaultCards() {
        List<Card> ownedCards = new ArrayList<>();
        for (Card card : loadCards()) {
            migrateLegacyQuantity(card);
            for (CardCondition condition : CardCondition.values()) {
                if (quantityForCondition(card, condition) > 0) {
                    ownedCards.add(card.withCondition(condition));
                }
            }
        }
        return ownedCards;
    }

    public CardCondition getCondition(Card card) {
        if (card.getCondition() != null) {
            return card.getCondition();
        }
        String stored = preferences.getString(
            selectionKey(getCurrentUser().getUsername(), card),
            CardCondition.NEAR_MINT.name()
        );
        return CardCondition.fromStoredValue(stored);
    }

    public void setCondition(Card card, CardCondition condition) {
        preferences.edit().putString(
            selectionKey(getCurrentUser().getUsername(), card),
            condition.name()
        ).apply();
    }

    public double getAdjustedValue(Card card) {
        return card.getMarketValue() * getCondition(card).getPriceMultiplier();
    }

    public int getTotalCardCount() {
        int total = 0;
        for (Card card : getVaultCards()) {
            total += getQuantity(card);
        }
        return total;
    }

    public double getVaultValue() {
        double total = 0;
        for (Card card : getVaultCards()) {
            total += getAdjustedValue(card) * getQuantity(card);
        }
        return total;
    }

    private String passwordKey(String username) {
        return "password_" + username;
    }

    private CardCondition effectiveCondition(Card card) {
        return card.getCondition() == null ? getCondition(card) : card.getCondition();
    }

    private int quantityForCondition(Card card, CardCondition condition) {
        return preferences.getInt(
            quantityKey(getCurrentUser().getUsername(), card, condition), 0
        );
    }

    private void migrateLegacyQuantity(Card card) {
        String username = getCurrentUser().getUsername();
        String oldKey = "quantity_" + username + "_" + card.getName();
        if (!preferences.contains(oldKey)) {
            return;
        }
        int oldQuantity = Math.min(
            MAX_CARD_QUANTITY, Math.max(0, preferences.getInt(oldKey, 0))
        );
        CardCondition oldCondition = CardCondition.fromStoredValue(
            preferences.getString(
                "condition_" + username + "_" + card.getName(),
                CardCondition.NEAR_MINT.name()
            )
        );
        String newKey = quantityKey(username, card, oldCondition);
        int existing = preferences.getInt(newKey, 0);
        preferences.edit()
            .putInt(newKey, Math.min(MAX_CARD_QUANTITY, existing + oldQuantity))
            .remove(oldKey)
            .remove("condition_" + username + "_" + card.getName())
            .apply();
    }

    private String cardKey(Card card) {
        return card.getSetName() + "_" + card.getNumber();
    }

    private String quantityKey(String username, Card card, CardCondition condition) {
        return "quantity_v2_" + username + "_" + cardKey(card) + "_" + condition.name();
    }

    private String selectionKey(String username, Card card) {
        return "condition_selection_" + username + "_" + cardKey(card);
    }
}
