package com.pokevault.app;

public class CardFilters {
    private String type = "All";
    private Integer minimumHp;
    private Integer maximumHp;
    private String cardNumber = "";

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public Integer getMinimumHp() {
        return minimumHp;
    }

    public void setMinimumHp(Integer minimumHp) {
        this.minimumHp = minimumHp;
    }

    public Integer getMaximumHp() {
        return maximumHp;
    }

    public void setMaximumHp(Integer maximumHp) {
        this.maximumHp = maximumHp;
    }

    public String getCardNumber() {
        return cardNumber;
    }

    public void setCardNumber(String cardNumber) {
        this.cardNumber = cardNumber;
    }

    public void clear() {
        type = "All";
        minimumHp = null;
        maximumHp = null;
        cardNumber = "";
    }

    public boolean isActive() {
        return !type.equals("All") || minimumHp != null
            || maximumHp != null || !cardNumber.isEmpty();
    }
}
