//Midi keyboard with automatic chord playing and guides for Stephen Mugglin's Chord Maps

#include QMK_KEYBOARD_H
#include "custom_actions.c"
#include "quantum.h"
#include "midi.h"
#include "rgb_matrix.h"
#include "id75.h"


#if defined(CONSOLE_ENABLE)
    #include "print.h"
#else
    #include "debug.h"
#endif


#define MIDI_CC_TOGGLE_1 20
#define MIDI_CC_TOGGLE_2 21
#define MIDI_CC_TOGGLE_3 22
#define MIDI_CC_TOGGLE_4 23
#define MIDI_CC_TOGGLE_5 24
#define MIDI_CC_TOGGLE_6 25
#define MIDI_CC_TOGGLE_7 26
#define MIDI_CC_TOGGLE_8 27
#define MIDI_CC_TOGGLE_9 28
#define MIDI_CC_TOGGLE_10 29


#define MIDI_CC_KEY 30

#define NUM_MIDI_KEYS 65

MidiDevice midi_device;

bool toggle_states[10] = {false, false, false, false, false, false, false, false, false, false};

uint8_t relativeNotes[MATRIX_ROWS][MATRIX_COLS]  = {0};

uint8_t key_signature = 0;

bool enabledSus4 = false, enabledSus2 = false, enabledGreen = false;

const uint8_t major_scale[] = {0, 2, 4, 5, 7, 9, 11};

uint8_t last_chord_index = 0;
bool risky_mode = false;

// Add this global variable
uint8_t chord_grid[MATRIX_ROWS][MATRIX_COLS] = {0};  // Initialize all to 0

bool risky_chord_played = false;

bool initted = false;

enum custom_keycodes {
    MIDI_CC_UP = SAFE_RANGE,
    MIDI_CC_DOWN,
	TOGGLE_1,
    TOGGLE_2,
    TOGGLE_3,
	TOGGLE_4,
	TOGGLE_5,
    TOGGLE_6,
    TOGGLE_7,
    TOGGLE_8,
    TOGGLE_9,
    TOGGLE_10,
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

const int8_t chord_tones[][5] = {
    {0, -1, -1, -1, -1},  // 0 (blank)
    {0, 4, 7, 11, -1},     // 1 (I7)
    {0, 4, 7, 11, -1},     // 2 (II7)
    {0, 4, 7, 11, -1},     // 3 (III7)
    {0, 4, 7, 11, -1},     // 4 (IV7)
    {0, 4, 7, 11, -1},     // 5 (V7)
    {0, 4, 7, 11, -1},     // 6 (VI7)
    {0, 4, 7, 10, -1},     // 7 (VII7) - Note: typically a diminished 7th
    {0, 3, 6, 10, -1},     // 8 (IIIm7b5)
    {0, 3, 6, 9, -1},      // 9 (#Idim7)
    {0, 4, 7, -1, -1},     // 10 (VI)
    {0, 3, 6, 10, -1},     // 11 (#IVm7b5)
    {0, 4, 7, -1, -1},     // 12 (VII)
    {0, 3, 6, 9, -1},      // 13 (#IIdim7)
    {0, 3, 7, -1, -1},     // 14 (Vm)
    {0, 4, 7, -1, -1},     // 15 (I)
    {-1, -1, -1, -1, -1},  // 16 (blank)
    {0, 4, 7, 9, -1},      // 17 (Im6)
    {0, 2, 7, -1, -1},     // 18 (vo2)
    {0, 4, 7, -1, -1},     // 19 (II)
    {0, 4, 7, -1, -1},     // 20 (bVI)
    {0, 4, 7, -1, -1},     // 21 (bVII)
    {0, 3, 6, 10, -1},     // 22 (VIm7b5ob3)
    {0, 3, 6, 9, -1},      // 23 (#Vdim7)
    {0, 4, 7, -1, -1},     // 24 (III)
    {0, 3, 6, 10, -1},     // 25 (VIIm7b5)
    {-1, -1, -1, -1, -1},  // 26 (blank)
    {0, 3, 6, -1, -1},     // 27 (Idimob3)
    {0, 4, 7, 10, -1},     // 28 (bVI7)
    {0, 4, 7, 10, 14},     // 29 (bVII9)
    {-1, -1, -1, -1, -1},  // 30 (blank)
    {0, 7, -1, -1, -1},    // 31 (Io5)
    {0, 3, 7, 10, -1},     // 32 (IVm7)
    {0, 4, 7, 10, -1},     // 33 (bII7)
};

const int8_t chord_root_offsets[] = {
    -1,  // 0 (blank)
    0,   // 1 (I)
    2,   // 2 (II)
    4,   // 3 (III)
    5,   // 4 (IV)
    7,   // 5 (V)
    9,   // 6 (VI)
    11,  // 7 (VII)
    4,   // 8 (IIIm7b5)
    1,   // 9 (#Idim7)
    9,   // 10 (VI)
    6,   // 11 (#IVm7b5)
    11,  // 12 (VII)
    3,   // 13 (#IIdim7)
    7,   // 14 (Vm)
    0,   // 15 (I)
    -1,  // 16 (blank)
    0,   // 17 (Im6)
    7,   // 18 (vo2)
    2,   // 19 (II)
    8,   // 20 (bVI)
    10,  // 21 (bVII)
    9,   // 22 (VIm7b5ob3)
    8,   // 23 (#Vdim7)
    4,   // 24 (III)
    11,  // 25 (VIIm7b5)
    -1,  // 26 (blank)
    0,   // 27 (Idimob3)
    8,   // 28 (bVI7)
    10,  // 29 (bVII9)
    -1,  // 30 (blank)
    0,   // 31 (Io5)
    5,   // 32 (IVm7)
    1,   // 33 (bII7)
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
        [0] = LAYOUT_ortho_5x15(RISKY_CHORDS_ENABLE, TOGGLE_6, MI_B2, MI_As2, MI_A2, MI_Gs2, MI_G2, MI_Fs2, MI_F2, MI_E2, MI_Ds2, MI_D2, MI_Cs2, MI_C2, MI_B1,
								SUS2_ENABLE, TOGGLE_7, MI_E3, MI_Ds3, MI_D3, MI_Cs3, MI_C3, MI_B2, MI_As2, MI_A2, MI_Gs2, MI_G2, MI_Fs2, MI_F2, MI_E2,
								SUS4_ENABLE, TOGGLE_8, MI_A3, MI_Gs3, MI_G3, MI_Fs3, MI_F3, MI_E3, MI_Ds3, MI_D3, MI_Cs3, MI_C3, MI_B2, MI_As2, MI_A2,
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



uint8_t nextChordsRisky[][7] = {
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
#define CHORD_GREEN  {0,   255,   0}
#define CHORD_BLUE {0, 0, 255}
#define KEY_CENTER {255, 0, 255}

// Enum for color indices
enum led_colors {
    COLOR_IN_SCALE,
    COLOR_NOT_IN_SCALE,
    COLOR_CURRENTLY_PLAYED,
    COLOR_FEEDBACK_PLAYED,
    COLOR_CHORD_RISKY,
	COLOR_CHORD_SAFE,
	COLOR_KEY_CENTER,
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
	KEY_CENTER ,
};


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
    update_next_chord_leds();
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

void set_xy_led(uint8_t x, uint8_t y, uint8_t color_index) {
    uint8_t led_index = g_led_config.matrix_co[y][x];
    if (led_index != NO_LED) {
        rgb_matrix_set_color(led_index, 
                             pgm_read_byte(&LED_COLORS[color_index][0]),
                             pgm_read_byte(&LED_COLORS[color_index][1]),
                             pgm_read_byte(&LED_COLORS[color_index][2]));
    }
}
uint8_t midiR = 0;
void midi_receive_cc(MidiDevice* device, uint8_t channel, uint8_t control, uint8_t value) {
    xprintf("midi cc in %d %d %d\n", channel, value, control);
    midiR++;
    switch (control) {
        case MIDI_CC_KEY:
            xprintf("set key %d\n", value);

            setKeyLEDs(value);
			// redrawLEDs();
            break;        
    }
}



void matrix_init_user(void) {
    initted = true;
    dprint("keyboard_post_init_user\n");
    dprint("matrix_init_user start\n");
    midi_device_init(&midi_device);
    midi_register_cc_callback(&midi_device, midi_receive_cc);
    rgb_matrix_enable();
    xprintf("RGB Matrix initialized\n");
    setKeyLEDs(0);
    // rgb_matrix_set_color_all(0, 0, 0);  // Turn off all LEDs
}

void keyboard_pre_init_user(void) {
    debug_enable=true;
    debug_matrix=true;
    dprint("keyboard_pre_init_user\n");
}

void keyboard_post_init_user(void) {
    
    // redrawLEDs();
 
    // rgb_matrix_sethsv_noeeprom(HSV_OFF); // Set all LEDs to off
    dprint("MIDI CC callback registered\n");
}

#define MAX_NOTES_PER_CHORD 7

struct ChordState {
    uint8_t notes[MAX_NOTES_PER_CHORD];
    uint8_t note_count;
};

struct ChordState chord_states[NUM_MIDI_KEYS];

bool process_midi_messages(uint16_t keycode, keyrecord_t *record) {
    xprintf("process midi mesages \n");
    if (initted)
        xprintf("has initted \n");
    if (keycode >= MI_B1 && keycode <= MI_G4) {
        xprintf("MIDI keycode detected %d %d\n", keycode, MI_B2);
        uint8_t chord_index = keycode - MI_B1;
        struct ChordState *chord = &chord_states[chord_index];

        if (record->event.pressed) {
            xprintf("Key pressed \n");
            // Clear previous chord state
            chord->note_count = 0;

            uint8_t row = record->event.key.row;
            uint8_t col = record->event.key.col;
            uint8_t chord_type = chord_grid[row][col];
            uint8_t base_note = keycode - MI_Cs3 + 60;

            bool is_regular_chord = (chord_type >= 1 && chord_type <= 7);

            for (int i = 0; i < 5; i++) {
                xprintf("iterate a\n");
                int8_t interval = chord_tones[chord_type][i];
                if (interval == -1){
                    xprintf("exit early, no interval \n");
                    // dprint("exit early, no interval\n");
                    break;
                } 
                // xprintf("iterate b\n");
                uint8_t note = base_note + interval;

                bool add_note = false;
                if (is_regular_chord) {
                    xprintf("is_regular_chord: \n");
                    // For regular chords (1-7)
                    if (i == 0) add_note = true;  // Always add root
                    else if (i == 1 && toggle_states[0]) add_note = true;  // 2nd note if TOGGLE_1 is on
                    else if (i == 2 && toggle_states[1]) add_note = true;  // 3rd note if TOGGLE_2 is on
                    else if (i == 3 && toggle_states[2]) add_note = true;  // 7th note if TOGGLE_3 is on
                } else {
                    xprintf("not is_regular_chord: %d\n", is_regular_chord);
                    // For other chords, always add all defined notes
                    add_note = true;
                }

                if (add_note) {
                    xprintf("add note : %d\n", note);
                    chord->notes[chord->note_count++] = note;
                }
            }

            // Send note-on for all notes in the chord
            for (int i = 0; i < chord->note_count; i++) {
                xprintf("MIDI noteon %d\n", chord->notes[i]);
                midi_send_noteon(&midi_device, 0, chord->notes[i], 127);
            }
        } else {
            // Send note-off for all notes in the chord
            xprintf("MIDI up %d\n", chord->note_count);

            for (int i = 0; i < chord->note_count; i++) {
                xprintf("MIDI noteoff %d\n", chord->notes[i]);

                midi_send_noteoff(&midi_device, 0, chord->notes[i], 0);
            }
            // Clear chord state
            chord->note_count = 0;
        }
        return false;
    }
    xprintf("Exiting process_midi_messages\n");
    return true;
}





void update_next_chord_leds(void) {
    const uint8_t (*next_chords)[7];
    uint8_t num_next_chords;

    if (risky_chord_played || (risky_mode && !risky_chord_played)) {
        next_chords = nextChordsRisky;
        num_next_chords = 5;  // Adjust based on actual array size
    } else {
        next_chords = nextChordsSafe;
        num_next_chords = 7;  // Adjust based on actual array size
    }

    // Turn off all LEDs first and clear the chord grid
    rgb_matrix_set_color_all(0, 0, 0);
    memset(chord_grid, 0, sizeof(chord_grid));

    // Light up LEDs for available next chords
    for (int i = 0; i < num_next_chords; i++) {
        uint8_t next_chord_index = next_chords[last_chord_index][i];
        if (next_chord_index == 0) break;  // End of available chords

        // Calculate the LED position for this chord
        int8_t root_offset = chord_root_offsets[next_chord_index];
        if (root_offset != -1) {
            // Find the first key with this root note
            for (int y = 0; y < MATRIX_ROWS; y++) {
                for (int x = 2; x < MATRIX_COLS; x++) {
                    if (relativeNotes[y][x] == root_offset) {
                        set_xy_led(x, y, (risky_chord_played || risky_mode) ? COLOR_CHORD_RISKY : COLOR_CHORD_SAFE);
                        chord_grid[y][x] = next_chord_index;  // Store the chord index in the grid
                        goto next_chord;  // Move to the next chord after finding the first matching key
                    }
                }
            }
        }
        next_chord:
        continue;
    }
    // rgb_matrix_update_pwm_buffers();
}



bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    xprintf("process_record_user called: keycode = %u %d\n", keycode, midiR);
    //  if (keycode >= MI_B1 && keycode <= MI_G4) {  
    //     if (record->event.pressed) {
    //         uint8_t y = record->event.key.row;
    //         uint8_t x = record->event.key.col;
    //         uint8_t chord_index = chord_grid[y][x];
            
    //         if (chord_index != 0) {  // If a valid chord index is found
    //             last_chord_index = chord_index;
    //             update_next_chord_leds();
    //         }
    //     }
    //     return process_midi_messages(keycode, record);
    // }

	if (keycode >= MI_B1 && keycode <= MI_G4) {  
		if (record->event.pressed) {
			uint8_t y = record->event.key.row;
			uint8_t x = record->event.key.col;
			uint8_t chord_index = chord_grid[y][x];
			
			if (chord_index != 0) {  // If a valid chord index is found
				last_chord_index = chord_index;
				
				// Check if this is a risky chord
				bool is_risky_chord = (chord_index > 7);  // Assuming chords 1-7 are safe, and 8+ are risky
				
				if (is_risky_chord) {
					risky_chord_played = true;
				} else if (risky_chord_played) {
					// If we're playing a safe chord after a risky one, reset
					risky_chord_played = false;
				}
				
				update_next_chord_leds();
			}
		}
        
		return process_midi_messages(keycode, record);
	}

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
        case TOGGLE_2:case TOGGLE_6:case TOGGLE_7:
        case TOGGLE_3:case TOGGLE_8:case TOGGLE_9:case TOGGLE_10:
            if (record->event.pressed) {
                uint8_t index = keycode - TOGGLE_1;
                toggle_states[index] = !toggle_states[index];
                midi_send_cc(&midi_device, midi_config.channel, MIDI_CC_TOGGLE_1 + index, toggle_states[index] ? 127 : 0);
                xprintf("Toggle %d state: %d\n", index + 1, toggle_states[index]);
                if (toggle_states[index]) {
                    //  rgb_matrix_set_color(index, 255, 0, 0);
                    //rgb_matrix_set_color(1+index, RGB_RED); 
                    xprintf("Setting LED %d to red\n", index);
                } else {
                    // rgb_matrix_set_color(index, 0, 0, 0);
                    //rgb_matrix_set_color(1+index, RGB_OFF); 

                    xprintf("Turning LED %d off\n", index);
                }
            }
            // rgb_matrix_update_pwm_buffers();
            // rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR); // Refresh the display

            return false;
		case RISKY_CHORDS_ENABLE:
			if (record->event.pressed) {
				risky_mode = true;
				update_next_chord_leds();
			} else {
				risky_mode = false;
				if (!risky_chord_played) {
					update_next_chord_leds();
				}
			}
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

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    
    update_next_chord_leds();
    for (uint8_t index = 0; index < 5; index++){
        if (toggle_states[index]) {
            set_xy_led(0, index, (risky_chord_played || risky_mode) ? COLOR_CHORD_RISKY : COLOR_CHORD_SAFE);
        } else {
            set_xy_led(0, index, (risky_chord_played || risky_mode) ? COLOR_CURRENTLY_PLAYED : COLOR_FEEDBACK_PLAYED);
        }
         if (toggle_states[index+5]) {
            set_xy_led(1, index, (risky_chord_played || risky_mode) ? COLOR_CHORD_RISKY : COLOR_CHORD_SAFE);
        } else {
            set_xy_led(1, index, (risky_chord_played || risky_mode) ? COLOR_CURRENTLY_PLAYED : COLOR_FEEDBACK_PLAYED);
        }             
    }
    
   

    return false;
}