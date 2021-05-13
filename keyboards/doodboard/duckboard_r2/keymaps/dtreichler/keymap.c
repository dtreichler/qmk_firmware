/* Copyright 2020 doodboard
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

#include "duckboard_r2.h"

enum duckboard_layers {
    _NUMPAD,
    _FUNC,
    _RGB,
    _LR_LIB,
    _LR_DEV,
};

enum duckboard_keycodes {
    LR = SAFE_RANGE,
    DEVELOP,
    LOUPE,
    GRID,
    FUNC,
    LRGB,
    NUMPAD,
};

#define T_NUM TO(_NUMPAD) // Activate numpad layer
#define T_FUNC TG(_FUNC) // Activate function layer
#define T_LIB TO(_LR_LIB) // Activate LR Library Layer
#define T_DEV TO(_LR_DEV) // Activate LR Develop Layer
#define T_RGB TO(_RGB) // Activate RGB layer
#define TGL_FLAG KC_GRV
#define WHITE_BAL KC_W
#define CROP KC_R
#define SPOT KC_Q
#define SET_NEXT KC_DOT
#define SET_PREV KC_COMMA
#define RATE_INC KC_RBRC
#define RATE_DEC KC_LBRC
#define REJECT KC_X
#define RED KC_6
#define YELLOW KC_7
#define GREEN KC_8
#define BLUE KC_9
#define ROT_CW C(KC_RBRC)
#define ROT_CCW C(KC_LBRC)

// #undef ENCODER_RESOLUTION
// #define ENCODER_RESOLUTION 4

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

	[_NUMPAD] = LAYOUT(
		         FUNC,    KC_PSLS, KC_PAST, KC_BSPC,
		         KC_7,    KC_8,    KC_9,    KC_PMNS,
		         KC_4,    KC_5,    KC_6,    KC_PPLS,
		KC_MUTE, KC_1,    KC_2,    KC_3,    KC_ENT,
		T_LIB,   KC_0,    KC_0,    KC_DOT,  RESET
    ),

	[_FUNC] = LAYOUT(
		         _______, _______, _______, _______,
		         KC_HOME, KC_UP,   KC_PGUP, _______,
		         KC_LEFT, _______, KC_RGHT, _______,
		_______, KC_END,  KC_DOWN, KC_PGDN, _______,
		_______, _______, KC_INS,  KC_DEL,  _______
    ),

    [_RGB] = LAYOUT(
		         _______, RGB_TOG, RGB_MOD, _______,
		         RGB_HUI, RGB_SAI, RGB_VAI, _______,
		         RGB_HUD, RGB_SAD, RGB_VAD, _______,
		_______, _______, _______, _______, _______,
		T_NUM,   RESET,   _______, _______, _______
    ),
    
    [_LR_LIB] = LAYOUT(
		         GRID,     LOUPE,     DEVELOP,    XXXXXXX,
		         TGL_FLAG, RATE_DEC,  RATE_INC,   REJECT,
		         RED,      YELLOW,    GREEN,      BLUE,
		XXXXXXX, ROT_CCW,  ROT_CW,    XXXXXXX,    XXXXXXX,
		T_RGB,   XXXXXXX,  XXXXXXX,   XXXXXXX,    XXXXXXX
    ),

    [_LR_DEV] = LAYOUT(
		          GRID,     LOUPE,     DEVELOP,   XXXXXXX,
		          TGL_FLAG, WHITE_BAL, CROP,      SPOT,
		          SET_PREV, SET_NEXT,  XXXXXXX,   XXXXXXX,
		XXXXXXX,  ROT_CCW,  ROT_CW,    XXXXXXX,   XXXXXXX,
		T_RGB,    XXXXXXX,  XXXXXXX,   XXXXXXX,   XXXXXXX
    ),

};

void encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { /* First encoder */
        switch (get_highest_layer(layer_state)) {
            case _NUMPAD:
            case _FUNC:
            case _RGB:
                if (clockwise) {
                    tap_code(KC_VOLU);
                } else {
                    tap_code(KC_VOLD);
                }
                break;
            case _LR_LIB:
                if (clockwise) {
                    tap_code(KC_RGHT);
                } else {
                    tap_code(KC_LEFT);
                }
                break;
            case _LR_DEV:
                if (clockwise) {
                    tap_code(KC_PPLS);
                } else {
                    tap_code(KC_PMNS);
                }
                break;
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
        case FUNC:
            if (record->event.pressed) {
                layer_move(_FUNC);
            }
            return false;
            break;
        case NUMPAD:
            if (record->event.pressed) {
                layer_move(_NUMPAD);
            }
            return false;
            break;
        case GRID:
            if (record->event.pressed) {
                tap_code(KC_G);            
                layer_move(_LR_LIB);
            }
          return false;
          break;
        case LOUPE:
            if (record->event.pressed) {
                tap_code(KC_E);
                layer_move(_LR_LIB);
            }
          return false;
          break;
        case LR:
            if (record->event.pressed) {
                layer_move(_LR_LIB);
            }
          return false;
          break;
        case DEVELOP:
            if (record->event.pressed) {
                tap_code(KC_D);
                layer_move(_LR_DEV);
            }
            return false;
            break;
      }
    return true;
};


#define OLED_DRIVER_ENABLE 1

#ifdef OLED_DRIVER_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_270; }


// WPM-responsive animation stuff here
#define IDLE_FRAMES 2
#define IDLE_SPEED 40 // below this wpm value your animation will idle

#define ANIM_FRAME_DURATION 200 // how long each frame lasts in ms
// #define SLEEP_TIMER 60000 // should sleep after this period of 0 wpm, needs fixing
#define ANIM_SIZE 636 // number of bytes in array, minimize for adequate firmware size, max is 1024

uint32_t anim_timer = 0;
uint32_t anim_sleep = 0;
uint8_t current_idle_frame = 0;

// Credit to u/Pop-X- for the initial code. You can find his commit here https://github.com/qmk/qmk_firmware/pull/9264/files#diff-303f6e3a7a5ee54be0a9a13630842956R196-R333.
static void render_anim(void) {
    static const char PROGMEM idle[IDLE_FRAMES][ANIM_SIZE] = {
        {
        0,  0,192,192,192,192,192,192,192,248,248, 30, 30,254,254,248,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  3,  3,  3,  3,255,255,255,255,255,255,255,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,127,127,255,255,255,255,255,159,159,135,135,129,129,129, 97, 97, 25, 25,  7,  7,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1, 97, 97,127,  1,  1, 97, 97,127,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0
        },
        {
        0,  0,128,128,128,128,128,128,128,240,240, 60, 60,252,252,240,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  7,  7,  7,  7,  7,255,255,254,254,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,255,255,255,255, 63, 63, 15, 15,  3,  3,  3,195,195, 51, 51, 15, 15,  3,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  3, 99, 99,127,  3,  3, 99, 99,127,  3,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0
        }
    };

    //assumes 1 frame prep stage
    void animation_phase(void) {
            current_idle_frame = (current_idle_frame + 1) % IDLE_FRAMES;
            oled_write_raw_P(idle[abs((IDLE_FRAMES-1)-current_idle_frame)], ANIM_SIZE);
    }

        if(timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
            anim_timer = timer_read32();
            animation_phase();
        }
    }

void oled_task_user(void) {
    render_anim();
    oled_set_cursor(0,6);
    oled_write_P(PSTR("DUCK\nBOARD\n"), false);
    oled_write_P(PSTR("-----\n"), false);
    // Host Keyboard Layer Status
    oled_write_P(PSTR("MODE\n"), false);
    oled_write_P(PSTR("\n"), false);

    switch (get_highest_layer(layer_state)) {
        case _NUMPAD:
            oled_write_P(PSTR("NUMPD\n"), false);
            break;
        case _FUNC:
            oled_write_P(PSTR("FUNC\n"), false);
            break;
        case _RGB:
            oled_write_P(PSTR("RGB\n"), false);
            break;
        case _LR_DEV:
            oled_write_P(PSTR("LRDEV\n"), false);
            break;
        case _LR_LIB:
            oled_write_P(PSTR("LRLIB\n"), false);
            break;
    }
}
#endif

void keyboard_post_init_user(void) {
  //Customise these values to debug
  debug_enable=true;
  debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;
}
