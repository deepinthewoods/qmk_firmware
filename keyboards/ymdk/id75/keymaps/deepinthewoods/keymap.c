#include QMK_KEYBOARD_H
#include "custom_actions.c"
#include "quantum.h"
#include "midi.h"
#include "rgb_matrix.h"

#define MIDI_CC_TOGGLE_1 20
#define MIDI_CC_TOGGLE_2 21
#define MIDI_CC_TOGGLE_3 22
#define MIDI_CC_TOGGLE_4 23
#define MIDI_CC_TOGGLE_5 24
#define MIDI_CC_TOGGLE_6 25
#define MIDI_CC_TOGGLE_7 26
#define MIDI_CC_TOGGLE_8 27

#define MIDI_CC_KEY 30

#define NUM_MIDI_KEYS 65

MidiDevice midi_device;

bool toggle_states[8] = {false, false, false, false, false, false, false, false};

uint8_t relativeNotes[MATRIX_ROWS][MATRIX_COLS]  = {0};

uint8_t key_signature = 0;

bool enabledSus4 = false, enabledSus2 = false, enabledGreen = false;

const uint8_t major_scale[] = {0, 2, 4, 5, 7, 9, 11};

enum custom_keycodes {
    MIDI_CC_UP = SAFE_RANGE,
    MIDI_CC_DOWN,
	TOGGLE_1,
    TOGGLE_2,
    TOGGLE_3,
	TOGGLE_4,
	TOGGLE_5,
	SUS2_ENABLE,
	SUS4_ENABLE,
	RISKY_CHORDS_ENABLE
	
};

const uint8_t IIIm7b5 = 8,
		sIdim7 = 9,
		VI = 10,
		sIVm7b5 = 11,
		VII = 12,
		sIIdim7 = 13,
		Vm = 14,
		I = 15,
		//16
		Im6 = 17,
		vo2 = 18,
		II = 19,
		bVI = 20,
		bVII = 21,
		VIm7b5ob3 = 22,
		sVdim7 = 23,
		III = 24,
		VIIm7b5 = 25,
		//sIdim7 = 26,
		//VI = 99,
		Idimob3 = 27,
		bVI7 = 28,
		bVII9 = 29,
		//sIVm7b5 = 30,
		Io5 = 31,
		IVm7 = 32,
		bII7 = 33;


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
        [0] = LAYOUT_ortho_5x15(RISKY_CHORDS_ENABLE, TOGGLE_1, MI_B2, MI_As2, MI_A2, MI_Gs2, MI_G2, MI_Fs2, MI_F2, MI_E2, MI_Ds2, MI_D2, MI_Cs2, MI_C2, MI_B1,
								SUS2_ENABLE, TOGGLE_2, MI_E3, MI_Ds3, MI_D3, MI_Cs3, MI_C3, MI_B2, MI_As2, MI_A2, MI_Gs2, MI_G2, MI_Fs2, MI_F2, MI_E2,
								SUS4_ENABLE, TOGGLE_3, MI_A3, MI_Gs3, MI_G3, MI_Fs3, MI_F3, MI_E3, MI_Ds3, MI_D3, MI_Cs3, MI_C3, MI_B2, MI_As2, MI_A2,
								TOGGLE_4, MIDI_CC_UP, MI_D4, MI_Cs4, MI_C4, MI_B3, MI_As3, MI_A3, MI_Gs3, MI_G3, MI_Fs3, MI_F3, MI_E3, MI_Ds3, MI_D3,
								TOGGLE_5, MIDI_CC_DOWN, MI_G4, MI_Fs4, MI_F4, MI_E4, MI_Ds4, MI_D4, MI_Cs4, MI_C4, MI_B3, MI_As3, MI_A3, MI_Gs3, MI_G3),
								
        [1] = LAYOUT_ortho_5x15(
			KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
		 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RGB_MOD,
		  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
		  , QK_BOOT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS)
};

// uint8_t[][] nextChords = {//circles one
// 		{}, 
// 		{2,3,4,5,6,7},//1
// 		{4, 5, 7},//2
// 		{6, 5, 2, 4, 5},//3
// 		{2, 5, 7},//4
// 		{1, 7},//5
// 		{3, 4, 2, 4, 5},//6
// 		{1, 5}//7
// }

// uint8_t[][] nextChords2 = {//simple
// 		{}, 
// 		{2,3,4,5,6},//1
// 		{3, 5},//2
// 		{4, 6},//3
// 		{2, 5},//4
// 		{1, 6, 3},//5
// 		{2, 4},//6
// 		{}//7
// }

uint8_t nextChordsSafe[][7] = {
	{},//0
		{2,3,4,5,6,7},//1
			{3,5, IVm7, bII7, Io5},//2
				{1,4,6},//3
					{2,5,Io5,1},//4
						{1},//5
							{2, 4},//6
								{}//7
	
};



uint8_t nextChordsRisky[][5] = {
	{},//0
		{},//1
		{},//2
		{},//3
		{},//4
		{},//5
		{},//6
		{},//7
		{VI, 4},//IIIm7b5 = 8
		{2},//sIdim7 = 9
		{2},//VI = 10,
		{VII, Io5, 5}, //sIVm7b5 = 11,
		{3},//		VII = 12,
		{3},//		sIIdim7 = 13,
		{I},//Vm = 14,
		{4},//I = 15,
		{},//16,
		{II, vo2},//Im6 = 17,
		{II},//vo2 = 18,
		{5},//II = 19,
		{bVII},//bVI = 20,
		{1},//bVII = 21,
		{II},//VIm7b5ob3 = 22,
		{6},//sVdim7 = 23,
		{6},//III = 24,
		{III},//VIIm7b5 = 25,
		{},// 26,
		{2},//Idimob3 = 27,
		{Io5},//bVI7 = 28,
		{Io5},//bVII9 = 29,
		{},//{io5},//sIVm7b5 = 30,
		{2, 3, 4, 5, 6},//io5 = 31,
		{1},//IVm7 = 32,
		{1}//bII7 = 33;
		
};




#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {

};
#endif // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)


// Define colors as 3-element arrays
#define IN_SCALE    {255, 0,   0}
#define NOT_IN_SCALE  {0,   255, 0}
#define CURRENTLY_PLAYED   {0,   0,   255}
#define FEEDBACK_PLAYED  {255, 255, 255}
#define CHORD_GREEN  {0,   0,   0}
#define CHORD_BLUE {0, 0, 255}

// Enum for color indices
enum led_colors {
    COLOR_IN_SCALE,
    COLOR_NOT_IN_SCALE,
    COLOR_CURRENTLY_PLAYED,
    COLOR_FEEDBACK_PLAYED,
    COLOR_CHORD_RISKY,
	COLOR_CHORD_SAFE,
    // Add more colors as needed
    NUM_LED_COLORS // This will automatically be the count of colors
};

// Create an array of color arrays
const uint8_t PROGMEM LED_COLORS[][3] = {
    IN_SCALE   ,
 	NOT_IN_SCALE ,
 	CURRENTLY_PLAYED ,
 	FEEDBACK_PLAYED ,
 	CHORD_GREEN ,
 	CHORD_BLUE ,
};

// Function to set color from the array
void set_led(uint8_t led_index, enum led_colors color) {
	
    rgb_matrix_set_color(led_index, 
                         pgm_read_byte(&LED_COLORS[color][0]),
                         pgm_read_byte(&LED_COLORS[color][1]),
                         pgm_read_byte(&LED_COLORS[color][2]));
}

void set_xy_led(uint8_t y, uint8_t x, enum led_colors color) {
	uint8_t led_index = g_led_config.matrix_co[y][x];
	set_led(led_index, color);
}


void setKeyLEDs(uint8_t keySignature){
	key_signature = keySignature;
	for (int y = 0; y < 5; y++){
		for (int x = 2; x < 15; x++){
			uint8_t keycode = pgm_read_word(&keymaps[0][y][x]);
					
			// Calculate the root note based on the key signature
			uint8_t root_note = key_signature;

			// Calculate the note value within the octave (0-11)
			uint8_t note_value = keycode % 12;

			// Adjust the note value based on the root note
			uint8_t adjusted_note = (note_value - root_note + 12) % 12;
			relativeNotes[y][x] = adjusted_note;
		}
	}
}

bool is_in_scale(uint8_t adjusted_note) {
    // Define the intervals of the major scale
    // Check if the adjusted note value exists in the major scale intervals
    for (uint8_t i = 0; i < sizeof(major_scale); i++) {
        if (adjusted_note == major_scale[i]) {
            return true;
        }
    }

    return false;
}

void redrawLEDs(void){
	for (int y = 0; y < 5; y++){
		for (int x = 2; x < 15; x++){
			uint8_t keycode = relativeNotes[y][x];
			uint8_t led_index = g_led_config.matrix_co[y][x]; // Get the LED index for the current row and column
            if (is_in_scale(keycode)){
                rgb_matrix_set_color(led_index, 0, 0, 255); // Set color to blue (RGB values: 0, 0, 255)
            } else {
                rgb_matrix_set_color(led_index, 0, 0, 0); // Set color to off (RGB values: 0, 0, 0)
            }
		}
	}
}

void redrawLED(int y, int x){
			uint8_t keycode = relativeNotes[y][x];
			uint8_t led_index = g_led_config.matrix_co[y][x]; // Get the LED index for the current row and column
            if (is_in_scale(keycode)){
                rgb_matrix_set_color(led_index, 0, 0, 255); // Set color to blue (RGB values: 0, 0, 255)
            } else {
                rgb_matrix_set_color(led_index, 0, 0, 0); // Set color to off (RGB values: 0, 0, 0)
            }
}

void midi_receive_cc(MidiDevice* device, uint8_t channel, uint8_t control, uint8_t value) {
    switch (control) {
        case MIDI_CC_KEY:
            setKeyLEDs(value);
			redrawLEDs();
            break;        
    }
}

void matrix_init_user(void) {
    midi_device_init(&midi_device);
    midi_register_cc_callback(&midi_device, midi_receive_cc);
}

#define MAX_NOTES_PER_CHORD 7

struct ChordState {
    uint8_t notes[MAX_NOTES_PER_CHORD];
    uint8_t note_count;
};

struct ChordState chord_states[NUM_MIDI_KEYS];

bool process_midi_messages(uint16_t keycode, keyrecord_t *record) {
    if (keycode >= MI_B1 && keycode <= MI_A4) {
        uint8_t chord_index = keycode - MI_B1;
        struct ChordState *chord = &chord_states[chord_index];

        if (record->event.pressed) {
            // Clear previous chord state
            chord->note_count = 0;

            // Add base note
            uint8_t base_note = keycode - MI_Cs3 + 60;
            uint8_t scale_degree = (base_note - key_signature + 12) % 12;
            uint8_t scale_index = 0;
            for (int i = 0; i < 7; i++) {
                if (major_scale[i] == scale_degree) {
                    scale_index = i;
                    break;
                }
            }

            chord->notes[chord->note_count++] = base_note;

            // Add 3rd if TOGGLE_1 is on
            if (toggle_states[0]) {
                uint8_t third = base_note + major_scale[(scale_index + 2) % 7] - major_scale[scale_index];
                if (third < base_note) third += 12;
                chord->notes[chord->note_count++] = third;
            }

            // Add 5th if TOGGLE_2 is on
            if (toggle_states[1]) {
                uint8_t fifth = base_note + major_scale[(scale_index + 4) % 7] - major_scale[scale_index];
                if (fifth < base_note) fifth += 12;
                chord->notes[chord->note_count++] = fifth;
            }

            // Add 7th if TOGGLE_3 is on
            if (toggle_states[2]) {
                uint8_t seventh = base_note + major_scale[(scale_index + 6) % 7] - major_scale[scale_index];
                if (seventh < base_note) seventh += 12;
                chord->notes[chord->note_count++] = seventh;
            }

            // Send note-on for all notes in the chord
            for (int i = 0; i < chord->note_count; i++) {
                midi_send_noteon(&midi_device, 0, chord->notes[i], 127);
            }
        } else {
            // Send note-off for all notes in the chord
            for (int i = 0; i < chord->note_count; i++) {
                midi_send_noteoff(&midi_device, 0, chord->notes[i], 0);
            }
            // Clear chord state
            chord->note_count = 0;
        }
        return false;
    }
    return true;

}

// bool process_record_user(uint16_t keycode, keyrecord_t *record) {
//     if (!process_midi_messages(keycode, record)) {
//         return false;
//     }

//     // Rest of your switch statement for other keys...
// }

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    process_midi_messages(keycode, record);
	switch (keycode) {
        case MIDI_CC_UP:
            if (record->event.pressed) {
                midi_cc_up(midi_device);
            }
            return false;
        case MIDI_CC_DOWN:
            if (record->event.pressed) {
                midi_cc_down(midi_device);
            }
            return false;
		
        case TOGGLE_1:case TOGGLE_4:case TOGGLE_5:
        case TOGGLE_2:
        case TOGGLE_3:
            if (record->event.pressed) {
                uint8_t index = keycode - TOGGLE_1;
                toggle_states[index] = !toggle_states[index];
                midi_send_cc(&midi_device, midi_config.channel, MIDI_CC_TOGGLE_1 + index, toggle_states[index] ? 127 : 0);
                if (toggle_states[index]) {
                    rgb_matrix_set_color(index, RGB_RED);
                } else {
                    rgb_matrix_set_color(index, RGB_OFF);
                }
            }
            return false;
		case RISKY_CHORDS_ENABLE:
			if (record->event.pressed) 
				enabledGreen = true;
			else enabledGreen = false;
			redrawLEDs();
			return false;
		case SUS2_ENABLE:
			// if (record->event.pressed) 
			// 	enabledSus2 = true;
			// else enabledSus2 = false;
			return false;
		case SUS4_ENABLE:
		// if (record->event.pressed) 
		// 		enabledSus4 = true;
		// 	else enabledSus4 = false;
			return false;

        default:
            return true;
    }
    return true;
}

