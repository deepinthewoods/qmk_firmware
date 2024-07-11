#include QMK_KEYBOARD_H
#include "midi.h"

// Define the MIDI channel to listen on (0-15)
#define MIDI_CHANNEL 0

// Number of keys to track
#define NUM_TRACKED_KEYS 60

// Array to store CC values for each key
uint8_t cc_values[NUM_TRACKED_KEYS] = {0};

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_5x15(
        KC_ESC,  MI_C1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_DEL,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_PGUP,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,  KC_ENT,  KC_PGDN,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,   KC_RSFT, KC_END,
        KC_LCTL, KC_LGUI, KC_LALT, KC_SPC,  KC_SPC,  KC_SPC,  KC_SPC,  KC_SPC,  KC_SPC,  KC_RALT, MO(1),   KC_LEFT, KC_DOWN, KC_RGHT, KC_RCTL
    ),
    [1] = LAYOUT_ortho_5x15(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,  KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_7,    KC_8,    KC_9,    KC_PMNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_4,    KC_5,    KC_6,    KC_PPLS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_1,    KC_2,    KC_3,    KC_PENT, KC_TRNS, KC_VOLU, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_0,    KC_0,    KC_DOT,  KC_TRNS, KC_MPRV, KC_VOLD, KC_MNXT, KC_MPLY
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return true;
}

void keyboard_post_init_user(void) {
    // Enable MIDI
    midi_init();
}

// Function to convert CC value to RGB color
RGB cc_value_to_color(uint8_t value) {
    uint8_t red = value * 2;
    uint8_t green = 255 - value * 2;
    uint8_t blue = 0;
    return (RGB){red, green, blue};
}

void midi_cc_user(uint8_t channel, uint8_t controller, uint8_t value) {
    // Check if the incoming MIDI message is on the channel we're listening to
    // if (channel == MIDI_CHANNEL) {
        // Check if the CC number is within our tracked range
        if (controller < NUM_TRACKED_KEYS) {
            // Store the CC value
            cc_values[controller] = value;
            
            // Set the LED color based on the CC value
            RGB color = cc_value_to_color(value);
            rgb_matrix_set_color(controller, color.r, color.g, color.b);
        }
    // }
}

void matrix_scan_user(void) {
    // Update all tracked LEDs based on stored CC values
    for (int i = 0; i < NUM_TRACKED_KEYS; i++) {
        RGB color = cc_value_to_color(cc_values[i]);
        rgb_matrix_set_color(i, color.r, color.g, color.b);
    }
}