# PokeVault

PokeVault is now a basic Android app. It uses:

- Java for what each screen does
- XML for how each screen looks
- CSV for the starter Pokémon card list
- Android local storage for profiles and vault cards

The older C files are still in the project. They were not deleted or changed
into Java.

## Run the Android app

1. Open Android Studio.
2. Click **Open**.
3. Select the `Poke-Vault` folder.
4. Wait for the Gradle sync to finish.
5. Select an Android emulator at the top.
6. Click the green **Run** triangle.

The app starts on the login screen. Select **Create Profile** the first time.

## Very simple Android file map

| Folder or file | What it does |
|---|---|
| `app/src/main/java/com/pokevault/app` | Java behavior |
| `app/src/main/res/layout` | XML screen designs |
| `app/src/main/res/raw/cards.csv` | Card catalog used by Android |
| `app/src/main/AndroidManifest.xml` | List of app screens |
| `app/build.gradle` | Android project settings |

## Screens

| Java file | XML file | Job |
|---|---|---|
| `LoginActivity.java` | `activity_login.xml` | Log in |
| `CreateProfileActivity.java` | `activity_create_profile.xml` | Make a profile |
| `HomeActivity.java` | `activity_home.xml` | Main menu |
| `CatalogActivity.java` | `activity_catalog.xml` | Search and add cards |
| `VaultActivity.java` | `activity_vault.xml` | View and remove owned cards |
| `ProfileActivity.java` | `activity_profile.xml` | Show profile totals |

## Model and data classes

| Java file | Job |
|---|---|
| `Card.java` | Stores one card's name, set, value, and rarity |
| `UserProfile.java` | Stores the logged-in username |
| `PokeVaultData.java` | Loads cards and saves users/vault quantities |
| `CardAdapter.java` | Puts card information into each list row |

For this classroom prototype, everything stays on the Android device. There is
no online account system and no API yet. Password storage is only a simple
prototype and must not be used for a real public application.

## Older C terminal version

The C version still compiles separately:

```sh
./PokeVault.sh
```

Its CSV files are in the top-level `data` folder. The Android app has its own
copy of the catalog in `app/src/main/res/raw/cards.csv`.

The C catalog stores each card's HP and collector number. Each card added to a
user's vault is saved as its own row, including that copy's condition, so
duplicates with different conditions remain separate. The C home menu also
provides catalog and vault filters for type, HP range, and card number.
