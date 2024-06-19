#include QMK_KEYBOARD_H
#include "midi.h"
#include "rgb_matrix.h"

static uint8_t midi_cc_state = 2;  // Initialize to the middle state
static uint8_t midi_cc_value[] = {0, 42, 84, 126, 127};
static uint8_t midi_cc_number = 1;

static uint8_t up_key_led_index = 0;    // Replace with the actual index
static uint8_t down_key_led_index = 1;  // Replace with the actual index

void midi_cc_up(MidiDevice midi_device) {
  if (midi_cc_state < 4) {
    midi_cc_state++;
  }
  uint8_t value = midi_cc_value[midi_cc_state];

  midi_send_cc(&midi_device, 0, midi_cc_number, value);

  switch (midi_cc_state) {
    case 0:
      rgb_matrix_set_color(up_key_led_index, 255, 0, 0);    // Up key: Red
      rgb_matrix_set_color(down_key_led_index, 0, 0, 0);  // Down key: Off
      break;
    case 1:
      rgb_matrix_set_color(up_key_led_index, 0, 255, 0);    // Up key: Green
      rgb_matrix_set_color(down_key_led_index, 0, 0, 0);  // Down key: Off
      break;
    case 2:
      rgb_matrix_set_color(up_key_led_index, 0, 0, 0);    // Up key: Off
      rgb_matrix_set_color(down_key_led_index, 0, 0, 0);  // Down key: Off
      break;
    case 3:
      rgb_matrix_set_color(up_key_led_index, 0, 0, 0);      // Up key: Off
      rgb_matrix_set_color(down_key_led_index, 0, 255, 0);  // Down key: Green
      break;
    case 4:
      rgb_matrix_set_color(up_key_led_index, 0, 0, 0);      // Up key: Off
      rgb_matrix_set_color(down_key_led_index, 255, 0, 0);  // Down key: Red
      break;
  }
}

void midi_cc_down(MidiDevice midi_device) {
  if (midi_cc_state > 0) {
    midi_cc_state--;
  }
  uint8_t value = midi_cc_value[midi_cc_state];

  midi_send_cc(&midi_device, 0, midi_cc_number, value);

  switch (midi_cc_state) {
    case 0:
      rgb_matrix_set_color(up_key_led_index, 255, 0, 0);    // Up key: Red
      rgb_matrix_set_color(down_key_led_index, 0, 0, 0);  // Down key: Off
      break;
    case 1:
      rgb_matrix_set_color(up_key_led_index, 0, 255, 0);    // Up key: Green
      rgb_matrix_set_color(down_key_led_index, 0, 0, 0);  // Down key: Off
      break;
    case 2:
      rgb_matrix_set_color(up_key_led_index, 0, 0, 0);    // Up key: Off
      rgb_matrix_set_color(down_key_led_index, 0, 0, 0);  // Down key: Off
      break;
    case 3:
      rgb_matrix_set_color(up_key_led_index, 0, 0, 0);      // Up key: Off
      rgb_matrix_set_color(down_key_led_index, 0, 255, 0);  // Down key: Green
      break;
    case 4:
      rgb_matrix_set_color(up_key_led_index, 0, 0, 0);      // Up key: Off
      rgb_matrix_set_color(down_key_led_index, 255, 0, 0);  // Down key: Red
      break;
  }
}