/*
 * SCREEN CONTROLLER: Displays cards owned by the current user and removes cards.
 * Its matching screen design is res/layout/activity_vault.xml.
 */
package com.pokevault.app;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Button;

import java.util.List;
import java.util.Locale;

public class VaultActivity extends Activity {
    private PokeVaultData data;
    private Button filterButton;
    private final CardFilters filters = new CardFilters();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_vault);
        data = new PokeVaultData(this);
        filterButton = findViewById(R.id.vaultFilterButton);
        filterButton.setOnClickListener(view ->
            CardFilterDialog.show(this, filters, this::showVault));
        showVault();
        findViewById(R.id.vaultBackButton).setOnClickListener(view -> finish());
    }

    private void showVault() {
        List<Card> cards = data.filterCards(
            data.getVaultCards(), "", filters.getType(),
            filters.getMinimumHp(), filters.getMaximumHp(),
            filters.getCardNumber()
        );
        filterButton.setText(filters.isActive() ? "Filters On" : "Filters");
        TextView total = findViewById(R.id.vaultTotalText);
        total.setText("Cards: " + data.getTotalCardCount()
            + "   Value: " + String.format(Locale.US, "$%.2f", data.getVaultValue()));

        ListView list = findViewById(R.id.vaultList);
        list.setEmptyView(findViewById(R.id.vaultEmptyText));
        list.setAdapter(new CardAdapter(this, cards, data, true, this::showVault));
    }
}
