/*
 * UI HELPER: Converts Card objects into catalog and vault rows.
 */
package com.pokevault.app;

import android.content.Context;
import android.text.Editable;
import android.text.InputFilter;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Arrays;
import java.util.List;
import java.util.Locale;

public class CardAdapter extends BaseAdapter {
    private final Context context;
    private final LayoutInflater inflater;
    private final List<Card> cards;
    private final PokeVaultData data;
    private final boolean vaultMode;
    private final Runnable onVaultChanged;
    private final List<CardCondition> conditions = Arrays.asList(CardCondition.values());

    public CardAdapter(Context context, List<Card> cards, PokeVaultData data,
                       boolean vaultMode, Runnable onVaultChanged) {
        this.context = context;
        inflater = LayoutInflater.from(context);
        this.cards = cards;
        this.data = data;
        this.vaultMode = vaultMode;
        this.onVaultChanged = onVaultChanged;
    }

    @Override
    public int getCount() {
        return cards.size();
    }

    @Override
    public Card getItem(int position) {
        return cards.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View oldView, ViewGroup parent) {
        View row = oldView;
        if (row == null) {
            row = inflater.inflate(R.layout.row_card, parent, false);
        }

        Card card = getItem(position);
        ImageView image = row.findViewById(R.id.cardImage);
        TextView name = row.findViewById(R.id.cardNameText);
        TextView information = row.findViewById(R.id.cardInfoText);
        EditText quantity = row.findViewById(R.id.cardQuantityInput);
        Spinner condition = row.findViewById(R.id.cardConditionSpinner);
        Button minus = row.findViewById(R.id.cardMinusButton);
        Button plus = row.findViewById(R.id.cardPlusButton);
        Button remove = row.findViewById(R.id.cardActionButton);

        CardImageLoader.load(card.getImageUrl(), image);
        name.setText(card.getName());
        bindDetails(card, information, quantity);

        ArrayAdapter<CardCondition> conditionAdapter = new ArrayAdapter<>(
            context, android.R.layout.simple_spinner_item, conditions
        );
        conditionAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        condition.setOnItemSelectedListener(null);
        condition.setAdapter(conditionAdapter);
        condition.setSelection(conditions.indexOf(data.getCondition(card)), false);
        condition.setEnabled(!vaultMode);
        if (!vaultMode) {
            condition.setOnItemSelectedListener(new SimpleItemSelectedListener(selectedPosition -> {
                CardCondition selected = conditions.get(selectedPosition);
                if (selected != data.getCondition(card)) {
                    data.setCondition(card, selected);
                    notifyDataSetChanged();
                }
            }));
        }

        int currentQuantity = data.getQuantity(card, data.getCondition(card));
        minus.setEnabled(currentQuantity > 0);
        plus.setEnabled(currentQuantity < PokeVaultData.MAX_CARD_QUANTITY);

        Object previousWatcher = quantity.getTag();
        if (previousWatcher instanceof TextWatcher) {
            quantity.removeTextChangedListener((TextWatcher)previousWatcher);
        }
        quantity.setFilters(new InputFilter[] { new InputFilter.LengthFilter(3) });
        TextWatcher quantityWatcher = new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence text, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence text, int start, int before, int count) {
            }

            @Override
            public void afterTextChanged(Editable editable) {
                if (editable.length() == 0) {
                    return;
                }
                int requested = Integer.parseInt(editable.toString());
                data.setQuantity(card, requested);
                minus.setEnabled(requested > 0);
                plus.setEnabled(requested < PokeVaultData.MAX_CARD_QUANTITY);
            }
        };
        quantity.setTag(quantityWatcher);
        quantity.addTextChangedListener(quantityWatcher);
        quantity.setOnFocusChangeListener((view, hasFocus) -> {
            if (hasFocus) {
                return;
            }
            String entered = quantity.getText().toString();
            data.setQuantity(card, entered.isEmpty() ? 0 : Integer.parseInt(entered));
            if (vaultMode) {
                changed();
            }
        });
        minus.setOnClickListener(view -> {
            data.removeCard(card);
            Toast.makeText(context, "Removed one " + card.getName() + ".",
                Toast.LENGTH_SHORT).show();
            changed();
        });
        plus.setOnClickListener(view -> {
            if (data.getQuantity(card, data.getCondition(card))
                    >= PokeVaultData.MAX_CARD_QUANTITY) {
                Toast.makeText(context, "Card quantity is limited to 999.",
                    Toast.LENGTH_SHORT).show();
                return;
            }
            data.addCard(card);
            Toast.makeText(context, "Added one " + card.getName() + ".",
                Toast.LENGTH_SHORT).show();
            changed();
        });

        remove.setVisibility(vaultMode ? View.VISIBLE : View.GONE);
        remove.setText("Remove Card");
        remove.setOnClickListener(view -> {
            data.removeAll(card);
            Toast.makeText(context, card.getName() + " removed from your vault.",
                Toast.LENGTH_SHORT).show();
            changed();
        });
        return row;
    }

    private void bindDetails(Card card, TextView information, EditText quantity) {
        String hp = card.getHp() > 0 ? card.getHp() + " HP" : "No HP";
        String details = card.getSetName() + " • #" + card.getNumber()
            + " • " + card.getType() + " • " + hp
            + "\n" + card.getRarityName() + " • "
            + data.getCondition(card).getDisplayName() + " value: "
            + String.format(Locale.US, "$%.2f", data.getAdjustedValue(card));
        information.setText(details);
        quantity.setText(String.valueOf(
            data.getQuantity(card, data.getCondition(card))
        ));
    }

    private void changed() {
        if (vaultMode && onVaultChanged != null) {
            onVaultChanged.run();
        } else {
            notifyDataSetChanged();
        }
    }
}
