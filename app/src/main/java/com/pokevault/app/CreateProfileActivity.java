/*
 * SCREEN CONTROLLER: Checks the create-profile form and saves a new user.
 * Its matching screen design is res/layout/activity_create_profile.xml.
 */
package com.pokevault.app;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class CreateProfileActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_create_profile);

        findViewById(R.id.saveProfileButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                createProfile();
            }
        });
        findViewById(R.id.backToLoginButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                finish();
            }
        });
    }

    private void createProfile() {
        EditText usernameInput = findViewById(R.id.newUsernameInput);
        EditText passwordInput = findViewById(R.id.newPasswordInput);
        EditText confirmInput = findViewById(R.id.confirmPasswordInput);
        String username = usernameInput.getText().toString().trim();
        String password = passwordInput.getText().toString();
        String confirmation = confirmInput.getText().toString();

        if (username.isEmpty()) {
            usernameInput.setError("Create a username to save your profile.");
            usernameInput.requestFocus();
            return;
        }
        if (password.isEmpty()) {
            passwordInput.setError("Create a password for your account.");
            passwordInput.requestFocus();
            return;
        }
        if (password.length() < 4) {
            passwordInput.setError("Password must be at least 4 characters.");
            passwordInput.requestFocus();
            return;
        }
        if (confirmation.isEmpty()) {
            confirmInput.setError("Enter your password again.");
            confirmInput.requestFocus();
            return;
        }
        if (!password.equals(confirmation)) {
            confirmInput.setError("The passwords do not match.");
            confirmInput.requestFocus();
            return;
        }
        if (!new PokeVaultData(this).createUser(username, password)) {
            Toast.makeText(this, "That username already exists.", Toast.LENGTH_SHORT).show();
            return;
        }

        Intent login = new Intent(this, LoginActivity.class);
        login.putExtra(LoginActivity.PROFILE_CREATED_EXTRA, true);
        login.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        startActivity(login);
        finish();
    }
}
