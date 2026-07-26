package com.pokevault.app;

import android.view.View;
import android.widget.AdapterView;

public class SimpleItemSelectedListener implements AdapterView.OnItemSelectedListener {
    public interface SelectionAction {
        void run(int position);
    }

    private final SelectionAction action;

    public SimpleItemSelectedListener(SelectionAction action) {
        this.action = action;
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        action.run(position);
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {
    }
}
