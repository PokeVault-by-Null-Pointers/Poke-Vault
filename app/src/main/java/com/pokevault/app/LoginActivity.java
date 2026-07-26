/*
 * SCREEN CONTROLLER: Handles the login screen.
 * Its matching screen design is res/layout/activity_login.xml.
 */
package com.pokevault.app;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class LoginActivity extends Activity {
    public static final String PROFILE_CREATED_EXTRA = "profile_created";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        showProfileCreatedMessage();

        findViewById(R.id.loginButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                logIn();
            }
        });

        findViewById(R.id.createButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(LoginActivity.this, CreateProfileActivity.class));
            }
        });
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        setIntent(intent);
        showProfileCreatedMessage();
    }

    private void showProfileCreatedMessage() {
        TextView message = findViewById(R.id.loginMessageText);
        boolean profileCreated = getIntent().getBooleanExtra(PROFILE_CREATED_EXTRA, false);
        message.setVisibility(profileCreated ? View.VISIBLE : View.GONE);
        if (profileCreated) {
            message.setText("Profile created successfully. Please log in.");
            getIntent().removeExtra(PROFILE_CREATED_EXTRA);
        }
    }

    private void logIn() {
        EditText usernameInput = findViewById(R.id.usernameInput);
        EditText passwordInput = findViewById(R.id.passwordInput);
        String username = usernameInput.getText().toString().trim();
        String password = passwordInput.getText().toString();

        PokeVaultData data = new PokeVaultData(this);
        if (data.login(username, password)) {
            startActivity(new Intent(this, HomeActivity.class));
            finish();
        } else {
            Toast.makeText(this, "Username or password is incorrect.", Toast.LENGTH_SHORT).show();
        }
    }
}
