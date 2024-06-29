/* Copyright 2021 MT
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once


#define WS2812_DRIVER PIO_WS2812_NUM0 // Assuming you're using PIO instance 0
#define WS2812_PIN 28                // GPIO pin for WS2812 data

#define MIDI_BASIC
#define MIDI_ADVANCED
#define MIDI_TONE_KEYCODE_OCTAVES 2

#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 150
#define RGB_MATRIX_HUE_STEP 8
#define RGB_MATRIX_SAT_STEP 8
#define RGB_MATRIX_VAL_STEP 8
#define RGB_MATRIX_SPD_STEP 10

#define RP2040_DEVICE RP2040
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 1000U

#define PICO_SDK_PATH "/c/Users/niall/qmk_firmware/lib/pico-sdk" // Linux format
#define PICO_BOARD "pico" // or "pico_w" if using the wireless version
#define PICO_RP2040 // Should be uncommented
