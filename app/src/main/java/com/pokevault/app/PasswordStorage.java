package com.pokevault.app;

import android.util.Base64;

import java.security.MessageDigest;
import java.security.SecureRandom;

import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;

final class PasswordStorage {
    private static final String PREFIX = "pbkdf2_sha256";
    private static final int ITERATIONS = 120000;
    private static final int SALT_BYTES = 16;
    private static final int KEY_BITS = 256;

    private PasswordStorage() {
    }

    static String hash(String password) {
        try {
            byte[] salt = new byte[SALT_BYTES];
            new SecureRandom().nextBytes(salt);
            byte[] derived = derive(password, salt, ITERATIONS);
            return PREFIX + "$" + ITERATIONS + "$"
                + Base64.encodeToString(salt, Base64.NO_WRAP) + "$"
                + Base64.encodeToString(derived, Base64.NO_WRAP);
        } catch (Exception ignored) {
            return null;
        }
    }

    static boolean verify(String password, String stored) {
        if (password == null || stored == null) {
            return false;
        }
        if (!stored.startsWith(PREFIX + "$")) {
            return stored.equals(password);
        }
        try {
            String[] parts = stored.split("\\$", -1);
            if (parts.length != 4) {
                return false;
            }
            int iterations = Integer.parseInt(parts[1]);
            byte[] salt = Base64.decode(parts[2], Base64.NO_WRAP);
            byte[] expected = Base64.decode(parts[3], Base64.NO_WRAP);
            byte[] actual = derive(password, salt, iterations);
            return MessageDigest.isEqual(expected, actual);
        } catch (Exception ignored) {
            return false;
        }
    }

    static boolean needsUpgrade(String stored) {
        return stored != null && !stored.startsWith(PREFIX + "$");
    }

    private static byte[] derive(String password, byte[] salt, int iterations)
            throws Exception {
        PBEKeySpec specification = new PBEKeySpec(
            password.toCharArray(), salt, iterations, KEY_BITS
        );
        try {
            return SecretKeyFactory.getInstance("PBKDF2WithHmacSHA256")
                .generateSecret(specification)
                .getEncoded();
        } finally {
            specification.clearPassword();
        }
    }
}
