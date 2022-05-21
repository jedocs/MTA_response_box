// ha mindkét nagyméretû hangfájl használva van, akkor az egyikben egy 256ms-os glitch lesz (65535-ös értékek)
// a compiler runtimeban a RAM-ba másolja és nem fér el?????
// az egyiket nem használva a jelenség megszûnik (konkrétan a mátrix olvasást kikommentelve (dc_out = stop_sound[STOP_SOUND_LENGTH - stop_sound_length];)).
// az egymáshoz képesti sorrendjüktõl (TIMER int-ben) függ, hogy melyiknél fordul elõ

// release buildnél nagyon nem mindegy az optimalizáció, van hogy egyáltalán nem mûködik a program
// release buildnél extra delayok kellenek, különben nem mûködik ami debugban igen

// 1.0.6 release-ben optimize for size opcióval lett fordítva, start/stop sound nélkül



// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// irq-ból nem jó az LCD soros kommunikáció!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


#include <atmel_start.h>
#include <global.h>

#include <sounddata.h>

static struct timer_task TIMER_0_task1, TIMER_0_task2;

static bool adc_read;
static bool pb_received;
static uint16_t adc_value;
static uint32_t kb_value;

static uint32_t minvalue;
static uint32_t maxvalue;

static uint16_t pulse_counter;
static uint16_t TR_counter;

char buffer[64];

extern struct report event_report;
extern struct session session_data;
extern struct menu menu_structure;
extern struct session_temp_values session_temp;

extern bool ACK_received;
extern bool NAK_received;
extern uint16_t usb_disconnected;

static uint8_t usb_disconnected_prescaler = 0;

const char switch_to_default_form[] =			{WRITE_OBJ,FORM,DEFAULT_FORM,0,0,WRITE_OBJ^FORM^DEFAULT_FORM};
const char switch_to_session_running_form[] =	{WRITE_OBJ,FORM,SESSION_RUNNING_FORM,0,0,WRITE_OBJ^FORM^SESSION_RUNNING_FORM};
const char switch_to_manual_trigger_form[] =	{WRITE_OBJ,FORM,MANUAL_TRIGGER_FORM,0,0,WRITE_OBJ^FORM^MANUAL_TRIGGER_FORM};
const char switch_to_simulation_running_form[] = {WRITE_OBJ,FORM,SIMULATION_RUNNING_FORM,0,0,WRITE_OBJ^FORM^SIMULATION_RUNNING_FORM};
const char switch_to_settings_1_form[] =		{WRITE_OBJ,FORM,SETTINGS_1_FORM,0,0,WRITE_OBJ^FORM^SETTINGS_1_FORM};
const char switch_to_settings_2_form[] =		{WRITE_OBJ,FORM,SETTINGS_2_FORM,0,0,WRITE_OBJ^FORM^SETTINGS_2_FORM};
const char switch_to_keyboard_form[] =			{WRITE_OBJ,FORM,KEYBOARD_FORM,0,0,WRITE_OBJ^FORM^KEYBOARD_FORM};
const char switch_to_menu_form[] =				{WRITE_OBJ,FORM,MENU_FORM,0,0,WRITE_OBJ^FORM^MENU_FORM};
const char switch_to_settings_3_form[] =		{WRITE_OBJ,FORM,SETTINGS_3_FORM,0,0,WRITE_OBJ^FORM^SETTINGS_3_FORM};
const char switch_to_about_form[] =				{WRITE_OBJ,FORM,ABOUT_FORM,0,0,WRITE_OBJ^FORM^ABOUT_FORM};
const char switch_to_error_form[] =				{WRITE_OBJ,FORM,ERROR_FORM,0,0,WRITE_OBJ^FORM^ERROR_FORM};
const char switch_to_help_form[] =				{WRITE_OBJ,FORM,HELP_FORM,0,0,WRITE_OBJ^FORM^HELP_FORM};


const char set_trigger_sound_off[] = {WRITE_OBJ,STRINGS,2,0,OFF,WRITE_OBJ^STRINGS^2^OFF};
const char set_trigger_sound_on[] = {WRITE_OBJ,STRINGS,2,0,ON,WRITE_OBJ^STRINGS^2^ON};
	
const char set_trigger_sound_switch_off[] = {WRITE_OBJ,FOURDBUTTON,0,0,OFF,WRITE_OBJ^FOURDBUTTON^0^OFF};
const char set_trigger_sound_switch_on[] = {WRITE_OBJ,FOURDBUTTON,0,0,ON,WRITE_OBJ^FOURDBUTTON^0^ON};

const char set_response_sound_off[] = {WRITE_OBJ,STRINGS,3,0,OFF,WRITE_OBJ^STRINGS^3^OFF};
const char set_response_sound_on[] = {WRITE_OBJ,STRINGS,3,0,ON,WRITE_OBJ^STRINGS^3^ON};
	
const char set_response_sound_switch_off[] = {WRITE_OBJ,FOURDBUTTON,1,0,OFF,WRITE_OBJ^FOURDBUTTON^1^OFF};
const char set_response_sound_switch_on[] = {WRITE_OBJ,FOURDBUTTON,1,0,ON,WRITE_OBJ^FOURDBUTTON^1^ON};
	
	
	
	
const char set_form1_trigger_sound_off[] = {WRITE_OBJ,STRINGS,20,0,OFF,WRITE_OBJ^STRINGS^20^OFF};
const char set_form1_trigger_sound_on[] = {WRITE_OBJ,STRINGS,20,0,ON,WRITE_OBJ^STRINGS^20^ON};

const char set_form1_trigger_sound_switch_off[] = {WRITE_OBJ,FOURDBUTTON,2,0,OFF,WRITE_OBJ^FOURDBUTTON^2^OFF};
const char set_form1_trigger_sound_switch_on[] = {WRITE_OBJ,FOURDBUTTON,2,0,ON,WRITE_OBJ^FOURDBUTTON^2^ON};

const char set_form1_response_sound_off[] = {WRITE_OBJ,STRINGS,21,0,OFF,WRITE_OBJ^STRINGS^21^OFF};
const char set_form1_response_sound_on[] = {WRITE_OBJ,STRINGS,21,0,ON,WRITE_OBJ^STRINGS^21^ON};

const char set_form1_response_sound_switch_off[] = {WRITE_OBJ,FOURDBUTTON,3,0,OFF,WRITE_OBJ^FOURDBUTTON^3^OFF};
const char set_form1_response_sound_switch_on[] = {WRITE_OBJ,FOURDBUTTON,3,0,ON,WRITE_OBJ^FOURDBUTTON^3^ON};
	
	
	
	
const char set_no_of_slices_txt[] =	{WRITE_OBJ,STRINGS,10,0,0,WRITE_OBJ^STRINGS^10^0};
const char set_no_of_volumes_txt[] =	{WRITE_OBJ,STRINGS,10,0,1,WRITE_OBJ^STRINGS^10^1};
const char set_trigg_slice_txt[] =		{WRITE_OBJ,STRINGS,10,0,2,WRITE_OBJ^STRINGS^10^2};
const char set_trigg_volume_txt[] =	{WRITE_OBJ,STRINGS,10,0,3,WRITE_OBJ^STRINGS^10^3};
const char set_pulse_length_txt[] =	{WRITE_OBJ,STRINGS,10,0,4,WRITE_OBJ^STRINGS^10^4};
const char set_tr_time_txt[] =			{WRITE_OBJ,STRINGS,10,0,5,WRITE_OBJ^STRINGS^10^5};

const char simulation_trigger_LED_on[] =	{WRITE_OBJ,USERLED,SIMULATION_TRIGGER_LED,0,ON,WRITE_OBJ^USERLED^SIMULATION_TRIGGER_LED^ON};
const char simulation_trigger_LED_off[] =	{WRITE_OBJ,USERLED,SIMULATION_TRIGGER_LED,0,OFF,WRITE_OBJ^USERLED^SIMULATION_TRIGGER_LED^OFF};
	
const char man_trig_trigger_LED_on[] =	{WRITE_OBJ,USERLED,MAN_TRIG_TRIGGER_LED,0,ON,WRITE_OBJ^USERLED^MAN_TRIG_TRIGGER_LED^ON};
const char man_trig_trigger_LED_off[] =	{WRITE_OBJ,USERLED,MAN_TRIG_TRIGGER_LED,0,OFF,WRITE_OBJ^USERLED^MAN_TRIG_TRIGGER_LED^OFF};

const char trigger_received_LED_on[] =	{WRITE_OBJ,USERLED,TRIGGER_RECEIVED_LED,0,ON,WRITE_OBJ^USERLED^TRIGGER_RECEIVED_LED^ON};
const char trigger_received_LED_off[] =	{WRITE_OBJ,USERLED,TRIGGER_RECEIVED_LED,0,OFF,WRITE_OBJ^USERLED^TRIGGER_RECEIVED_LED^OFF};
const char trigger_sent_LED_on[] =	{WRITE_OBJ,USERLED,TRIGGER_SENT_LED,0,ON,WRITE_OBJ^USERLED^TRIGGER_SENT_LED^ON};
const char trigger_sent_LED_off[] =	{WRITE_OBJ,USERLED,TRIGGER_SENT_LED,0,OFF,WRITE_OBJ^USERLED^TRIGGER_SENT_LED^OFF};

const char set_error_text_PS_high[] = {WRITE_OBJ,STRINGS,ERROR_STR,0,PS_VOLTAGE_HIGH,WRITE_OBJ^STRINGS^ERROR_STR^PS_VOLTAGE_HIGH};
const char set_error_text_PS_low[] = {WRITE_OBJ,STRINGS,ERROR_STR,0,PS_VOLTAGE_LOW,WRITE_OBJ^STRINGS^ERROR_STR^PS_VOLTAGE_LOW};

const char set_error_text_USB_error[] = {WRITE_OBJ,STRINGS,ERROR_STR,0,USB_DISCONNECTED,WRITE_OBJ^STRINGS^ERROR_STR^USB_DISCONNECTED};

const uint32_t ad = 0x3e000;

static uint32_t timeout_counter = 0;
static uint8_t prescaler = PRESCALER_VALUE;


static uint16_t error_beep_length = 0;
static uint16_t click_length = 0;
static uint16_t start_sound_length = 0;
static uint16_t stop_sound_length = 0;
extern uint16_t beep_sound_length;
extern uint16_t sync_beep_length;

static uint16_t ps_error_counter = 0;

static uint16_t dc_out = 511;
static uint16_t trig_LED_delay = TRIGGER_LED_DELAY;
static uint16_t trig_sent_LED_delay = TRIGGER_LED_DELAY;
static uint16_t trigger_debounce = TRIGGER_DEBOUNCE_TIME;
static uint16_t pb_debounce = PB_DEBOUNCE_TIME;



void write_flash(){
	uint8_t delay = 10;
	
	int i;
	i = session_data.no_of_slices;
	unsigned char * ic = (unsigned char *) &i;  //https://stackoverflow.com/questions/3609972/converting-int-double-to-char-array-in-c-or-objective-c
	printf("flash write no of slices, %u\r\n", i);
	flash_write(&FLASH_0, ad, ic, 2);
	printf("flash written no of slices, %u, %u\r\n", ic[0], ic[1]);
	delay_ms(delay);
		
	i = session_data.no_of_volumes;
	ic = (unsigned char *) &i;
	printf("flash write no of volumes, %u\r\n", i);
	flash_write(&FLASH_0, ad + 2, ic, 2);
	printf("flash written no of volumes, %u, %u\r\n", ic[0], ic[1]);
	delay_ms(delay);
	
	i = session_data.trig_on_slice;
	ic = (unsigned char *) &i;
	printf("flash write trigg on slice, %u\r\n", i);
	flash_write(&FLASH_0, ad + 4, ic, 2);
	printf("flash written trigg on slice, %u, %u\r\n", ic[0], ic[1]);
	delay_ms(delay);
	
	i = session_data.trig_on_volume;
	ic = (unsigned char *) &i;
	printf("flash write trigg on volume, %u\r\n", i);
	flash_write(&FLASH_0, ad + 6, ic, 2);
	printf("flash written trigg on volume, %u, %u\r\n", ic[0], ic[1]);
	delay_ms(delay);
	
	i = session_data.pulse_length;
	ic = (unsigned char *) &i;
	printf("flash write pulse length, %u\r\n", i);
	flash_write(&FLASH_0, ad + 8, ic, 2);
	printf("flash written pulse lenght, %u, %u\r\n", ic[0], ic[1]);
	delay_ms(delay);

	uint32_t li;
	li = session_data.TR_time;
	unsigned char * lic = (unsigned char *) &li;
	printf("flash write TR time, %lu\r\n", li);
	flash_write(&FLASH_0, ad + 0x0a, lic, 4);
	printf("flash written TR time, %u, %u, %u, %u\r\n", lic[0], lic[1], lic[2], lic[3]);
	delay_ms(delay);	
	
	i = session_data.trigger_sound * 256 + session_data.response_sound;
	ic = (unsigned char *) &i;
	printf("flash write trigger / response sound, %u\r\n", i);
	flash_write(&FLASH_0, ad + 0x0e, ic, 2);
	printf("flash written trigger / response sound, %u, %u\r\n", ic[1], ic[0]);
	delay_ms(delay);
	
	i = session_data.sound_volume;
	ic = (unsigned char *) &i;
	printf("flash write sound volume, %u\r\n", i);
	flash_write(&FLASH_0, ad + 0x10, ic, 2);
	printf("flash written sound volume, %u, %u\r\n", ic[0], ic[1]);
	delay_ms(delay);
}

void read_flash(){
		
	static uint8_t a[4];
	printf("read session data from flash\r\n");
		
	flash_read(&FLASH_0, ad, a, 2);	
	session_data.no_of_slices = a[0] + a[1] * 256;	
	printf("no_of_slices read from flash: %u\r\n", session_data.no_of_slices);	
	if ((session_data.no_of_slices < 1) || (session_data.no_of_slices > 9999)) {
		session_data.no_of_slices = 25;
	}	
	
	flash_read(&FLASH_0, ad + 2, a, 2);
	session_data.no_of_volumes = a[0] + a[1] * 256;
	printf("no_of_volumes read from flash: %u\r\n", session_data.no_of_volumes);	
	if ((session_data.no_of_volumes < 1) || (session_data.no_of_volumes > 9999)) {
		session_data.no_of_volumes = 100;
	}
	
	flash_read(&FLASH_0, ad + 4, a, 2);
	session_data.trig_on_slice = a[0] + a[1] * 256;
	printf("trig_on_slice read from flash: %u\r\n", session_data.trig_on_slice);	
	if ((session_data.trig_on_slice < 0) || (session_data.trig_on_slice > 9999)) {
		session_data.trig_on_slice = 0;
	}
			
	flash_read(&FLASH_0, ad + 6, a, 2);
	session_data.trig_on_volume = a[0] + a[1] * 256;
	printf("trig_on_volume read from flash: %u\r\n", session_data.trig_on_volume);	
	if ((session_data.trig_on_volume < 0) || (session_data.trig_on_volume > 9999)) {
		session_data.trig_on_volume = 0;
	}
		
	flash_read(&FLASH_0, ad + 8, a, 2);
	session_data.pulse_length = a[0] + a[1] * 256;
	printf("pulse_length read from flash: %u\r\n", session_data.pulse_length);	
	if ((session_data.pulse_length < 100) || (session_data.pulse_length > 999)) {
		session_data.pulse_length = 100;
	}
		
	flash_read(&FLASH_0, ad + 0x0a, a, 4);
	session_data.TR_time = a[0] + a[1] * 256 + a[2] * 65536 + a[3] * 16777216;
	printf("TR_time read from flash: %lu\r\n", session_data.TR_time);	
	uint32_t minvalue_long = session_data.pulse_length * session_data.no_of_slices;
	if (minvalue_long > 9999999){   // hibaellenõrzés!!!!!!!!!!!!
		minvalue_long = 9999999;
	}
	if (session_data.TR_time < minvalue_long) {  
		session_data.TR_time = minvalue_long;
	}
	
	if (session_data.TR_time > 9999999) {
		session_data.TR_time = 9999999;
	}
	
		
	flash_read(&FLASH_0, ad + 0x0e, a, 2);
	if (a[1] > 0){
		session_data.trigger_sound = true;
	}
	else{
		session_data.trigger_sound = false;
	}	
	
	if (a[0] > 0){
		session_data.response_sound = true;
	}
	else{
		session_data.response_sound = false;
	}
	printf("trigger / response sound: %u, %u\r\n", session_data.trigger_sound, session_data.response_sound);		
	
	flash_read(&FLASH_0, ad + 0x10, a, 2);
	session_data.sound_volume = a[0];
	printf("sound volume read from flash: %u\r\n", session_data.sound_volume);	
	if ((session_data.sound_volume < 0) || (session_data.sound_volume > 127)) {
		session_data.sound_volume = 64;
	}
}

void resync(){
	printf("no ACK received, resync");
}

void io_write_clear_ack(const char *const buf, const unsigned char length)
{
	unsigned char i;
	if (ACK_received){
		ACK_received = false;
		
		for (i = 0; i < length; i++)
		{
			io_write(&USART_1.io, &buf[i], 1);
			while(usart_async_is_tx_empty(&USART_1)!=1);  // waiting the tx is over		
		}
	}
	else{
		printf("start timeout\r\n");
		timeout_counter = ACK_TIMEOUT;
		while (timeout_counter){
			delay_us(100);    		// delay nélkül release-ban nem mûködik, csak debug-ban!!!!!
				if (ACK_received){
				break;
			}
		}
		if (ACK_received){
			printf("received an ACK finally...\r\n");
			printf("timeout counter: %lu\r\n", (ACK_TIMEOUT - timeout_counter));
			printf("send command\r\n");
			io_write_clear_ack(buf, length);
		}
		else{
			resync();
		}
	}
}

unsigned char getCRC(char message[], unsigned char length)
{
	unsigned char i, crc = 0;
	
	for (i = 0; i < length; i++)
	{
		crc ^= message[i];
	}
	return crc;
}

static void print_version(){
	
	unsigned char length = snprintf(buffer, 25, "___version: %s", VERSION);
	
	buffer[0] = WRITE_STR;
	buffer[1] = VERSION_STR;
	buffer[2] = length-3;
	buffer[length] = getCRC(buffer, length);
	buffer[length+1] = 0; // ????????????
	io_write_clear_ack(buffer, length+1);
	printf("version: %s\r\n", VERSION);
}

static void print_no(uint32_t number, uint8_t str_no){
	
	unsigned char length = snprintf(buffer, 25, "%lu", number);	
	unsigned char pad = (7-length);
	if ((pad<0) || (pad>6)){
		pad = 0;
	}
	
	length = snprintf(buffer, 25, "% *s   %lu", pad, "", number);   // pad to center align
	
	buffer[0] = WRITE_STR;
	buffer[1] = str_no;
	buffer[2] = length-3;
	buffer[length] = getCRC(buffer, length);
	buffer[length+1] = 0; // ????????????
    io_write_clear_ack(buffer, length+1);
	printf("number: %lu\r\n", number);
}

static void print_slices(uint8_t str_no){
	uint8_t length = snprintf(buffer, 64, "___%u / %u    ", session_data.current_slice, session_data.no_of_slices);
	buffer[0] = WRITE_STR;
	buffer[1] = str_no;
	buffer[2] = length-3;
	buffer[length] = getCRC(buffer, length);
	buffer[length+1] = 0; // ????????????
	io_write_clear_ack(buffer,length+1);
	printf("current slice: %u\r\n", session_data.current_slice);
}

static void print_volumes(uint8_t str_no){
	uint8_t length = snprintf(buffer, 64, "___%u / %u    ", session_data.current_volume, session_data.no_of_volumes);
	buffer[0] = WRITE_STR;
	buffer[1] = str_no;
	buffer[2] = length-3;
	buffer[length] = getCRC(buffer, length);
	buffer[length+1] = 0; // ????????????
	io_write_clear_ack(buffer,length+1);
	printf("**********current volume: %u\r\n", session_data.current_volume);
}

static void convert_cb_ADC_0(const struct adc_async_descriptor *const descr, const uint8_t channel)
{
	adc_async_read_channel(&ADC_0, channel, &adc_value, 2);
	adc_read=true;
}

/**
 * ADC init
 */
void ADC_init(void)
{
	adc_async_register_callback(&ADC_0, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_0);
	adc_async_enable_channel(&ADC_0, 0);
}

static void set_prev_curr_form(const uint8_t form){
	session_data.previous_form = session_data.current_form;
	session_data.current_form = form;
}

static void switch_to_form(const uint8_t form)
{		
	switch (form){
				
		case DEFAULT_FORM:
			printf("sw to default form\r\n");
			io_write_clear_ack(switch_to_default_form,6);
			set_prev_curr_form(form);			
		break;
		
		case SESSION_RUNNING_FORM:
			printf("sw to ses running form\r\n");
			io_write_clear_ack(switch_to_session_running_form,6);
			set_prev_curr_form(form);
			
			if (session_data.response_sound){
				printf("setting response sound switch and text on\r\n");
				io_write_clear_ack(set_form1_response_sound_on,6);
				io_write_clear_ack(set_form1_response_sound_switch_on,6);
			}
			else{
				printf("setting response sound switch and text off\r\n");
				io_write_clear_ack(set_form1_response_sound_off,6);
				io_write_clear_ack(set_form1_response_sound_switch_off,6);
			}
			
			if (session_data.trigger_sound){
				printf("setting trigger sound switch and text on\r\n");
				io_write_clear_ack(set_form1_trigger_sound_on,6);
				io_write_clear_ack(set_form1_trigger_sound_switch_on,6);
			}
			else{
				printf("setting trigger sound switch and text off\r\n");
				io_write_clear_ack(set_form1_trigger_sound_off,6);
				io_write_clear_ack(set_form1_trigger_sound_switch_off,6);
			}
			
			
		break;	
		
		case MANUAL_TRIGGER_FORM:
			printf("sw to man trig form\r\n");
			io_write_clear_ack(switch_to_manual_trigger_form,6);
			set_prev_curr_form(form);
			//io_write_clear_ack(man_trig_trigger_LED_off,6);
			break;
			
			case SIMULATION_RUNNING_FORM:
			printf("sw to sim running form\r\n");
			io_write_clear_ack(switch_to_simulation_running_form,6);
			set_prev_curr_form(form);
			//io_write_clear_ack(simulation_trigger_LED_off,6);			
		break;
				
		case SETTINGS_1_FORM:
			printf("switch to settings 1 form\r\n");
			io_write_clear_ack(switch_to_settings_1_form,6);
			set_prev_curr_form(form);
						
			if (session_data.response_sound){
				printf("setting response sound switch and text on\r\n");
				io_write_clear_ack(set_response_sound_on,6);
				io_write_clear_ack(set_response_sound_switch_on,6);
			}
			else{
				printf("setting response sound switch and text off\r\n");
				io_write_clear_ack(set_response_sound_off,6);
				io_write_clear_ack(set_response_sound_switch_off,6);
			}
						
			if (session_data.trigger_sound){
				printf("setting trigger sound switch and text on\r\n");
				io_write_clear_ack(set_trigger_sound_on,6);
				io_write_clear_ack(set_trigger_sound_switch_on,6);
			}
			else{
				printf("setting trigger sound switch and text off\r\n");
				io_write_clear_ack(set_trigger_sound_off,6);
				io_write_clear_ack(set_trigger_sound_switch_off,6);
			}
			
				printf("setting trig on slice string\r\n");
				print_no(session_temp.trig_on_slice, SLICE_TRIGGERNO_STR);
				printf("setting trig on volume string\r\n");
				print_no(session_temp.trig_on_volume, VOLUME_TRIGGERNO_STR);
			
		break;
		
		case SETTINGS_2_FORM:
			printf("sw to settings 2 form\r\n");
			io_write_clear_ack(switch_to_settings_2_form,6);
			set_prev_curr_form(form);					
						
			printf("setting no of volumes string\r\n");
			print_no(session_temp.no_of_volumes, SET_VOLUMES_STR);
			printf("setting no of slices string\r\n");
			print_no(session_temp.no_of_slices, SET_SLICES_STR);
			printf("setting pulse length string\r\n"); 
 			print_no(session_temp.pulse_length, PULSE_LENGTH_STR);
			printf("setting TR time string\r\n");
 			print_no(session_temp.TR_time, TR_TIME_STR);
		break;
		
		case SETTINGS_3_FORM:
			printf("sw to settings 3 form\r\n");
			io_write_clear_ack(switch_to_settings_3_form,6);
			set_prev_curr_form(form);
		break;
				
		case KEYBOARD_FORM:
			printf("sw to keyboard form\r\n");
			io_write_clear_ack(switch_to_keyboard_form,6);
			set_prev_curr_form(form);
		break;
		
		case MENU_FORM:
			printf("sw to menu form\r\n");
			io_write_clear_ack(switch_to_menu_form,6);
			set_prev_curr_form(form);
		break;		
		
		case ABOUT_FORM:
			printf("sw to about form\r\n");
			io_write_clear_ack(switch_to_about_form,6);
			print_version();
			set_prev_curr_form(form);
		break;
		
		case ERROR_FORM:
			printf("sw to error form\r\n");
			
				gpio_set_pin_level(T1, false);
				gpio_set_pin_level(T2, false);
				gpio_set_pin_level(T3, false);
				gpio_set_pin_level(T4, false);
				gpio_set_pin_level(T5, false);
				gpio_set_pin_level(T6, false);
				gpio_set_pin_level(RX_PWR, false);
				
				session_data.session_running = false;
				session_data.simulation_mode = false;
				session_data.manual_trigger=false;
				session_data.trigger = false;
				session_data.current_volume = 1;
				session_data.current_slice = 0;
			
			
			io_write_clear_ack(switch_to_error_form,6);
			set_prev_curr_form(form);
			error_beep_length = ERROR_BEEP_LENGTH;
		break;

		case HELP_FORM:
			printf("sw to help form\r\n");
			io_write_clear_ack(switch_to_help_form,6);
			set_prev_curr_form(form);
		break;

	}	
}


void ps_error(){

	switch_to_form(ERROR_FORM);
	
	if (adc_value < 1550){
		io_write_clear_ack(set_error_text_PS_low,6);
		printf("!!!!!!!!!!!!!!!!!!!");
		printf("ps voltage too low");
	}
	
	if (adc_value > 2000){
		io_write_clear_ack(set_error_text_PS_high,6);
		printf("!!!!!!!!!!!!!!!!!!!");
		printf("ps voltage too high");
	}
	printf("psup voltage is %d\r\n", adc_value);
}

void usb_error(){
	switch_to_form(ERROR_FORM);	
	
	io_write_clear_ack(set_error_text_USB_error,6);
	printf("!!!!!!!!!!!!!!!!!!!\r\n");
	printf("USB error\r\n");
	
	while (usb_disconnected){
		delay_ms(1);		// delay nélkül release-ban nem mûködik, csak debug-ban!!!!!
	}
	
	printf("USB connection ok\r\n");
	switch_to_form(DEFAULT_FORM);
}

void start_simulation()
{
	printf("starting simulation\r\n");
	
	pulse_counter = 0;
	TR_counter = 0;
	
	session_data.trig_LED_on = false;
	session_data.trigger = false;
	session_data.current_volume = 1;
	session_data.current_slice = 0;
	session_data.previous_volume = session_data.no_of_volumes;
	session_data.previous_slice = 0;
	session_data.simulated_A=false;
	session_data.simulated_B=false;
	session_data.simulated_C=false;
	session_data.simulated_D=false;
	session_data.manual_trigger=false;
	session_data.session_running = false;
	session_data.session_finished = false;
	switch_to_form(SIMULATION_RUNNING_FORM);
	printf("setting current slice string\r\n");
	print_slices(SIMULATION_CURRENT_SLICE_STR);
	printf("setting current volume string\r\n");	
	print_volumes(SIMULATION_CURRENT_VOLUME_STR);
	
	printf("playing start sound\r\n");
	start_sound_length = START_SOUND_LENGTH;
	delay_ms(1000);
	session_data.simulation_mode=true;	
}

void stop_simulation()
{
	printf("stopping simulation\r\n");
	pb_received = false;
	stop_sound_length = STOP_SOUND_LENGTH;
	io_write_clear_ack(simulation_trigger_LED_off,6);
	switch_to_form(MENU_FORM);

	session_data.session_running = false;
	session_data.simulation_mode = false;
	session_data.manual_trigger=false;
	session_data.trigger = false;
	session_data.current_volume = 1;
	session_data.current_slice = 0;
}

void start_man_trig()
{
	printf("starting manual trigger\r\n");
	
	session_data.trig_LED_on = false;
	session_data.trigger = false;
	session_data.current_volume = 1;
	session_data.current_slice = 0;
	session_data.previous_volume = session_data.no_of_volumes;
	session_data.previous_slice = 0;
	session_data.simulated_A=false;
	session_data.simulated_B=false;
	session_data.simulated_C=false;
	session_data.simulated_D=false;
	session_data.simulation_mode=false;
	session_data.session_running= false;
	session_data.session_finished = false;	
		
	switch_to_form(MANUAL_TRIGGER_FORM);
	printf("setting current slice string\r\n");
	print_slices(MAN_TRIG_CURRENT_SLICE_STR);
	printf("setting current volume string\r\n");
	print_volumes(MAN_TRIG_CURRENT_VOLUME_STR);

	printf("playing start sound\r\n");
	start_sound_length = START_SOUND_LENGTH;
	session_data.manual_trigger=true;	
}

void stop_man_trig()
{
	printf("stopping manual trigger\r\n");
	pb_received = false;
	stop_sound_length = STOP_SOUND_LENGTH;
	io_write_clear_ack(man_trig_trigger_LED_off,6);
	switch_to_form(MENU_FORM);

	session_data.session_running = false;
	session_data.simulation_mode = false;
	session_data.manual_trigger=false;
	session_data.trigger = false;
	session_data.current_volume = 1;
	session_data.current_slice = 0;
}

void start_session()
{
	if ((adc_value > 1550) && (adc_value < 2000)) {
			
		printf("starting session\r\n");
		
		session_data.trig_LED_on = false;
		session_data.trig_sent_LED_on = false;
		session_data.trigger = false;
		session_data.current_volume = 1;
		session_data.current_slice = 0;
		session_data.previous_volume = session_data.no_of_volumes;
		session_data.previous_slice = 0;
		session_data.simulated_A=false;
		session_data.simulated_B=false;
		session_data.simulated_C=false;
		session_data.simulated_D=false;
		session_data.simulation_mode=false;
		session_data.session_finished = false;
	
		switch_to_form(SESSION_RUNNING_FORM);
	
		printf("enable RX pwr\r\n");
		gpio_set_pin_level(RX_PWR, true);
		
		printf("powering up TX ch. 1\r\n");
		gpio_set_pin_level(T1, true);
	
		printf("powering up TX ch. 2\r\n");
		gpio_set_pin_level(T2, true);
	
		io_write_clear_ack(trigger_received_LED_off,6);
		io_write_clear_ack(trigger_sent_LED_off,6);

		printf("setting current slice string\r\n");
		print_slices(CURRENT_SLICE_STR);
		printf("setting current volume string\r\n");
		print_volumes(CURRENT_VOLUME_STR);
	
		printf("playing start sound\r\n");
		start_sound_length = START_SOUND_LENGTH;
		session_data.session_running = true;
	}
	else {
		ps_error();
	}
}

void stop_session()
{
	printf("stopping session\r\n");
	stop_sound_length = STOP_SOUND_LENGTH;
	switch_to_form(DEFAULT_FORM);
	
	session_data.session_running = false;
	session_data.simulation_mode = false;
	session_data.trigger = false;
	session_data.current_volume = 1;
	session_data.current_slice = 0;
	
	printf("powering down TX ch. 1\r\n");
	gpio_set_pin_level(T1, false);
	printf("powering down TX ch. 2\r\n");
	gpio_set_pin_level(T2, false);	
	printf("disable RX pwr\r\n");
	gpio_set_pin_level(RX_PWR, false);
}

static void button(void)	// called by ext irq
{
	if (!pb_debounce){
	printf("button pressed\r\n");
	pb_debounce = PB_DEBOUNCE_TIME;
	pb_received = true;
	}
}

static void sync_trigger(void)	// called by ext irq
{	
	if ((session_data.session_running) && (!trigger_debounce)){
		trigger_debounce = TRIGGER_DEBOUNCE_TIME;
		session_data.current_slice += 1;
		
		if (((session_data.current_slice >= session_data.no_of_slices) && (session_data.current_volume == session_data.no_of_volumes)) ||
			(session_data.current_volume > session_data.no_of_volumes)){
			session_data.session_finished = true;
		}
		
		if ((session_data.current_slice > session_data.no_of_slices) && (!session_data.session_finished)){
			session_data.current_volume += 1;	
			session_data.current_slice = 1;					
			}
		
		uint16_t divider = session_data.trig_on_volume;
		if (divider == 0) {
			divider = 1;
		}
		if ((session_data.trig_on_volume == 0) || (((session_data.current_volume - 1) % divider) == 0)){
			if ((session_data.trig_on_slice == 0) || (session_data.trig_on_slice == session_data.current_slice)){
				session_data.trigger = true;
			}
		}
	}	
}

static void TIMER_task1_cb(const struct timer_task *const timer_task)   // 8kHz
   {	
	if (error_beep_length){
		dc_out = error_beep_sound_data[ERROR_BEEP_LENGTH - error_beep_length];
 		dac_async_write(&DAC_0, 0, &dc_out, 1);
 		error_beep_length --;
	}
	
	if (click_length){
		dc_out = click_sound_data[CLICK_SOUND_LENGTH - click_length];
		dac_async_write(&DAC_0, 0, &dc_out, 1);
		click_length --;
		}
		
	if (beep_sound_length){
		if (session_data.response_sound){
			dc_out = beep_sound_data[BEEP_SOUND_LENGTH - beep_sound_length];
			dac_async_write(&DAC_0, 0, &dc_out, 1);
			beep_sound_length --;
		}
		else{
			beep_sound_length = 0;
		}
	}
	
	if (sync_beep_length){
		if (session_data.trigger_sound){
			dc_out = sync_beep_sound_data[SYNC_BEEP_LENGTH - sync_beep_length];
			dac_async_write(&DAC_0, 0, &dc_out, 1);
			sync_beep_length --;
		}
		else{
			sync_beep_length = 0;
 		}
 	}
	
// 	else if (start_sound_length){
// 		dc_out = start_sound_data[START_SOUND_LENGTH - start_sound_length];
// 		dac_async_write(&DAC_0, 0, &dc_out, 1);
// 		start_sound_length --;	
// 	else if (stop_sound_length){
// 		dc_out = stop_sound_data[STOP_SOUND_LENGTH - stop_sound_length];
// 		dac_async_write(&DAC_0, 0, &dc_out, 1);
// 		stop_sound_length --;
// 	}
	
	
	if (prescaler){
		prescaler --;
	}
	else{  // divided to 1kHz
		prescaler = PRESCALER_VALUE;
		
		if (usb_disconnected_prescaler){
			usb_disconnected_prescaler --;
		}
		else {
			//printf("prescaler\r\n");
			usb_disconnected_prescaler = 5;
			if (usb_disconnected < 500){
				usb_disconnected ++;
				//printf("disc: %u\r\n", usb_disconnected);
			}
		}
		if (timeout_counter){
			timeout_counter --;
		}
		
		if (trigger_debounce){
			trigger_debounce --;
		}
		
		if (trig_LED_delay){
			trig_LED_delay --;
		}

		if (trig_sent_LED_delay){
			trig_sent_LED_delay --;
		}
		
		if (pb_debounce){
			pb_debounce --;
		}
		
		//gpio_toggle_pin_level(T1);
	
		if (session_data.simulation_mode){		
			pulse_counter += 1;
			TR_counter += 1;
		
			if (pulse_counter >= session_data.pulse_length){ // slice acquisition starts
				pulse_counter = 0;
				if (session_data.current_slice < session_data.no_of_slices){
					session_data.current_slice += 1;
					
					
					if (((session_data.current_slice >= session_data.no_of_slices) && (session_data.current_volume == session_data.no_of_volumes)) || 
						(session_data.current_volume > session_data.no_of_volumes)){
						session_data.session_finished = true;
					}
										
					uint16_t divider = session_data.trig_on_volume;
					if (divider == 0) {
						divider = 1;
					}
					if ((session_data.trig_on_volume == 0) || (((session_data.current_volume - 1) % divider) == 0)){
						if ((session_data.trig_on_slice == 0) || (session_data.trig_on_slice == session_data.current_slice)){
							session_data.trigger = true;
							sync_beep_length = SYNC_BEEP_LENGTH;
						}
					}
				}
			}
			if (TR_counter >= session_data.TR_time){ // new volume acquisition starts
				TR_counter = 0;
				pulse_counter = 0;
				session_data.current_volume += 1;
				if (session_data.current_volume > session_data.no_of_volumes){
					session_data.session_finished = true;
				}
				session_data.current_slice = 0;
			}
		}
		//gpio_toggle_pin_level(T1);
	}
}

static void TIMER_task2_cb(const struct timer_task *const timer_task)  // 1.6Hz
{
	//gpio_toggle_pin_level(T2);
	if (adc_read){
		adc_read=false;
		//printf("ADC value: %d\r\n", adc_value);
		adc_async_start_conversion(&ADC_0);
	}
}

void TIMER_init(void)
{
	TIMER_0_task1.interval = 1; // 8kHz
	TIMER_0_task1.cb       = TIMER_task1_cb;
	TIMER_0_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_0_task2.interval = 5000;   //1.6 Hz
	TIMER_0_task2.cb       = TIMER_task2_cb;
	TIMER_0_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_0, &TIMER_0_task1);
	timer_add_task(&TIMER_0, &TIMER_0_task2);
	timer_start(&TIMER_0);
}

int main(void)
{
	pb_received = false;
	uint16_t page_size;
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	gpio_set_pin_level(T1, false);
	gpio_set_pin_level(T2, false);
	gpio_set_pin_level(T3, false);
	gpio_set_pin_level(T4, false);
	gpio_set_pin_level(T5, false);
	gpio_set_pin_level(T6, false);
	gpio_set_pin_level(RX_PWR, false);
	
	
	ADC_init();
	adc_read=true;
	TIMER_init();
	uart_init();  // LCD communication
		
	page_size = flash_get_page_size(&FLASH_0);	
	printf("\r\nFLASH page size: %d\r\n", page_size);
	
	page_size=sizeof(session_data);
	printf("\r\nsession data size: %d\r\n", page_size);
	
	page_size=sizeof(session_data.sound_volume);
	printf("\r\nsound volume size: %d\r\n", page_size);
	
	page_size=sizeof(session_data.response_sound);
	printf("\r\nresponse sound size: %d\r\n", page_size);
	
	page_size=sizeof(session_data.current_form);
	printf("\r\ncurrent form size: %d\r\n", page_size);	
	
	printf("reading settings from FLASH memory");
	read_flash();
	
	session_data.trigger = false;
	session_data.session_running = false;
	session_data.session_finished = false;
	session_data.simulation_mode = false;
	session_data.manual_trigger = false;
	session_data.current_form = DEFAULT_FORM;
	session_data.previous_form = DEFAULT_FORM;
	session_data.simulated_A = false;
	session_data.simulated_B = false;
	session_data.simulated_C = false;
	session_data.simulated_D = false;

	printf("enabling DAC\r\n");
	dac_async_enable_channel(&DAC_0, 0);
	
	printf("register ext irq sync trigger\r\n");
    ext_irq_register(I0, sync_trigger);
	
	printf("register ext irq button\r\n");
    ext_irq_register(PB2, button);
		
	printf("reset LCD\r\n");
	gpio_set_pin_level(DISP_RESET, true);
	delay_ms(2000);
	printf("set ACK after display reset\r\n");
	ACK_received = true;
	
	printf("enabling audio amp\r\n");
	gpio_set_pin_level(AUDENB, true);

	printf("***************************************\r\n");
	printf("initialization done, starting main loop\r\n");
	usb_disconnected = 0;
	while (1) {		
		
		if ((adc_value < 1500) || (adc_value > 2000)) {
			ps_error_counter ++;
			printf("ps dip, voltage: %d, counter: %u\r\n", adc_value, ps_error_counter);
		}
		else{
			ps_error_counter = 0;
		}
		if (ps_error_counter > 3){
			ps_error();
		}
		
		
		if (usb_disconnected > 100){
			printf("USB disconnected!\r\n");
			usb_error();
		}
		
		

		
		if (session_data.session_running) {		

			if (session_data.trigger){
				if (!session_data.trig_sent_LED_on){
					session_data.trig_sent_LED_on = true;
					printf("trigger sent LED on\r\n");
					io_write_clear_ack(trigger_sent_LED_on,6);
					trig_sent_LED_delay = TRIGGER_LED_DELAY;
				}
			}
			
			if (session_data.previous_slice != session_data.current_slice){
				session_data.previous_slice = session_data.current_slice;
				if (!session_data.trig_LED_on){
					session_data.trig_LED_on = true;
					printf("trigger received LED on\r\n");
					io_write_clear_ack(trigger_received_LED_on,6);
					trig_LED_delay = TRIGGER_LED_DELAY;
				}				
				print_slices(CURRENT_SLICE_STR);

			}

			if (session_data.previous_volume != session_data.current_volume){
				session_data.previous_volume = session_data.current_volume;				
				print_volumes(CURRENT_VOLUME_STR);			
			}
			
			if ((!trig_LED_delay) && (session_data.trig_LED_on)){
				session_data.trig_LED_on = false;
				printf("trigger received LED off\r\n");
				io_write_clear_ack(trigger_received_LED_off,6);
				
			}

			if ((!trig_sent_LED_delay) && (session_data.trig_sent_LED_on)){
				session_data.trig_sent_LED_on = false;
				printf("trigger sent LED off\r\n");
				io_write_clear_ack(trigger_sent_LED_off,6);
	
			}

			if (session_data.session_finished) {
				session_data.session_running = false;
				delay_ms(2000);
				stop_session();
			}
		}
		
		else if (session_data.manual_trigger) {
			
			if (pb_received){
				pb_received = false;
				if ((!session_data.simulation_mode) && (!session_data.session_running) && (session_data.manual_trigger) && (!trigger_debounce)){
					trigger_debounce = TRIGGER_DEBOUNCE_TIME;
					printf("simulated S\r\n");
					session_data.current_slice += 1;
					
					if (((session_data.current_slice >= session_data.no_of_slices) && (session_data.current_volume == session_data.no_of_volumes)) ||
					(session_data.current_volume > session_data.no_of_volumes)){
						session_data.session_finished = true;
					}
					
					if ((session_data.current_slice > session_data.no_of_slices) && (!session_data.session_finished)){
						session_data.current_volume += 1;
						session_data.current_slice = 1;
					}
					
					uint16_t divider = session_data.trig_on_volume;
					if (divider == 0) {
						divider = 1;
					}
									
					if ((session_data.trig_on_volume == 0) || (((session_data.current_volume - 1) % divider) == 0)){
						if ((session_data.trig_on_slice == 0) || (session_data.trig_on_slice == session_data.current_slice)){
							session_data.trigger = true;
							sync_beep_length = SYNC_BEEP_LENGTH;
							if (!session_data.trig_LED_on){
								session_data.trig_LED_on = true;
								printf("switch trigger LED on\r\n");
								io_write_clear_ack(man_trig_trigger_LED_on,6);
								trig_LED_delay = TRIGGER_LED_DELAY;
							}
						}
					}
				}
			}
			
			if ((!trig_LED_delay) && (session_data.trig_LED_on)){
				session_data.trig_LED_on = false;
				printf("man trig switch trigger LED off\r\n");
				io_write_clear_ack(man_trig_trigger_LED_off,6);
				
			}
			
			if (session_data.previous_slice != session_data.current_slice){
				session_data.previous_slice = session_data.current_slice;
				
				print_slices(MAN_TRIG_CURRENT_SLICE_STR);
			}
			
			if (session_data.previous_volume != session_data.current_volume){
				session_data.previous_volume = session_data.current_volume;
				
				print_volumes(MAN_TRIG_CURRENT_VOLUME_STR);
			}
			
			if (session_data.session_finished) {
				session_data.manual_trigger = false;
				delay_ms(2000);
				stop_man_trig();
			}
		}
		
		else if (session_data.simulation_mode) {
			
			if ((session_data.trigger) && (!session_data.trig_LED_on)){
				session_data.trig_LED_on = true;
				printf("simulation switch trigger LED on\r\n");
				io_write_clear_ack(simulation_trigger_LED_on,6);
				trig_LED_delay = TRIGGER_LED_DELAY;
			}			
			
			if ((!trig_LED_delay) && (session_data.trig_LED_on)){
				session_data.trig_LED_on = false;
				printf("simulation switch trigger LED off\r\n");
				io_write_clear_ack(simulation_trigger_LED_off,6);				
			}
			
			if (session_data.previous_slice != session_data.current_slice){
				session_data.previous_slice = session_data.current_slice;
				print_slices(SIMULATION_CURRENT_SLICE_STR);
			}
			
			if (session_data.previous_volume != session_data.current_volume){
				session_data.previous_volume = session_data.current_volume;
				print_volumes(SIMULATION_CURRENT_VOLUME_STR);			
			}			
	
			if (session_data.session_finished) {
				session_data.simulation_mode = false;
				delay_ms(2000);
				stop_simulation();
			}
		}
		
		if ((pb_received) && (!session_data.simulation_mode) && (!session_data.manual_trigger)){
			pb_received = false;
			if (session_data.session_running){
				printf("stop session by pushbutton\r\n");
				stop_session();
			}
			else{
				printf("start session by pushbutton\r\n");
				start_session();
			}
		}
				
		if (event_report.report_valid){			
			event_report.report_valid=false;
	
			switch (event_report.object_id) {
				case WINBUTTON:			
					
					printf("\r\nwinbutton pressed, index: %u\r\n", event_report.object_index);
					
					if ((event_report.object_index < STOP_SIMULATION) && (event_report.object_index > MAN_TRIG_BACK)){ //avoid doubleclick with 'Both' action buttons
						if (event_report.value_lsb == 1){
							click_length = CLICK_SOUND_LENGTH;
						}
					}
					
					else {
						click_length = CLICK_SOUND_LENGTH;
					}
									
					switch (event_report.object_index) {
						
						// **************** form0
						case START_SESSION:		
							
							start_session();
							
						break;
						
						case MENU:
							switch_to_form(MENU_FORM);
							printf("switching to form 7\r\n");
						
						break;
						
						
						// **************** form1
						case STOP_SESSION:
							stop_session();
							
						break;
						
						
						// **************** form2
						case MAN_TRIG_SEND_A:
							if (event_report.value_lsb == ON){
								session_data.simulated_A = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_A = false;
							}
							printf("simulated A\r\n");
						break;
						
						case MAN_TRIG_SEND_B:
							if (event_report.value_lsb == ON){
								session_data.simulated_B = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_B = false;
								
							}
							printf("simulated B\r\n");
						break;
						
						case MAN_TRIG_SEND_C:
							if (event_report.value_lsb == ON){
								session_data.simulated_C = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_C = false;
							
							}
							printf("simulated C\r\n");
						break;
						
						case MAN_TRIG_SEND_D:
							if (event_report.value_lsb == ON){
								session_data.simulated_D = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_D = false;
								
							}
							printf("simulated D\r\n");
						break;
						
						case MAN_TRIG_SEND_S:
							
							if ((event_report.value_lsb == ON) && (!session_data.simulation_mode) && (!session_data.session_running) && (session_data.manual_trigger) && (!trigger_debounce)){
								trigger_debounce = TRIGGER_DEBOUNCE_TIME;
								printf("simulated S\r\n");
								session_data.current_slice += 1;
								
								if (((session_data.current_slice >= session_data.no_of_slices) && (session_data.current_volume == session_data.no_of_volumes)) ||
								(session_data.current_volume > session_data.no_of_volumes)){
									session_data.session_finished = true;
								}
								
								if ((session_data.current_slice > session_data.no_of_slices) && (!session_data.session_finished)){
									session_data.current_volume += 1;
									session_data.current_slice = 1;
								}
															
								uint16_t divider = session_data.trig_on_volume;
								if (divider == 0) {
									divider = 1;
								}
								if ((session_data.trig_on_volume == 0) || (((session_data.current_volume - 1) % divider) == 0)){
									if ((session_data.trig_on_slice == 0) || (session_data.trig_on_slice == session_data.current_slice)){
										session_data.trigger = true;										
										
										printf("start sync sound\r\n");
										sync_beep_length = SYNC_BEEP_LENGTH;
										if (!session_data.trig_LED_on){
											session_data.trig_LED_on = true;
											printf("switch trigger LED on\r\n");
											io_write_clear_ack(man_trig_trigger_LED_on,6);
											trig_LED_delay = TRIGGER_LED_DELAY;
										}									
									}
								}
							}												
						
						break;
						
						case MAN_TRIG_BACK:
							stop_man_trig();						
						break;
						
						// ***********************   form3
						case SIMULATION_SEND_A:
							if (event_report.value_lsb == ON){
								session_data.simulated_A = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_A = false;
							}
							printf("simulated A\r\n");
						break;
						
						case SIMULATION_SEND_B:
							if (event_report.value_lsb == ON){
								session_data.simulated_B = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_B = false;
							}
							printf("simulated B\r\n");
						break;
						
						case SIMULATION_SEND_C:
							if (event_report.value_lsb == ON){
								session_data.simulated_C = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_C = false;
							}
							printf("simulated C\r\n");
						break;
						
						case SIMULATION_SEND_D:
							if (event_report.value_lsb == ON){
								session_data.simulated_D = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_D = false;
							}
							printf("simulated D\r\n");
						break;
						
						case STOP_SIMULATION:
							stop_simulation();												
						break;
						
						// **************** form4
						
						case SLICE_TRIGGERNO_BTN:
							session_temp.parameter = SLICE_TRIGGERNO_BTN;
							switch_to_form(KEYBOARD_FORM);
							kb_value = session_temp.trig_on_slice;
							minvalue = 0;
							maxvalue = 9999;
						
							io_write_clear_ack(set_trigg_slice_txt,6);
							print_no(kb_value, 11);
							printf("set slice trigg no, switch to form 6\r\n");
						break;
						
						case VOLUME_TRIGGERNO_BTN:
							session_temp.parameter = VOLUME_TRIGGERNO_BTN;
							switch_to_form(KEYBOARD_FORM);
							kb_value = session_temp.trig_on_volume;
							minvalue = 0;
							maxvalue = 9999;
						
							io_write_clear_ack(set_trigg_volume_txt,6);
							print_no(kb_value, 11);
							printf("set volume trigg no, switch to form 6\r\n");
						break;
						
						case FORM4_MORE:
							switch_to_form(SETTINGS_2_FORM);
							printf("form4 more, switch to form 5\r\n");
						break;
						
						case FORM4_ACCEPT:
							switch_to_form(MENU_FORM);
							session_data.no_of_slices = session_temp.no_of_slices;
							session_data.no_of_volumes = session_temp.no_of_volumes;
							session_data.trig_on_slice = session_temp.trig_on_slice;
							session_data.trig_on_volume = session_temp.trig_on_volume;
							session_data.pulse_length = session_temp.pulse_length;
							session_data.TR_time = session_temp.TR_time;
							printf("form4 accept, switch to form 7\r\n");
							
							write_flash();
							printf("FLASH written\r\n");
						break;
						
						case FORM4_CANCEL:
							switch_to_form(MENU_FORM);
							printf("form4 cancel, switch to form 7\r\n");
						break;
						
						
						// **************** form5
						case SET_SLICESNO_BTN:
							session_temp.parameter = SET_SLICESNO_BTN;
							switch_to_form(KEYBOARD_FORM);
							kb_value = session_temp.no_of_slices;
							minvalue = 1;
							maxvalue = 9999;

							io_write_clear_ack(set_no_of_slices_txt,6);
							print_no(kb_value, 11);
							printf("set slices no, switch to form 6\r\n");
						break;
						
						case SET_VOLUMESNO_BTN:
							session_temp.parameter = SET_VOLUMESNO_BTN;
							switch_to_form(KEYBOARD_FORM);
							kb_value = session_temp.no_of_volumes;
							minvalue = 1;
							maxvalue = 9999;
						
							io_write_clear_ack(set_no_of_volumes_txt,6);
							print_no(kb_value, 11);
							printf("set volumes no, switch to form 6\r\n");
						break;
						
						
						case PULSE_LENGTH_BTN:
							session_temp.parameter = PULSE_LENGTH_BTN;
							switch_to_form(KEYBOARD_FORM);
							kb_value = session_temp.pulse_length;
							minvalue = 100;
							maxvalue = 999;
	
							io_write_clear_ack(set_pulse_length_txt,6);							
							print_no(kb_value, 11);							
							printf("set pulse length, switch to form 6\r\n");
						break;
						
						case TR_TIME_BTN:
							session_temp.parameter = TR_TIME_BTN;
							switch_to_form(KEYBOARD_FORM);
							kb_value = session_temp.TR_time;
							uint32_t minvalue_long = session_temp.pulse_length * session_temp.no_of_slices;
							if (minvalue_long > 9999999){   // hibaellenõrzés!!!!!!!!!!!!
								minvalue = 9999999;
							}
							else{
								minvalue = minvalue_long;
							}
							maxvalue = 9999999;
	
							io_write_clear_ack(set_tr_time_txt,6);							
							print_no(kb_value, 11);							
							printf("set TR time, switchto form 6\r\n");
						break;
						
						case FORM5_MORE:
							switch_to_form(SETTINGS_1_FORM);
							printf("form8 more, switch to form 8\r\n");
						break;
						
						case FORM5_ACCEPT:
							switch_to_form(MENU_FORM);
							session_data.no_of_slices = session_temp.no_of_slices;
							session_data.no_of_volumes = session_temp.no_of_volumes;
							session_data.trig_on_slice = session_temp.trig_on_slice;
							session_data.trig_on_volume = session_temp.trig_on_volume;
							session_data.pulse_length = session_temp.pulse_length;
							session_data.TR_time = session_temp.TR_time;
							printf("form5 accept, switch to form 7\r\n");
							
							write_flash();
							printf("FLASH written\r\n");
						break;
						
						case FORM5_CANCEL:
							switch_to_form(MENU_FORM);
							printf("form5 cancel, switch to form 7\r\n");
						break;
						
						// **************** form7
						case SETTINGS:
							session_temp.no_of_slices = session_data.no_of_slices;
							session_temp.no_of_volumes = session_data.no_of_volumes;
							session_temp.trig_on_slice = session_data.trig_on_slice;
							session_temp.trig_on_volume = session_data.trig_on_volume;
							session_temp.pulse_length = session_data.pulse_length;
							session_temp.TR_time = session_data.TR_time;
							
							switch_to_form(SETTINGS_1_FORM);							
							printf("settings, switch to form 4\r\n");
						break;
						
						case SIMULATION:
							start_simulation();
						break;
						
						case MANUAL_TRIGGER:
							start_man_trig();
						break;
						
						case MENU_BACK:
							switch_to_form(DEFAULT_FORM);
							printf("menu back, switch to form 0\r\n");
						break;
						
						case ABOUT:
							switch_to_form(ABOUT_FORM);
							printf("about, switch to form 0\r\n");
						break;

						case HELP:
							switch_to_form(HELP_FORM);
							printf("about, switch to form 0\r\n");
						break;


						// **************** form8
						case FORM8_MORE:
							switch_to_form(SETTINGS_1_FORM);
							printf("form4 more, switch to form 4  \r\n");
						break;
						
						case FORM8_ACCEPT:
							switch_to_form(MENU_FORM);
							session_data.no_of_slices = session_temp.no_of_slices;
							session_data.no_of_volumes = session_temp.no_of_volumes;
							session_data.trig_on_slice = session_temp.trig_on_slice;
							session_data.trig_on_volume = session_temp.trig_on_volume;
							session_data.pulse_length = session_temp.pulse_length;
							session_data.TR_time = session_temp.TR_time;
							printf("form4 accept, switch to form 7\r\n");
							
							write_flash();
							printf("FLASH written\r\n");
							
						break;
						
						// **************** form9
						case FORM9_BACK:
							switch_to_form(MENU_FORM);
							printf("form9 back, switch to menu form  \r\n");
						break;						

						// **************** form10
						case FORM10_BACK:
							switch_to_form(MENU_FORM);
							printf("form10 back, switch to menu form  \r\n");
						break;
												
						break;
						default:
						;
					}
				break;
				
				case FOURDBUTTON:
				
					printf("4dbutton pressed, index: %u, value: %u\r\n", event_report.object_index, event_report.value_lsb);
					click_length = CLICK_SOUND_LENGTH;
					
					switch (event_report.object_index) {
						
						// **************** form1
						case FORM1_TRIGGER_SOUND_SW:
							switch (event_report.value_lsb){
								case ON:
									io_write_clear_ack(set_form1_trigger_sound_on,6);
									session_data.trigger_sound = true;
									printf("trigger snd switch is on\r\n");								
								break;
								
								case OFF:
									io_write_clear_ack(set_form1_trigger_sound_off,6);
									session_data.trigger_sound = false;
									printf("trigger snd switch is off\r\n");	
								break;
									
								}
							break;
												
						case FORM1_RESPONSE_SOUND_SW:
							switch (event_report.value_lsb){
								case ON:
									io_write_clear_ack(set_form1_response_sound_on,6);
									session_data.response_sound = true;
									printf("response snd switch is on\r\n");	
								break;
								
								case OFF:
									io_write_clear_ack(set_form1_response_sound_off,6);
									session_data.response_sound = false;
									printf("response snd switch is off\r\n");	
								break;
								
							}
						break;
								
						// form4
						
						case TRIGGER_SOUND_SW:
						switch (event_report.value_lsb){
							case ON:
							io_write_clear_ack(set_trigger_sound_on,6);
							session_data.trigger_sound = true;
							printf("trigger snd switch is on\r\n");
							break;
													
							case OFF:
							io_write_clear_ack(set_trigger_sound_off,6);
							session_data.trigger_sound = false;
							printf("trigger snd switch is off\r\n");
							break;
													
						}
						break;
												
						case RESPONSE_SOUND_SW:
						switch (event_report.value_lsb){
							case ON:
							io_write_clear_ack(set_response_sound_on,6);
							session_data.response_sound = true;
							printf("response snd switch is on\r\n");
							break;
													
							case OFF:
							io_write_clear_ack(set_response_sound_off,6);
							session_data.response_sound = false;
							printf("response snd switch is off\r\n");
							break;
													
						}
						break;		
						default:
						;
					}
					
				break;
						
				case KEYBOARD:
				
					printf("4dbutton pressed, index: %u, value: %u\r\n", event_report.object_index, event_report.value_lsb);
					
					click_length = CLICK_SOUND_LENGTH;

					uint32_t prev_kb_value = kb_value;

					switch (event_report.value_lsb){
						case KEYBOARD_BACK:
							if (kb_value != 0){
								kb_value = kb_value / 10;
							}
							printf("back\r\n");
							
						break;
						
						case KEYBOARD_OK:
							printf("keyboard ok\r\n");
							
							printf("kb value: %lu\r\n", kb_value);
							
							if (kb_value < minvalue){
								kb_value = minvalue;
								print_no(kb_value, 11);
								delay_ms(500);
							}
							
							if (kb_value > maxvalue){
								kb_value = maxvalue;
								print_no(kb_value, 11);
								delay_ms(500);
							}
							
							printf("kb return value: %lu\r\n", kb_value);
							
							switch_to_form(session_data.previous_form);							
						
							switch (session_temp.parameter){
								case SET_SLICESNO_BTN:
									session_temp.no_of_slices = kb_value;	
									if (session_temp.TR_time < (session_temp.no_of_slices * session_temp.pulse_length)){
										session_temp.TR_time = (session_temp.no_of_slices * session_temp.pulse_length);
									}																
								break;
													
								case SET_VOLUMESNO_BTN:
									session_temp.no_of_volumes = kb_value;									
								break;
																					
								case SLICE_TRIGGERNO_BTN:
									session_temp.trig_on_slice = kb_value;									
								break;
													
								case VOLUME_TRIGGERNO_BTN:
									session_temp.trig_on_volume = kb_value;
								break;
													
								case PULSE_LENGTH_BTN:
									session_temp.pulse_length = kb_value;
									if (session_temp.TR_time < (session_temp.no_of_slices * session_temp.pulse_length)){
										session_temp.TR_time = (session_temp.no_of_slices * session_temp.pulse_length);
									}
								break;
													
								case TR_TIME_BTN:
									session_temp.TR_time = kb_value;
								break;
													
								default:
								break;
							}
							print_no(session_temp.no_of_slices, SET_SLICES_STR);
							print_no(session_temp.no_of_volumes, SET_VOLUMES_STR);
							print_no(session_temp.trig_on_slice, SLICE_TRIGGERNO_STR);
							print_no(session_temp.trig_on_volume, VOLUME_TRIGGERNO_STR);
							print_no(session_temp.pulse_length, PULSE_LENGTH_STR);
							print_no(session_temp.TR_time, TR_TIME_STR);
														
							printf("keyboard ok, switch back to form %u\r\n", session_data.previous_form);
							
						break;
						
						case KEYBOARD_CANCEL:
							printf("cancel\r\n");
							switch_to_form(session_data.previous_form);
							printf("keyboard cancel, switch back to form %u\r\n", session_data.previous_form);
							
						break;
						
						default:
						if ((event_report.value_lsb>0x29) && (event_report.value_lsb<0x3a)){
							unsigned char digit=event_report.value_lsb-0x30;
													
							kb_value = kb_value * 10 + digit;
							if (kb_value > maxvalue){
								kb_value = prev_kb_value;
							}
							
							printf("%i\r\n", digit);							
						}
						break;
					}
					printf("kb value: %lu, prev kb value: %lu\r\n", kb_value, prev_kb_value);
					print_no(kb_value, 11);
					
				break;
				default:
				;					
			}
			
			printf("Event received from %u, index: %u\r\n", event_report.object_id, event_report.object_index);
			
			//snprintf(buffer, 64, "%i\r\n", digit);
			//cdcdf_acm_write((uint8_t *) buffer, strnlen(buffer, 64));
		}
		
	}

}