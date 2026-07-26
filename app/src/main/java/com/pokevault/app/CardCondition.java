package com.pokevault.app;

public enum CardCondition {
    DAMAGED("Damaged", 0.25),
    HEAVILY_PLAYED("Heavily Played", 0.40),
    MODERATELY_PLAYED("Moderately Played", 0.60),
    LIGHTLY_PLAYED("Lightly Played", 0.80),
    NEAR_MINT("Near Mint", 1.00);

    private final String displayName;
    private final double priceMultiplier;

    CardCondition(String displayName, double priceMultiplier) {
        this.displayName = displayName;
        this.priceMultiplier = priceMultiplier;
    }

    public String getDisplayName() {
        return displayName;
    }

    public double getPriceMultiplier() {
        return priceMultiplier;
    }

    public static CardCondition fromStoredValue(String value) {
        if (value != null) {
            try {
                return CardCondition.valueOf(value);
            } catch (IllegalArgumentException ignored) {
                // Old or damaged preference values fall back to Near Mint.
            }
        }
        return NEAR_MINT;
    }

    @Override
    public String toString() {
        return displayName;
    }
}
