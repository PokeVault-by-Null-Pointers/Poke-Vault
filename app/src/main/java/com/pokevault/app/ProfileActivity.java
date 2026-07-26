/*
 * SCREEN CONTROLLER: Displays the current username and collection totals.
 * Its matching screen design is res/layout/activity_profile.xml.
 */
package com.pokevault.app;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.text.InputType;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Locale;

public class ProfileActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile);

        PokeVaultData data = new PokeVaultData(this);
        UserProfile user = data.getCurrentUser();
        TextView details = findViewById(R.id.profileDetailsText);
        details.setText("Username: " + user.getUsername()
            + "\n\nCards owned: " + data.getTotalCardCount()
            + "\n\nVault value: " + String.format(Locale.US, "$%.2f", data.getVaultValue()));

        findViewById(R.id.deleteAccountButton).setOnClickListener(view ->
            showDeleteAccountDialog(data));
        findViewById(R.id.profileBackButton).setOnClickListener(view -> finish());
    }

    private void showDeleteAccountDialog(PokeVaultData data) {
        EditText password = new EditText(this);
        int padding = (int)(20 * getResources().getDisplayMetrics().density);
        password.setHint("Enter your password");
        password.setInputType(
            InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD
        );
        password.setSingleLine(true);
        password.setPadding(padding, 0, padding, 0);

        AlertDialog dialog = new AlertDialog.Builder(this)
            .setTitle("Delete Account?")
            .setMessage("This permanently deletes your profile and every card in your vault.")
            .setView(password)
            .setNegativeButton("Cancel", null)
            .setPositiveButton("Delete", null)
            .create();

        dialog.setOnShowListener(ignored ->
            dialog.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener(view -> {
                if (!data.deleteCurrentUser(password.getText().toString())) {
                    password.setError("Incorrect password.");
                    password.requestFocus();
                    return;
                }
                dialog.dismiss();
                Toast.makeText(this, "Your account was deleted.", Toast.LENGTH_SHORT).show();
                Intent login = new Intent(this, LoginActivity.class);
                login.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
                startActivity(login);
            })
        );
        dialog.show();
    }
}
