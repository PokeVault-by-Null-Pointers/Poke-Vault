/*
 * SCREEN CONTROLLER: Displays collection totals and opens the other screens.
 * Its matching screen design is res/layout/activity_home.xml.
 */
package com.pokevault.app;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.util.Locale;

public class HomeActivity extends Activity {
    private PokeVaultData data;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);
        data = new PokeVaultData(this);

        findViewById(R.id.catalogButton).setOnClickListener(open(CatalogActivity.class));
        findViewById(R.id.vaultButton).setOnClickListener(open(VaultActivity.class));
        findViewById(R.id.profileButton).setOnClickListener(open(ProfileActivity.class));
        findViewById(R.id.logoutButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                data.logout();
                startActivity(new Intent(HomeActivity.this, LoginActivity.class));
                finish();
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        UserProfile user = data.getCurrentUser();
        TextView welcome = findViewById(R.id.welcomeText);
        TextView summary = findViewById(R.id.summaryText);
        welcome.setText("Welcome, " + user.getUsername() + "!");
        summary.setText("Cards owned: " + data.getTotalCardCount()
            + "\nVault value: " + String.format(Locale.US, "$%.2f", data.getVaultValue()));
    }

    private View.OnClickListener open(final Class<? extends Activity> screen) {
        return new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(HomeActivity.this, screen));
            }
        };
    }
}
