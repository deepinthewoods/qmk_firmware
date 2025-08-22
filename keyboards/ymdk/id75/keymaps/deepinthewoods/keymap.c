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


#define MIDI_CC_KEY 101

#define NUM_MIDI_KEYS 65

MidiDevice midi_device;

bool toggle_states[10] = {false, false, false, false, false, false, false, true, true, true};

uint8_t relativeNotes[MATRIX_ROWS][MATRIX_COLS]  = {0};

uint8_t key_signature = 0;

bool enabledSus4 = false, enabledSus2 = false, enabledGreen = false;

const uint8_t major_scale[] = {0, 2, 4, 5, 7, 9, 11};

uint8_t last_chord_index = 1;
bool risky_mode = false;

int8_t octave = 3;

// Add this global variable
uint8_t chord_grid[MATRIX_ROWS][MATRIX_COLS] = {0};  // Initialize all to 0

bool risky_chord_played = false;

bool initted = false;

bool sus2_active = false;
bool sus4_active = false;


int8_t extra_octaves = 0;

// System selection state variables
uint8_t current_system = 0; // 0-5 for 6 different systems

bool chord_button_held = false;
bool sus2_button_held = false;
bool sus4_button_held = false;
uint16_t current_chord_keycode = 0;

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
	RISKY_CHORDS_ENABLE,
    KEY_CENTER_UP,
    KEY_CENTER_DOWN,
    OCTAVE_UP,
    OCTAVE_DOWN,
	
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
		bII7 = 33,
        It6 = 34,
        Fr6 = 35,
        Ger6 = 36;

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
    {0, 4, 6, -1, -1},      // 34 (It+6) - built on b6: root, major third, augmented fourth
    {0, 2, 4, 6, -1},       // 35 (Fr+6) - built on b6: root, major second, major third, augmented fourth
    {0, 3, 4, 6, -1},    // 36 (Ger+6) - built on b6: root, minor third, major third, augmented fourth
};

const int8_t chord_tones_sus2[][5] = {
    {0, -1, -1, -1, -1},  // 0 (blank)
    {0, 2, 7, 11, -1},    // 1 (Isus2) - root, major second, fifth, seventh
    {0, 2, 7, 11, -1},    // 2 (IIsus2)
    {0, 2, 7, 11, -1},    // 3 (IIIsus2) - changed from minor second to major second
    {0, 2, 7, 11, -1},    // 4 (IVsus2)
    {0, 2, 7, 11, -1},    // 5 (Vsus2)
    {0, 2, 7, 11, -1},    // 6 (VIsus2)
    {0, 2, 7, 10, -1},    // 7 (VIIsus2) - keeps diminished seventh
    {0, 2, 6, 10, -1},    // 8 (IIIm7b5sus2)
    {0, 2, 6, 9, -1},     // 9 (#Idim7sus2)
    {0, 2, 7, -1, -1},    // 10 (VIsus2)
    {0, 2, 6, 10, -1},    // 11 (#IVm7b5sus2)
    {0, 2, 7, -1, -1},    // 12 (VIIsus2)
    {0, 2, 6, 9, -1},     // 13 (#IIdim7sus2)
    {0, 2, 7, -1, -1},    // 14 (Vmsus2)
    {0, 2, 7, -1, -1},    // 15 (Isus2)
    {-1, -1, -1, -1, -1}, // 16 (blank)
    {0, 2, 7, 9, -1},     // 17 (Im6sus2)
    {0, 2, 7, -1, -1},    // 18 (vo2sus2)
    {0, 2, 7, -1, -1},    // 19 (IIsus2)
    {0, 2, 7, -1, -1},    // 20 (bVIsus2)
    {0, 2, 7, -1, -1},    // 21 (bVIIsus2)
    {0, 2, 6, 10, -1},    // 22 (VIm7b5ob3sus2)
    {0, 2, 6, 9, -1},     // 23 (#Vdim7sus2)
    {0, 2, 7, -1, -1},    // 24 (IIIsus2)
    {0, 2, 6, 10, -1},    // 25 (VIIm7b5sus2)
    {-1, -1, -1, -1, -1}, // 26 (blank)
    {0, 2, 6, -1, -1},    // 27 (Idimob3sus2)
    {0, 2, 7, 10, -1},    // 28 (bVI7sus2)
    {0, 2, 7, 10, 14},    // 29 (bVII9sus2)
    {-1, -1, -1, -1, -1}, // 30 (blank)
    {0, 2, 7, -1, -1},    // 31 (Io5sus2)
    {0, 2, 7, 10, -1},    // 32 (IVm7sus2)
    {0, 2, 7, 10, -1},    // 33 (bII7sus2)
    {0, 4, 6, -1, -1},    // 34 (It+6)
    {0, 2, 4, 6, -1},     // 35 (Fr+6)
    {0, 3, 4, 6, -1},     // 36 (Ger+6)
};

const int8_t chord_tones_sus4[][5] = {
    {0, -1, -1, -1, -1},  // 0 (blank)
    {0, 5, 7, 11, -1},    // 1 (Isus4) - root, perfect fourth, fifth, seventh
    {0, 5, 7, 11, -1},    // 2 (IIsus4)
    {0, 5, 7, 11, -1},    // 3 (IIIsus4)
    {0, 5, 7, 11, -1},    // 4 (IVsus4)
    {0, 5, 7, 11, -1},    // 5 (Vsus4)
    {0, 5, 7, 11, -1},    // 6 (VIsus4)
    {0, 5, 7, 10, -1},    // 7 (VIIsus4) - keeps diminished seventh
    {0, 5, 6, 10, -1},    // 8 (IIIm7b5sus4)
    {0, 5, 6, 9, -1},     // 9 (#Idim7sus4)
    {0, 5, 7, -1, -1},    // 10 (VIsus4)
    {0, 5, 6, 10, -1},    // 11 (#IVm7b5sus4)
    {0, 5, 7, -1, -1},    // 12 (VIIsus4)
    {0, 5, 6, 9, -1},     // 13 (#IIdim7sus4)
    {0, 5, 7, -1, -1},    // 14 (Vmsus4)
    {0, 5, 7, -1, -1},    // 15 (Isus4)
    {-1, -1, -1, -1, -1}, // 16 (blank)
    {0, 5, 7, 9, -1},     // 17 (Im6sus4)
    {0, 5, 7, -1, -1},    // 18 (vo2sus4)
    {0, 5, 7, -1, -1},    // 19 (IIsus4)
    {0, 5, 7, -1, -1},    // 20 (bVIsus4)
    {0, 5, 7, -1, -1},    // 21 (bVIIsus4)
    {0, 5, 6, 10, -1},    // 22 (VIm7b5ob3sus4)
    {0, 5, 6, 9, -1},     // 23 (#Vdim7sus4)
    {0, 5, 7, -1, -1},    // 24 (IIIsus4)
    {0, 5, 6, 10, -1},    // 25 (VIIm7b5sus4)
    {-1, -1, -1, -1, -1}, // 26 (blank)
    {0, 5, 6, -1, -1},    // 27 (Idimob3sus4)
    {0, 5, 7, 10, -1},    // 28 (bVI7sus4)
    {0, 5, 7, 10, 14},    // 29 (bVII9sus4)
    {-1, -1, -1, -1, -1}, // 30 (blank)
    {0, 5, 7, -1, -1},    // 31 (Io5sus4)
    {0, 5, 7, 10, -1},    // 32 (IVm7sus4)
    {0, 5, 7, 10, -1},    // 33 (bII7sus4)
    {0, 4, 6, -1, -1},    // 34 (It+6)
    {0, 2, 4, 6, -1},     // 35 (Fr+6)
    {0, 3, 4, 6, -1},     // 36 (Ger+6)
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
    8,   // 34 (It+6) - built on b6
    8,   // 35 (Fr+6) - built on b6
    8,   // 36 (Ger+6) - built on b6
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
        [0] = LAYOUT_ortho_5x15(RISKY_CHORDS_ENABLE, KEY_CENTER_UP, MI_B2, MI_As2, MI_A2, MI_Gs2, MI_G2, MI_Fs2, MI_F2, MI_E2, MI_Ds2, MI_D2, MI_Cs2, MI_C2, MI_B1,
								SUS2_ENABLE, KEY_CENTER_DOWN, MI_E3, MI_Ds3, MI_D3, MI_Cs3, MI_C3, MI_B2, MI_As2, MI_A2, MI_Gs2, MI_G2, MI_Fs2, MI_F2, MI_E2,
								SUS4_ENABLE, TOGGLE_8, MI_A3, MI_Gs3, MI_G3, MI_Fs3, MI_F3, MI_E3, MI_Ds3, MI_D3, MI_Cs3, MI_C3, MI_B2, MI_As2, MI_A2,
								OCTAVE_UP, TOGGLE_9, MI_D4, MI_Cs4, MI_C4, MI_B3, MI_As3, MI_A3, MI_Gs3, MI_G3, MI_Fs3, MI_F3, MI_E3, MI_Ds3, MI_D3,
								OCTAVE_DOWN, TOGGLE_10, MI_G4, MI_Fs4, MI_F4, MI_E4, MI_Ds4, MI_D4, MI_Cs4, MI_C4, MI_B3, MI_As3, MI_A3, MI_Gs3, MI_G3),
								
        [1] = LAYOUT_ortho_5x15(
			KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
		 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, RGB_MOD,
		  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
		  , QK_BOOT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS)
};

// [system_index][safe_or_risky][chord_index][next_chord_slots]
uint8_t nextChords[6][2][37][8] = {
    { // System 0 (Original system)
        { // Safe chords [0][0]
            {}, // 0
            {2,3,4,5,6}, // 1
            {1, 3,5, 32, 33, 31}, // 2
            {1,4,6}, // 3
            {2,5,1, 31}, // 4
            {1, 3, 6}, // 5
            {2, 4, 1}, // 6
            {}, // 7
            {}, // 8 (IIIm7b5)
            {}, // 9 (#Idim7)
            {}, // 10 (VI)
            {}, // 11 (#IVm7b5)
            {}, // 12 (VII)
            {}, // 13 (#IIdim7)
            {}, // 14 (Vm)
            {}, // 15 (I)
            {}, // 16
            {}, // 17 (Im6)
            {}, // 18 (vo2)
            {}, // 19 (II)
            {}, // 20 (bVI)
            {}, // 21 (bVII)
            {}, // 22 (VIm7b5ob3)
            {}, // 23 (#Vdim7)
            {}, // 24 (III)
            {}, // 25 (VIIm7b5)
            {}, // 26
            {}, // 27 (Idimob3)
            {}, // 28 (bVI7)
            {}, // 29 (bVII9)
            {}, // 30
            {2, 3, 4, 5, 6}, // 31 (Io5)
            {1}, // 32 (IVm7)
            {1}  // 33 (bII7)
        },
        { // Risky chords [0][1]
            {}, // 0
            {}, // 1
            {}, // 2
            {}, // 3
            {}, // 4
            {}, // 5
            {}, // 6
            {}, // 7
            {10, 4}, // 8 (IIIm7b5)
            {2}, // 9 (#Idim7)
            {2}, // 10 (VI)
            {12, 31, 5}, // 11 (#IVm7b5)
            {3}, // 12 (VII)
            {3}, // 13 (#IIdim7)
            {15}, // 14 (Vm)
            {4}, // 15 (I)
            {}, // 16
            {19, 18}, // 17 (Im6)
            {19}, // 18 (vo2)
            {5}, // 19 (II)
            {21}, // 20 (bVI)
            {1}, // 21 (bVII)
            {19}, // 22 (VIm7b5ob3)
            {6}, // 23 (#Vdim7)
            {6}, // 24 (III)
            {24}, // 25 (VIIm7b5)
            {}, // 26
            {2}, // 27 (Idimob3)
            {31}, // 28 (bVI7)
            {31}, // 29 (bVII9)
            {}, // 30
            {2, 3, 4, 5, 6}, // 31 (Io5)
            {1}, // 32 (IVm7)
            {1}  // 33 (bII7)
        }
    },
    { // System 1 https://chordprogressionblog.wordpress.com/2016/10/11/music-theory-the-chord-progression/
        { // Safe chords [1][0]
            {}, // 0
            {1,2,3,4,5,6,7}, // 1
            {3,5, 1}, // 2
            {4,6,2, 1}, // 3
            {1,3,5}, // 4
            {1, 6}, // 5
            {2, 4, 1}, // 6
            {1, 6}, // 7
            {}, // 8 (IIIm7b5)
            {}, // 9 (#Idim7)
            {}, // 10 (VI)
            {}, // 11 (#IVm7b5)
            {}, // 12 (VII)
            {}, // 13 (#IIdim7)
            {}, // 14 (Vm)
            {}, // 15 (I)
            {}, // 16
            {}, // 17 (Im6)
            {}, // 18 (vo2)
            {}, // 19 (II)
            {}, // 20 (bVI)
            {}, // 21 (bVII)
            {}, // 22 (VIm7b5ob3)
            {}, // 23 (#Vdim7)
            {}, // 24 (III)
            {}, // 25 (VIIm7b5)
            {}, // 26
            {}, // 27 (Idimob3)
            {}, // 28 (bVI7)
            {}, // 29 (bVII9)
            {}, // 30
            {2, 3, 4, 5, 6}, // 31 (Io5)
            {1}, // 32 (IVm7)
            {1}  // 33 (bII7)
        },
        { // Risky chords [1][1]
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
        }
    },
    { // System 2 https://www.reddit.com/r/musictheory/comments/3xmc1w/chord_progression_guideline_chart/
        { // Safe chords [2][0]
            {}, // 0
            {1,2,3,4,5,6, 7}, // 1
            {4, 5, 7, 1}, // 2
            {6, 2, 4, 5, 7, 1}, // 3
            {5, 2, 5, 7, 1}, // 4
            {1, 6, 7}, // 5
            {3, 2, 4, 5, 1}, // 6
            {5, 1, 6}, // 7
            {}, // 8 (IIIm7b5)
            {}, // 9 (#Idim7)
            {}, // 10 (VI)
            {}, // 11 (#IVm7b5)
            {}, // 12 (VII)
            {}, // 13 (#IIdim7)
            {}, // 14 (Vm)
            {}, // 15 (I)
            {}, // 16
            {}, // 17 (Im6)
            {}, // 18 (vo2)
            {}, // 19 (II)
            {}, // 20 (bVI)
            {}, // 21 (bVII)
            {}, // 22 (VIm7b5ob3)
            {}, // 23 (#Vdim7)
            {}, // 24 (III)
            {}, // 25 (VIIm7b5)
            {}, // 26
            {}, // 27 (Idimob3)
            {}, // 28 (bVI7)
            {}, // 29 (bVII9)
            {}, // 30
            {2, 3, 4, 5, 6}, // 31 (Io5)
            {1}, // 32 (IVm7)
            {1}  // 33 (bII7)
        },
        { // Risky chords [2][1]
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
        }
    },
    { // System 3 https://hyperbits.com/best-chord-progression-charts/ first
        { // Safe chords [3][0]
            {}, // 0
            {1, 2, 3, 4, 5, 6}, // 1
            {2, 3, 5, 1}, // 2
            {3, 4, 6, 1}, // 3
            {4, 2, 5, 1}, // 4
            {1, 5, 3, 6, 1}, // 5
            {6, 4, 1}, // 6
            {}, // 7
            {}, // 8 (IIIm7b5)
            {}, // 9 (#Idim7)
            {}, // 10 (VI)
            {}, // 11 (#IVm7b5)
            {}, // 12 (VII)
            {}, // 13 (#IIdim7)
            {}, // 14 (Vm)
            {}, // 15 (I)
            {}, // 16
            {}, // 17 (Im6)
            {}, // 18 (vo2)
            {}, // 19 (II)
            {}, // 20 (bVI)
            {}, // 21 (bVII)
            {}, // 22 (VIm7b5ob3)
            {}, // 23 (#Vdim7)
            {}, // 24 (III)
            {}, // 25 (VIIm7b5)
            {}, // 26
            {}, // 27 (Idimob3)
            {}, // 28 (bVI7)
            {}, // 29 (bVII9)
            {}, // 30
            {}, // 31 (Io5)
            {1}, // 32 (IVm7)
            {1}  // 33 (bII7)
        },
        { // Risky chords [3][1]
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
        }
    },
    { // System 4 (New empty system)
        { // Safe chords [4][0]
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
        },
        { // Risky chords [4][1]
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
        }
    },
    { // System 5 (New empty system)
        { // Safe chords [5][0]
             // Safe chords [3][0]
            {}, // 0
            {1, 2, 3, 4, 5, 6, 7}, // 1
 {1, 2, 3, 4, 5, 6, 7}, // 2

 {1, 2, 3, 4, 5, 6, 7}, // 3
 {1, 2, 3, 4, 5, 6, 7}, // 4
 {1, 2, 3, 4, 5, 6, 7}, // 5
 {1, 2, 3, 4, 5, 6, 7}, // 6
              {1, 2, 3, 4, 5, 6, 7}, // 1// 7
            {}, // 8 (IIIm7b5)
            {}, // 9 (#Idim7)
            {}, // 10 (VI)
            {}, // 11 (#IVm7b5)
            {}, // 12 (VII)
            {}, // 13 (#IIdim7)
            {}, // 14 (Vm)
            {}, // 15 (I)
            {}, // 16
            {}, // 17 (Im6)
            {}, // 18 (vo2)
            {}, // 19 (II)
            {}, // 20 (bVI)
            {}, // 21 (bVII)
            {}, // 22 (VIm7b5ob3)
            {}, // 23 (#Vdim7)
            {}, // 24 (III)
            {}, // 25 (VIIm7b5)
            {}, // 26
            {}, // 27 (Idimob3)
            {}, // 28 (bVI7)
            {}, // 29 (bVII9)
            {}, // 30
            {}, // 31 (Io5)
            {1}, // 32 (IVm7)
            {1}  // 33 (bII7)
        },
        { // Risky chords [5][1]
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, 
            {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
        }
    }
};




#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {

};
#endif // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)


// Define colors as 3-element arrays
#define IN_SCALE    {0, 0,   255}
#define NOT_IN_SCALE  {0,   0, 0}
#define CURRENTLY_PLAYED   {0,   0,   255}
#define FEEDBACK_PLAYED  {255, 255, 255}
#define CHORD_GREEN  {0,   255,   0}
#define CHORD_BLUE {0, 255, 100}
#define KEY_CENTER {255, 100, 0}
#define TOGGLE_ON {100, 100, 100}
#define TOGGLE_OFF {0, 0, 0}
#define KEY_CENTER_VALID {255, 0, 0}
#define OCTAVE_COLOR_0 {255, 0, 0}    // Bright Red
#define OCTAVE_COLOR_1 {255, 100, 0}    // Bright orange
#define OCTAVE_COLOR_2 {255, 255, 0}  // Bright Yellow
#define OCTAVE_COLOR_3 {0, 255, 0}    // Bright Green
#define OCTAVE_COLOR_4 {0, 255, 255}  // Bright Cyan
#define OCTAVE_COLOR_5 {0, 0, 255}    // Bright Blue

#define SYSTEM_COLOR_0 {255, 0, 0}    // Bright Red (same as OCTAVE_COLOR_0)
#define SYSTEM_COLOR_1 {255, 100, 0}  // Orange (same as OCTAVE_COLOR_1)  
#define SYSTEM_COLOR_2 {255, 255, 0}  // Yellow (same as OCTAVE_COLOR_2)
#define SYSTEM_COLOR_3 {0, 255, 0}    // Green (same as OCTAVE_COLOR_3)
#define SYSTEM_COLOR_4 {0, 255, 255}  // Cyan (same as OCTAVE_COLOR_4)
#define SYSTEM_COLOR_5 {0, 0, 255}    // Blue (same as OCTAVE_COLOR_5)

// Enum for color indices
enum led_colors {
    COLOR_IN_SCALE,
    COLOR_NOT_IN_SCALE,
    COLOR_CURRENTLY_PLAYED,
    COLOR_FEEDBACK_PLAYED,
    COLOR_CHORD_RISKY,
	COLOR_CHORD_SAFE,
	COLOR_KEY_CENTER,
    COLOR_TOGGLE_ON,
    COLOR_TOGGLE_OFF,
    COLOR_KEY_CENTER_VALID,
    COLOR_OCTAVE_COLOR_0,
    COLOR_OCTAVE_COLOR_1,
    COLOR_OCTAVE_COLOR_2,
    COLOR_OCTAVE_COLOR_3,
    COLOR_OCTAVE_COLOR_4,
    COLOR_OCTAVE_COLOR_5,
    COLOR_SYSTEM_COLOR_0,
    COLOR_SYSTEM_COLOR_1,
    COLOR_SYSTEM_COLOR_2,
    COLOR_SYSTEM_COLOR_3,
    COLOR_SYSTEM_COLOR_4,
    COLOR_SYSTEM_COLOR_5,
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
    TOGGLE_ON ,
    TOGGLE_OFF ,
    KEY_CENTER_VALID ,
    OCTAVE_COLOR_0 ,
    OCTAVE_COLOR_1 ,
    OCTAVE_COLOR_2 ,
    OCTAVE_COLOR_3 ,
    OCTAVE_COLOR_4 ,
    OCTAVE_COLOR_5 ,
    SYSTEM_COLOR_0 ,
    SYSTEM_COLOR_1 ,
    SYSTEM_COLOR_2 ,
    SYSTEM_COLOR_3 ,
    SYSTEM_COLOR_4 ,
    SYSTEM_COLOR_5 ,
   
};


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

void update_next_chords(void) {
    const uint8_t (*next_chords)[8];
    uint8_t num_next_chords;
    bool show_all_risky = false;

    // current_system is now directly managed by up/down controls

    if (risky_chord_played) {
        next_chords = &nextChords[current_system][1][0];
        num_next_chords = 8;
    } else if (risky_mode) {
        show_all_risky = true;
        next_chords = &nextChords[current_system][1][0];
        num_next_chords = sizeof(nextChords[0][1]) / sizeof(nextChords[0][1][0]);
    } else {
        next_chords = &nextChords[current_system][0][0];
        num_next_chords = sizeof(nextChords[0][0]) / sizeof(nextChords[0][0][0]);
    }

    // Clear the chord grid
    xprintf("clear chord grid\n");
    memset(chord_grid, 0, sizeof(chord_grid));

    // Calculate chord_grid values
    for (int y = 0; y < MATRIX_ROWS; y++) {
        for (int x = 2; x < MATRIX_COLS; x++) {
            uint8_t adjusted_note = relativeNotes[y][x];
            
            // First check if this note corresponds to the current chord
            if (last_chord_index > 0) {
                int8_t current_root_offset = chord_root_offsets[last_chord_index];
                if (current_root_offset != -1 && adjusted_note == current_root_offset) {
                    chord_grid[y][x] = last_chord_index;
                    continue;  // Skip checking other chords for this position
                }
            }
            
            if (show_all_risky) {
                for (uint8_t chord_index = 8; chord_index < sizeof(chord_root_offsets); chord_index++) {
                    int8_t root_offset = chord_root_offsets[chord_index];
                    if (root_offset != -1 && adjusted_note == root_offset) {
                        chord_grid[y][x] = chord_index;
                        break;
                    }
                }
            } else {
                for (int i = 0; i < num_next_chords; i++) {
                    uint8_t next_chord_index = next_chords[last_chord_index][i];
                    if (next_chord_index == 0) break;  // End of available chords
                    
                    int8_t root_offset = chord_root_offsets[next_chord_index];
                    if (root_offset != -1 && adjusted_note == root_offset) {
                        chord_grid[y][x] = next_chord_index;
                        xprintf("populate chord grid %d %d %d\n", x, y, next_chord_index);
                        break;
                    }
                }
            }
        }
    }
    xprintf("Final chord_grid:\n");
    for (int y = 0; y < MATRIX_ROWS; y++) {
        for (int x = 2; x < MATRIX_COLS; x++) {
            xprintf("%2d ", chord_grid[y][x]);
        }
        xprintf("\n");
    }
}

void update_next_chord_leds(void) {
    uint8_t chord_color = (risky_mode || risky_chord_played) ? COLOR_CHORD_RISKY : COLOR_CHORD_SAFE;
    rgb_matrix_set_color_all(0, 0, 0);
    
    // Set LEDs based on chord_grid and scale
    for (int y = 0; y < MATRIX_ROWS; y++) {
        for (int x = 2; x < MATRIX_COLS; x++) {
            uint8_t adjusted_note = relativeNotes[y][x];
            uint8_t chord_index = chord_grid[y][x];
            bool note_handled = false;

            // First priority: Chord roots
            if (chord_index != 0) {
                set_xy_led(x, y, chord_color);
                note_handled = true;
            }

            // Second priority: Key center
            if (adjusted_note == 0) {
                if (chord_index != 0) {
                    set_xy_led(x, y, COLOR_KEY_CENTER_VALID);
                } else {
                    set_xy_led(x, y, COLOR_KEY_CENTER);
                }
                note_handled = true;
            }

            // Third priority: Scale notes that aren't already highlighted
            if (!note_handled && is_in_scale(adjusted_note)) {
                set_xy_led(x, y, COLOR_IN_SCALE);
            }
        }
    }

    // Rest of the toggle and octave LED updates remain the same
    set_xy_led(0, 0, toggle_states[0] ? COLOR_TOGGLE_ON : COLOR_TOGGLE_OFF);
    set_xy_led(0, 1, toggle_states[1] ? COLOR_TOGGLE_ON : COLOR_TOGGLE_OFF);
    set_xy_led(0, 2, toggle_states[2] ? COLOR_TOGGLE_ON : COLOR_TOGGLE_OFF);

    // Set octave and extra octaves colors
    int8_t colorIndex = octave - 3;
    int8_t extraOctaveIndex = extra_octaves;
    uint8_t octColor = COLOR_OCTAVE_COLOR_0 + abs(colorIndex) - 1;
    uint8_t extraOctColor = COLOR_OCTAVE_COLOR_0 + abs(extraOctaveIndex) - 1;

    if (risky_mode) {
        // Display extra octaves when risky mode is active
        if (extra_octaves < 0) {
            set_xy_led(0, 4, extraOctColor);
        } else if (extra_octaves > 0) {
            set_xy_led(0, 3, extraOctColor);
        }
        // Display current system color on SUS2 and SUS4 buttons in risky mode
        uint8_t system_color = COLOR_SYSTEM_COLOR_0 + current_system;
        set_xy_led(0, 1, system_color);  // SUS2 button shows current system
        set_xy_led(0, 2, system_color);  // SUS4 button shows current system
    } else {
        // Display normal octave when risky mode is not active
        if (colorIndex < 0) {
            set_xy_led(0, 4, octColor);
        } else if (colorIndex > 0) {
            set_xy_led(0, 3, octColor);
        }
    }

    set_xy_led(1, 0, toggle_states[5] ? COLOR_TOGGLE_ON : COLOR_TOGGLE_OFF);
    set_xy_led(1, 1, toggle_states[6] ? COLOR_TOGGLE_ON : COLOR_TOGGLE_OFF);
    set_xy_led(1, 2, toggle_states[7] ? COLOR_TOGGLE_ON : COLOR_TOGGLE_OFF);
    set_xy_led(1, 3, toggle_states[8] ? COLOR_TOGGLE_ON : COLOR_TOGGLE_OFF);
    set_xy_led(1, 4, toggle_states[9] ? COLOR_TOGGLE_ON : COLOR_TOGGLE_OFF);
}

void setKeyLEDs(int8_t keySignature) {
    key_signature = (keySignature + 12) % 12;
    // xprintf("Setting key signature to: %d\n", key_signature);
    
    for (int y = 0; y < MATRIX_ROWS; y++) {
        for (int x = 2; x < MATRIX_COLS; x++) {
            uint16_t keycode = pgm_read_word(&keymaps[0][y][x]);
            
            // Calculate the note value within the octave (0-11)
            // Assuming MI_C_1 is the lowest C note in your MIDI range
            int8_t note_value = (keycode - MI_C1 + 12) % 12;
            
            // Adjust the note value based on the key signature
            // We subtract the key_signature because moving up in key signatures
            // (adding sharps) is equivalent to moving down in pitch class
            int8_t adjusted_note = (note_value - key_signature + 12) % 12;
            
            relativeNotes[y][x] = adjusted_note;
            
            // xprintf("Key at (%d, %d): keycode %d, note_value %d, adjusted %d\n", 
            //         x, y, keycode, note_value, adjusted_note);
        }
    }
    update_next_chords();
    update_next_chord_leds();
   // uint8_t scaled_key_value = ((key_signature + 2) % 12) * 127 / 11;
    midi_send_cc(&midi_device, 0, MIDI_CC_KEY, key_signature);

}

uint8_t midiR = 0;
void midi_receive_cc(MidiDevice* device, uint16_t cnt, uint8_t byte0, uint8_t byte1, uint8_t byte2) {
    // Handle MIDI message here
    // byte0 is typically the status byte (includes channel and message type)
    // byte1 and byte2 depend on the message type
    // xprintf("midi msg: %d %d %d", byte0, byte1, byte2);
    // uint8_t channel = byte0 & 0x0F;
    uint8_t type = byte0 & 0xF0;

    if (type == 0xB0) {  // Control Change
        // uint8_t control_number = byte1;
        uint8_t control_value = byte2;
        // Handle CC message
        setKeyLEDs(control_value);
    }
    // Handle other MIDI message types as needed
}



void matrix_init_user(void) {
    initted = true;
    // dprint("keyboard_post_init_user\n");
    // dprint("matrix_init_user start\n");
    midi_device_init(&midi_device);
    rgb_matrix_enable();
    // xprintf("RGB Matrix initialized\n");
    setKeyLEDs(key_signature);
    // rgb_matrix_set_color_all(0, 0, 0);  // Turn off all LEDs
}

void keyboard_pre_init_user(void) {
    debug_enable=true;
    debug_matrix=true;
    // dprint("keyboard_pre_init_user\n");
}

void keyboard_post_init_user(void) {
        midi_register_catchall_callback(&midi_device, midi_receive_cc);
}



#define MAX_NOTES_PER_CHORD 28

struct ChordState {
    uint8_t notes[MAX_NOTES_PER_CHORD];
    uint8_t note_count;
};

struct ChordState chord_states[NUM_MIDI_KEYS];

// Add this helper function to compare note arrays
bool contains_note(const uint8_t* notes, uint8_t note_count, uint8_t note) {
    for (int i = 0; i < note_count; i++) {
        if (notes[i] == note) return true;
    }
    return false;
}

void find_keycode_position(uint16_t keycode, uint8_t *row, uint8_t *col) {
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        for (uint8_t c = 0; c < MATRIX_COLS; c++) {
            if (pgm_read_word(&keymaps[0][r][c]) == keycode) {
                *row = r;
                *col = c;
                return;
            }
        }
    }
    // If not found, set invalid values
    *row = 0xFF;
    *col = 0xFF;
}

// Add this function to calculate the new chord state without playing notes
void calculate_chord_state(uint16_t keycode, uint8_t row, uint8_t col, struct ChordState* new_state) {
    new_state->note_count = 0;
    
    uint8_t chord_type = chord_grid[row][col];
    uint8_t base_note = keycode - MI_Cs3 + 60;
    int16_t transposed_base = base_note + ((octave - 3) * 12);
    
    if (transposed_base < 0) transposed_base = 0;
    if (transposed_base > 127) transposed_base = 127;
    
    const int8_t (*chord_tones_to_use)[5];
    if (sus4_active && sus4_button_held) {
        chord_tones_to_use = chord_tones_sus4;
    } else if (sus2_active && sus2_button_held) {
        chord_tones_to_use = chord_tones_sus2;
    } else {
        chord_tones_to_use = chord_tones;
    }

    bool is_regular_chord = (chord_type >= 1 && chord_type <= 7);

    for (int octave_offset = 0; octave_offset <= extra_octaves; octave_offset++) {
        for (int i = 0; i < 5; i++) {
            int8_t interval = chord_tones_to_use[chord_type][i];
            if (interval == -1) break;
           
            int16_t note = transposed_base + interval + (octave_offset * 12);
            
            if (note < 0) note = 0;
            if (note > 127) note = 127;

            bool add_note = false;
            if (is_regular_chord) {
                if (i == 0) add_note = true;
                else if (i == 1 && toggle_states[7]) add_note = true;
                else if (i == 2 && toggle_states[8]) add_note = true;
                else if (i == 3 && toggle_states[9]) add_note = true;
            } else {
                add_note = true;
            }

            if (add_note && new_state->note_count < MAX_NOTES_PER_CHORD) {
                new_state->notes[new_state->note_count++] = note;
            }
        }
    }
}

// Modified play_current_chord_variation function
void play_current_chord_variation(keyrecord_t *record) {
    if (current_chord_keycode) {
        struct ChordState *current_chord = &chord_states[current_chord_keycode - MI_B1];
        struct ChordState new_chord_state = {0};
        
        // Find the actual matrix position for the current chord keycode
        uint8_t row, col;
        find_keycode_position(current_chord_keycode, &row, &col);
        
        if (row != 0xFF && col != 0xFF) {
            // Calculate the new chord state
            calculate_chord_state(current_chord_keycode, row, col, &new_chord_state);
            
            // Compare and only send necessary MIDI messages
            // First, note-offs for notes that aren't in the new chord
            for (int i = 0; i < current_chord->note_count; i++) {
                if (!contains_note(new_chord_state.notes, new_chord_state.note_count, current_chord->notes[i])) {
                    midi_send_noteoff(&midi_device, 0, current_chord->notes[i], 0);
                }
            }
            
            // Then, note-ons for new notes that weren't in the old chord
            for (int i = 0; i < new_chord_state.note_count; i++) {
                if (!contains_note(current_chord->notes, current_chord->note_count, new_chord_state.notes[i])) {
                    midi_send_noteon(&midi_device, 0, new_chord_state.notes[i], 127);
                }
            }
            
            // Update the current chord state
            *current_chord = new_chord_state;
        }
    }
}

// Modified process_midi_messages function
bool process_midi_messages(uint16_t keycode, keyrecord_t *record) {
    if (keycode >= MI_B1 && keycode <= MI_G4) {
        uint8_t chord_index = keycode - MI_B1;
        struct ChordState *chord = &chord_states[chord_index];

        if (record->event.pressed) {
            chord_button_held = true;
            current_chord_keycode = keycode;
            
            uint8_t row = record->event.key.row;
            uint8_t col = record->event.key.col;
            
            // Calculate new chord state and play it
            struct ChordState new_chord_state = {0};
            calculate_chord_state(keycode, row, col, &new_chord_state);
            
            // Send note-on for all notes in the new chord
            for (int i = 0; i < new_chord_state.note_count; i++) {
                midi_send_noteon(&midi_device, 0, new_chord_state.notes[i], 127);
            }
            
            *chord = new_chord_state;
            
            update_next_chords();
            update_next_chord_leds();
        } else {
            // When chord key is released, turn off all notes
            chord_button_held = false;
            for (int i = 0; i < chord->note_count; i++) {
                midi_send_noteoff(&midi_device, 0, chord->notes[i], 0);
            }
            chord->note_count = 0;
            current_chord_keycode = 0;
        }
        return false;
    }
    return true;
}






bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    xprintf("process_record_user called: keycode = %u %d\n", keycode, midiR);

    if (keycode >= MI_B1 && keycode <= MI_G4) {  
        if (record->event.pressed) {
            uint8_t y = record->event.key.row;
            uint8_t x = record->event.key.col;
            uint8_t chord_index = chord_grid[y][x];
            
            if (chord_index != 0) { // If a valid chord index is found or the I chord is played
                last_chord_index = chord_index;
                
                // Check if this is a risky chord
                bool is_risky_chord = (chord_index > 7);  // Assuming chords 1-7 are safe, and 8+ are risky
                
                if (is_risky_chord) {
                    risky_chord_played = true;
                } else if (risky_chord_played) {
                    // If we're playing a safe chord after a risky one, reset
                    risky_chord_played = false;
                    risky_mode = false;
                }
                
                xprintf("Chord played: %d, Risky: %d\n", last_chord_index, risky_chord_played);
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
                    xprintf("Setting LED %d to red\n", index);
                } else {
                    xprintf("Turning LED %d off\n", index);
                }
            }
            return false;
        case RISKY_CHORDS_ENABLE:
            if (record->event.pressed) {
                risky_mode = true;
                update_next_chords();
                update_next_chord_leds();
            } else {
                if (!risky_chord_played) {
                    risky_mode = false;
                    update_next_chords();
                    update_next_chord_leds();
                }
            }
            return false;
        case SUS2_ENABLE:
    if (record->event.pressed) {
        if (risky_mode) {
            // System down (decrease current_system)
            current_system = (current_system == 0) ? 5 : current_system - 1;
            update_next_chords();
            update_next_chord_leds();
        } else {
            sus2_button_held = true;
            // Only activate sus2 if sus4 isn't held (sus4 takes priority)
            if (!sus4_button_held) {
                if (chord_button_held) {
                    // First, send note-off for all currently playing notes
                    struct ChordState *current_chord = &chord_states[current_chord_keycode - MI_B1];
                    for (int i = 0; i < current_chord->note_count; i++) {
                        midi_send_noteoff(&midi_device, 0, current_chord->notes[i], 0);
                    }
                    current_chord->note_count = 0;
                }
                sus2_active = true;
                sus4_active = false;
                play_current_chord_variation(record);
            }
        }
    } else {  // Button released
        if (!risky_mode) {
            sus2_button_held = false;
            sus2_active = false;
            
            if (chord_button_held) {  // Only play new variation if chord is still held
                // First, send note-off for all currently playing notes
                struct ChordState *current_chord = &chord_states[current_chord_keycode - MI_B1];
                for (int i = 0; i < current_chord->note_count; i++) {
                    midi_send_noteoff(&midi_device, 0, current_chord->notes[i], 0);
                }
                current_chord->note_count = 0;
                
                if (sus4_button_held) {
                    // If sus4 is held, switch to sus4 variation
                    sus4_active = true;
                    play_current_chord_variation(record);
                } else {
                    // Switch back to normal version
                    play_current_chord_variation(record);
                }
            }
        }
    }
    return false;

case SUS4_ENABLE:
    if (record->event.pressed) {
        if (risky_mode) {
            // System up (increase current_system)  
            current_system = (current_system == 5) ? 0 : current_system + 1;
            update_next_chords();
            update_next_chord_leds();
        } else {
            sus4_button_held = true;
            if (chord_button_held) {
                // First, send note-off for all currently playing notes
                struct ChordState *current_chord = &chord_states[current_chord_keycode - MI_B1];
                for (int i = 0; i < current_chord->note_count; i++) {
                    midi_send_noteoff(&midi_device, 0, current_chord->notes[i], 0);
                }
                current_chord->note_count = 0;
            }
            // sus4 always takes priority over sus2
            sus4_active = true;
            sus2_active = false;
            play_current_chord_variation(record);
        }
    } else {  // Button released
        if (!risky_mode) {
            sus4_button_held = false;
            sus4_active = false;
            
            if (chord_button_held) {  // Only play new variation if chord is still held
                // First, send note-off for all currently playing notes
                struct ChordState *current_chord = &chord_states[current_chord_keycode - MI_B1];
                for (int i = 0; i < current_chord->note_count; i++) {
                    midi_send_noteoff(&midi_device, 0, current_chord->notes[i], 0);
                }
                current_chord->note_count = 0;
                
                if (sus2_button_held) {
                    // If sus2 is held, switch to sus2 variation
                    sus2_active = true;
                    play_current_chord_variation(record);
                } else {
                    // Switch back to normal version
                    play_current_chord_variation(record);
                }
            }
        }
    }
    return false;
        case KEY_CENTER_UP:
            if (record->event.pressed) {
                setKeyLEDs((uint8_t)(key_signature+1));
            }
            return false;
        case KEY_CENTER_DOWN:
            if (record->event.pressed) {
                setKeyLEDs((uint8_t)(key_signature-1));
            }
            return false;
        case OCTAVE_UP:
            if (record->event.pressed) {
                if (risky_mode) {
                    extra_octaves = (extra_octaves + 1 > 3) ? 3 : extra_octaves + 1;
                } else {
                    octave = (octave + 1) % 10;
                    midi_config.transpose = (octave - 3) * 12;
                }
                update_next_chord_leds();
            }
            return false;
        case OCTAVE_DOWN:
            if (record->event.pressed) {
                if (risky_mode) {
                    extra_octaves = (extra_octaves - 1 < 0) ? 0 : extra_octaves - 1;
                } else {
                    octave = (octave - 1 + 10) % 10;
                    midi_config.transpose = (octave - 3) * 12;
                }
                update_next_chord_leds();
            }
            return false;
        default:
            return true;
    }
}



bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    
    update_next_chord_leds();
   
   

    return false;
}