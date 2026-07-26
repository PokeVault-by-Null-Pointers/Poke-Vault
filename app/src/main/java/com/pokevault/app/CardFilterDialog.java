package com.pokevault.app;

import android.app.AlertDialog;
import android.content.Context;
import android.text.InputType;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.Arrays;
import java.util.List;

public final class CardFilterDialog {
    private static final List<String> TYPES = Arrays.asList(
        "All", "Colorless", "Fighting", "Fire", "Grass",
        "Lightning", "Psychic", "Water", "Trainer", "Energy"
    );

    private CardFilterDialog() {
    }

    public static void show(Context context, CardFilters filters, Runnable onChanged) {
        int padding = (int)(20 * context.getResources().getDisplayMetrics().density);
        LinearLayout fields = new LinearLayout(context);
        fields.setOrientation(LinearLayout.VERTICAL);
        fields.setPadding(padding, padding / 2, padding, 0);

        Spinner type = new Spinner(context);
        ArrayAdapter<String> types = new ArrayAdapter<>(
            context, android.R.layout.simple_spinner_item, TYPES
        );
        types.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        type.setAdapter(types);
        type.setSelection(Math.max(0, TYPES.indexOf(filters.getType())));
        fields.addView(type, matchWidth());

        EditText minimumHp = numberField(context, "Minimum HP");
        setNumber(minimumHp, filters.getMinimumHp());
        fields.addView(minimumHp, matchWidth());

        EditText maximumHp = numberField(context, "Maximum HP");
        setNumber(maximumHp, filters.getMaximumHp());
        fields.addView(maximumHp, matchWidth());

        EditText cardNumber = new EditText(context);
        cardNumber.setHint("Card number (example: 4)");
        cardNumber.setSingleLine(true);
        cardNumber.setText(filters.getCardNumber());
        fields.addView(cardNumber, matchWidth());

        AlertDialog dialog = new AlertDialog.Builder(context)
            .setTitle("Filter Cards")
            .setView(fields)
            .setNegativeButton("Cancel", null)
            .setNeutralButton("Clear", null)
            .setPositiveButton("Apply", null)
            .create();

        dialog.setOnShowListener(ignored -> {
            dialog.getButton(AlertDialog.BUTTON_NEUTRAL).setOnClickListener(view -> {
                filters.clear();
                dialog.dismiss();
                onChanged.run();
            });
            dialog.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener(view -> {
                Integer minimum = parseNumber(minimumHp);
                Integer maximum = parseNumber(maximumHp);
                if (minimum != null && maximum != null && minimum > maximum) {
                    Toast.makeText(context, "Minimum HP cannot exceed maximum HP.",
                        Toast.LENGTH_SHORT).show();
                    return;
                }
                filters.setType((String)type.getSelectedItem());
                filters.setMinimumHp(minimum);
                filters.setMaximumHp(maximum);
                filters.setCardNumber(cardNumber.getText().toString().trim());
                dialog.dismiss();
                onChanged.run();
            });
        });
        dialog.show();
    }

    private static EditText numberField(Context context, String hint) {
        EditText field = new EditText(context);
        field.setHint(hint);
        field.setSingleLine(true);
        field.setInputType(InputType.TYPE_CLASS_NUMBER);
        return field;
    }

    private static Integer parseNumber(EditText field) {
        String value = field.getText().toString().trim();
        return value.isEmpty() ? null : Integer.valueOf(value);
    }

    private static void setNumber(EditText field, Integer value) {
        field.setText(value == null ? "" : String.valueOf(value));
    }

    private static LinearLayout.LayoutParams matchWidth() {
        return new LinearLayout.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT,
            ViewGroup.LayoutParams.WRAP_CONTENT
        );
    }
}
