/*
 * global.h
 *
 * Created: 2021. 03. 08. 20:49:07
 *  Author: Jedocs
 */ 


#ifndef GLOBAL_H_
#define GLOBAL_H_
#include <atmel_start.h>

#define VERSION "1.0.8"

#define RESPONSE_BEEP_LENGTH 1148
#define SYNC_BEEP_LENGTH 982
#define START_SOUND_LENGTH 8100 //10002
#define STOP_SOUND_LENGTH 10001
#define ERROR_BEEP_LENGTH 3202
#define CLICK_SOUND_LENGTH 376

#define ACK_TIMEOUT 400
#define PRESCALER_VALUE 7 // 7== 1kHz !!!!!!!!

#define TRIGGER_DEBOUNCE_TIME 50
#define PB_DEBOUNCE_TIME 200
#define TRIGGER_LED_DELAY 80

#define ACK 0x06
#define NAK 0x15

#define ON 1
#define OFF 0

// LCD serial interface commands
#define READ_OBJ 0x00
#define WRITE_OBJ 0x01
#define WRITE_STR 0x02
#define WRITE_ STRU 0x03
#define REPORT_OBJ 0x05
#define REPORT_EVENT 0x07

// LCD serial interface command objects
#define DIPSWITCH 0
#define KNOB 1
#define ROCKERSWITCH 2
#define ROTARYSWITCH 3
#define SLIDER 4
#define TRACKBAR 5
#define WINBUTTON 6
#define ANGULARMETER 7
#define COOLGAUGE 8
#define CUSTOMDIGITS 9
#define FORM 10
#define GAUGE 11
#define KEYBOARD 13
#define LED 14
#define LEDDIGITS 15
#define METER 16
#define STRINGS 17
#define THERMOMETER 18
#define USERLED 19
#define VIDEO 20
#define SOUND 22
#define TIMER 23
#define SPECTRUM 24
#define SCOPE 25
#define TANK 26
#define USERIMAGES 27
#define PINOUTPUT 28
#define PININPUT 29
#define FOURDBUTTON 30 // számmal nem kezdõdhet!!!! (?)
#define ANIBUTTON 31
#define COLORPICKER 32
#define USERBUTTON 33

// LCD serial interface sound commands
#define PLAY 0x00
#define PAUSE_PLAY 0x02
#define CONTINUE_PLAY 0x03
#define STOP_PLAY 0x04

//form0
#define START_SESSION 0
#define MENU 1

//form1
#define STOP_SESSION 2
#define CURRENT_SLICE_STR 0
#define CURRENT_VOLUME_STR 1

#define FORM1_TRIGGER_SOUND_SW 2
#define FORM1_RESPONSE_SOUND_SW 3
#define FORM1_TRIGGER_SOUND_STR 20
#define FORM1_RESPONSE_SOUND_STR 21
#define TRIGGER_RECEIVED_LED 3
#define TRIGGER_SENT_LED 2

//form2
#define MAN_TRIG_CURRENT_SLICE_STR 16
#define MAN_TRIG_CURRENT_VOLUME_STR 17
#define MAN_TRIG_SEND_S 8
#define MAN_TRIG_SEND_A 4
#define MAN_TRIG_SEND_B 5
#define MAN_TRIG_SEND_C 6
#define MAN_TRIG_SEND_D 7
#define MAN_TRIG_BACK 3
#define MAN_TRIG_TRIGGER_LED 1

//form3
#define SIMULATION_CURRENT_SLICE_STR 14
#define SIMULATION_CURRENT_VOLUME_STR 15
#define STOP_SIMULATION 13
#define SIMULATION_SEND_A 10
#define SIMULATION_SEND_B 9
#define SIMULATION_SEND_C 11
#define SIMULATION_SEND_D 12

#define SIMULATION_TRIGGER_LED 0


//form4
#define TRIGGER_SOUND_SW 0
#define RESPONSE_SOUND_SW 1
#define SET_SLICESNO_BTN 25
#define SET_VOLUMESNO_BTN 24
#define FORM4_MORE 17
#define FORM4_ACCEPT 16
#define FORM4_CANCEL 15

#define TRIGGER_SOUND_STR 2
#define RESPONSE_SOUND_STR 3
#define SET_SLICES_STR 6
#define SET_VOLUMES_STR 7

//form5
#define SLICE_TRIGGERNO_BTN 14
#define VOLUME_TRIGGERNO_BTN 18
#define PULSE_LENGTH_BTN 23
#define TR_TIME_BTN 22
#define FORM5_MORE 19
#define FORM5_ACCEPT 20
#define FORM5_CANCEL 21

#define SLICE_TRIGGERNO_STR 4
#define VOLUME_TRIGGERNO_STR 5
#define PULSE_LENGTH_STR 8
#define TR_TIME_STR 9

//form6
#define SET_PARAMETER_STR 10

//keyboard buttons
#define KEYBOARD_CANCEL	0x63
#define KEYBOARD_BACK	0xb0
#define KEYBOARD_OK		0x6f

//form7
#define SETTINGS 26
#define SIMULATION 28
#define MANUAL_TRIGGER 27
#define MENU_BACK 32
#define ABOUT 31
#define HELP 37

//form8
#define FORM8_MORE 29
#define FORM8_ACCEPT 30

//form9
#define FORM9_BACK 36

//form10
#define FORM10_BACK 33
#define VERSION_STR 13

//form11
#define ERROR_STR 18

#define DEFAULT_FORM 0
#define SESSION_RUNNING_FORM 1
#define MANUAL_TRIGGER_FORM 2
#define SIMULATION_RUNNING_FORM 3
#define SETTINGS_1_FORM 4
#define SETTINGS_2_FORM 5
#define KEYBOARD_FORM 6
#define MENU_FORM 7
#define SETTINGS_3_FORM 8
#define HELP_FORM 9
#define ABOUT_FORM 10
#define ERROR_FORM 11


#define  PS_VOLTAGE_LOW 0
#define  PS_VOLTAGE_HIGH 1
#define USB_DISCONNECTED 2

//****************


/**
 * This tells the compiler that g_myglobal exists somewhere.
 * Without "extern", this would create a new variable named
 * g_myglobal in _every file_ that included it. Don't miss this!
 */

extern bool ACK_received;
extern bool NAK_received;
extern uint16_t response_beep_length;
extern uint16_t sync_beep_length;
extern uint16_t usb_disconnected;



extern struct report {
	uint8_t object_id;
	uint8_t object_index;
	uint8_t value_msb;
	uint8_t value_lsb;
	uint8_t cksum;
	bool report_valid;
}report;

extern struct session {
	bool trig_LED_on;
	bool trig_sent_LED_on;
	bool session_finished;
	bool session_running;
	bool simulation_mode;
	bool manual_trigger;
	bool trigger;
	bool simulated_A;
	bool simulated_B;
	bool simulated_C;
	bool simulated_D;
	bool trigger_sound;
	bool response_sound;
	uint8_t current_form;
	uint8_t previous_form;
	uint16_t sound_volume;
	uint16_t no_of_volumes;
	uint16_t no_of_slices;
	uint16_t trig_on_volume;
	uint16_t trig_on_slice;
	uint16_t current_volume;
	uint16_t current_slice;
	uint16_t previous_volume;
	uint16_t previous_slice;
	uint16_t pulse_length;
	uint32_t TR_time;
}session;


extern struct menu {
	uint8_t previous_form;
	uint8_t actual_form;
	uint8_t actual_button;
	uint8_t previous_button;	
	}menu;


extern struct session_temp_values {
	uint16_t no_of_volumes;
	uint16_t no_of_slices;
	uint16_t trig_on_volume;
	uint16_t trig_on_slice;
	uint16_t pulse_length;
	uint32_t TR_time;
	uint8_t parameter;
}session_temp_values;

#endif /* GLOBAL_H_ */