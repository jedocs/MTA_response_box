// ha mindk�t nagym�ret� hangf�jl haszn�lva van, akkor az egyikben egy 256ms-os glitch lesz (65535-�s �rt�kek)
// a compiler runtimeban a RAM-ba m�solja �s nem f�r el?????
// az egyiket nem haszn�lva a jelens�g megsz�nik (konkr�tan a m�trix olvas�st kikommentelve (dc_out = stop_sound[STOP_SOUND_LENGTH - stop_sound_length];)).
// az egym�shoz k�pesti sorrendj�kt�l (TIMER int-ben) f�gg, hogy melyikn�l fordul el�

// release buildn�l nagyon nem mindegy az optimaliz�ci�, van hogy egy�ltal�n nem m�k�dik a program
// release buildn�l extra delayok kellenek, k�l�nben nem m�k�dik ami debugban igen

// 1.0.6 release-ben optimize for size opci�val lett ford�tva, start/stop sound n�lk�l



// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// irq-b�l nem j� az LCD soros kommunik�ci�!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

static uint32_t ack_timeout_counter = 0;
static uint8_t prescaler = PRESCALER_VALUE;


static uint16_t error_beep_length = 0;
static uint16_t click_length = 0;
// static uint16_t start_sound_length = 0;
// static uint16_t stop_sound_length = 0;
extern uint16_t response_beep_length;
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
	printf("flash write no of slices, %u\r", i);
	flash_write(&FLASH_0, ad, ic, 2);
	delay_ms(delay);
		
	i = session_data.no_of_volumes;
	ic = (unsigned char *) &i;
	printf("flash write no of volumes, %u\r", i);
	flash_write(&FLASH_0, ad + 2, ic, 2);
	delay_ms(delay);
	
	i = session_data.trig_on_slice;
	ic = (unsigned char *) &i;
	printf("flash write trigg on slice, %u\r", i);
	flash_write(&FLASH_0, ad + 4, ic, 2);
	delay_ms(delay);
	
	i = session_data.trig_on_volume;
	ic = (unsigned char *) &i;
	printf("flash write trigg on volume, %u\r", i);
	flash_write(&FLASH_0, ad + 6, ic, 2);
	delay_ms(delay);
	
	i = session_data.pulse_length;
	ic = (unsigned char *) &i;
	printf("flash write pulse length, %u\r", i);
	flash_write(&FLASH_0, ad + 8, ic, 2);
	delay_ms(delay);

	uint32_t li;
	li = session_data.TR_time;
	unsigned char * lic = (unsigned char *) &li;
	printf("flash write TR time, %lu\r", li);
	flash_write(&FLASH_0, ad + 0x0a, lic, 4);
	delay_ms(delay);	
	
	i = session_data.trigger_sound * 256 + session_data.response_sound;
	ic = (unsigned char *) &i;
	printf("flash write trigger / response sound, %u\r", i);
	flash_write(&FLASH_0, ad + 0x0e, ic, 2);
	delay_ms(delay);
	
	i = session_data.sound_volume;
	ic = (unsigned char *) &i;
	printf("flash write sound volume, %u\r", i);
	flash_write(&FLASH_0, ad + 0x10, ic, 2);
	delay_ms(delay);
}

void read_flash(){
		
	static uint8_t a[4];
	printf("\rread session data from flash\r");
		
	flash_read(&FLASH_0, ad, a, 2);	
	session_data.no_of_slices = a[0] + a[1] * 256;	
	printf("no_of_slices read from flash: %u\r", session_data.no_of_slices);	
	if ((session_data.no_of_slices < 1) || (session_data.no_of_slices > 9999)) {
		session_data.no_of_slices = 25;
	}	
	
	flash_read(&FLASH_0, ad + 2, a, 2);
	session_data.no_of_volumes = a[0] + a[1] * 256;
	printf("no_of_volumes read from flash: %u\r", session_data.no_of_volumes);	
	if ((session_data.no_of_volumes < 1) || (session_data.no_of_volumes > 9999)) {
		session_data.no_of_volumes = 100;
	}
	
	flash_read(&FLASH_0, ad + 4, a, 2);
	session_data.trig_on_slice = a[0] + a[1] * 256;
	printf("trig_on_slice read from flash: %u\r", session_data.trig_on_slice);	
	if ((session_data.trig_on_slice < 0) || (session_data.trig_on_slice > 9999)) {
		session_data.trig_on_slice = 0;
	}
			
	flash_read(&FLASH_0, ad + 6, a, 2);
	session_data.trig_on_volume = a[0] + a[1] * 256;
	printf("trig_on_volume read from flash: %u\r", session_data.trig_on_volume);	
	if ((session_data.trig_on_volume < 0) || (session_data.trig_on_volume > 9999)) {
		session_data.trig_on_volume = 0;
	}
		
	flash_read(&FLASH_0, ad + 8, a, 2);
	session_data.pulse_length = a[0] + a[1] * 256;
	printf("pulse_length read from flash: %u\r", session_data.pulse_length);	
	if ((session_data.pulse_length < 100) || (session_data.pulse_length > 999)) {
		session_data.pulse_length = 100;
	}
		
	flash_read(&FLASH_0, ad + 0x0a, a, 4);
	session_data.TR_time = a[0] + a[1] * 256 + a[2] * 65536 + a[3] * 16777216;
	printf("TR_time read from flash: %lu\r", session_data.TR_time);	
	uint32_t minvalue_long = session_data.pulse_length * session_data.no_of_slices;
	if (minvalue_long > 9999999){   // hibaellen�rz�s!!!!!!!!!!!!
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
	printf("trigger / response sound: %u, %u\r", session_data.trigger_sound, session_data.response_sound);		
	
	flash_read(&FLASH_0, ad + 0x10, a, 2);
	session_data.sound_volume = a[0];
	printf("sound volume read from flash: %u\r", session_data.sound_volume);	
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
		printf("command couldn't be sent, waiting for ACK from prev. command\r");
		ack_timeout_counter = ACK_TIMEOUT;
		while (ack_timeout_counter){
			delay_us(100);    		// delay n�lk�l release-ban nem m�k�dik, csak debug-ban!!!!!
				if (ACK_received){
				break;
			}
		}
		if (ACK_received){
			printf("received an ACK finally...\r");
			printf("timeout counter: %lu\r", (ACK_TIMEOUT - ack_timeout_counter));
			//printf("send command\r");
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
	printf("\rprint version string\r");
	io_write_clear_ack(buffer, length+1);
	printf("version: %s\r", VERSION);
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
	printf("\rwrite number\r");
    io_write_clear_ack(buffer, length+1);
	printf("number: %lu\r", number);
}

static void print_slices(uint8_t str_no){
	uint8_t length = snprintf(buffer, 64, "___%u / %u    ", session_data.current_slice, session_data.no_of_slices);
	buffer[0] = WRITE_STR;
	buffer[1] = str_no;
	buffer[2] = length-3;
	buffer[length] = getCRC(buffer, length);
	buffer[length+1] = 0; // ????????????
	printf("\rwrite slices\r");
	io_write_clear_ack(buffer,length+1);
	printf("current slice: %u\r", session_data.current_slice);
}

static void print_volumes(uint8_t str_no){
	uint8_t length = snprintf(buffer, 64, "___%u / %u    ", session_data.current_volume, session_data.no_of_volumes);
	buffer[0] = WRITE_STR;
	buffer[1] = str_no;
	buffer[2] = length-3;
	buffer[length] = getCRC(buffer, length);
	buffer[length+1] = 0; // ????????????
	printf("\rwrite volumes\r");
	io_write_clear_ack(buffer,length+1);
	printf("**********current volume: %u\r", session_data.current_volume);
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
			printf("\rsw to default form\r");
			io_write_clear_ack(switch_to_default_form,6);
			set_prev_curr_form(form);			
		break;
		
		case SESSION_RUNNING_FORM:
			printf("\rsw to experiment running form\r");
			io_write_clear_ack(switch_to_session_running_form,6);
			set_prev_curr_form(form);
			
			if (session_data.response_sound){
				printf("\rsetting response sound text to on\r");
				io_write_clear_ack(set_form1_response_sound_on,6);
				printf("\rsetting response sound switch to on\r");
				io_write_clear_ack(set_form1_response_sound_switch_on,6);
			}
			else{
				printf("\rsetting response sound text to off\r");
				io_write_clear_ack(set_form1_response_sound_off,6);
				printf("\rsetting response sound switch to off\r");
				io_write_clear_ack(set_form1_response_sound_switch_off,6);
			}
			
			if (session_data.trigger_sound){
				printf("\rsetting trigger sound text to on\r");
				io_write_clear_ack(set_form1_trigger_sound_on,6);
				printf("\rsetting trigger sound switch to on\r");				
				io_write_clear_ack(set_form1_trigger_sound_switch_on,6);
			}
			else{
				printf("\rsetting trigger sound text to off\r");
				io_write_clear_ack(set_form1_trigger_sound_off,6);
				printf("\rsetting trigger sound switch to off\r");
				io_write_clear_ack(set_form1_trigger_sound_switch_off,6);
			}
						
		break;	
		
		case MANUAL_TRIGGER_FORM:
			printf("\rsw to man trig form\r");
			io_write_clear_ack(switch_to_manual_trigger_form,6);
			set_prev_curr_form(form);
			break;
			
			case SIMULATION_RUNNING_FORM:
			printf("\rsw to simulation form\r");
			io_write_clear_ack(switch_to_simulation_running_form,6);
			set_prev_curr_form(form);
		break;
				
		case SETTINGS_1_FORM:
			printf("\rswitch to settings 1 form\r");
			io_write_clear_ack(switch_to_settings_1_form,6);
			set_prev_curr_form(form);
						
			if (session_data.response_sound){
				printf("\rsetting response sound text to on\r");
				io_write_clear_ack(set_response_sound_on,6);
				printf("\rsetting response sound switch to on\r");
				io_write_clear_ack(set_response_sound_switch_on,6);
			}
			else{
				printf("\rsetting response sound text to off\r");
				io_write_clear_ack(set_response_sound_off,6);
				printf("\rsetting response sound switch to off\r");
				io_write_clear_ack(set_response_sound_switch_off,6);
			}
						
			if (session_data.trigger_sound){
				printf("\rsetting trigger sound text to on\r");
				io_write_clear_ack(set_trigger_sound_on,6);
				printf("\rsetting trigger sound switch to on\r");
				io_write_clear_ack(set_trigger_sound_switch_on,6);
			}
			else{
				printf("\rsetting trigger sound text to off\r");
				io_write_clear_ack(set_trigger_sound_off,6);
				printf("\rsetting trigger sound switch to off\r");
				io_write_clear_ack(set_trigger_sound_switch_off,6);
			}
			
				printf("setting trig on slice string\r");
				print_no(session_temp.trig_on_slice, SLICE_TRIGGERNO_STR);
				printf("setting trig on volume string\r");
				print_no(session_temp.trig_on_volume, VOLUME_TRIGGERNO_STR);
			
		break;
		
		case SETTINGS_2_FORM:
			printf("\rsw to settings 2 form\r");
			io_write_clear_ack(switch_to_settings_2_form,6);
			set_prev_curr_form(form);					
						
			printf("setting no of volumes string\r");
			print_no(session_temp.no_of_volumes, SET_VOLUMES_STR);
			printf("setting no of slices string\r");
			print_no(session_temp.no_of_slices, SET_SLICES_STR);
			printf("setting pulse length string\r"); 
 			print_no(session_temp.pulse_length, PULSE_LENGTH_STR);
			printf("setting TR time string\r");
 			print_no(session_temp.TR_time, TR_TIME_STR);
		break;
		
		case SETTINGS_3_FORM:
			printf("\rsw to settings 3 form\r");
			io_write_clear_ack(switch_to_settings_3_form,6);
			set_prev_curr_form(form);
		break;
				
		case KEYBOARD_FORM:
			printf("\rsw to keyboard form\r");
			io_write_clear_ack(switch_to_keyboard_form,6);
			set_prev_curr_form(form);
		break;
		
		case MENU_FORM:
			printf("\rsw to menu form\r");
			io_write_clear_ack(switch_to_menu_form,6);
			set_prev_curr_form(form);
		break;		
		
		case ABOUT_FORM:
			printf("\rsw to about form\r");
			io_write_clear_ack(switch_to_about_form,6);
			print_version();
			set_prev_curr_form(form);
		break;
		
		case ERROR_FORM:
			printf("sw to error form\r");
			printf("disabling TX/RX power\r");
			
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
						
			printf("\rsw to error form\r");
			io_write_clear_ack(switch_to_error_form,6);
			set_prev_curr_form(form);
			error_beep_length = ERROR_BEEP_LENGTH;
		break;

		case HELP_FORM:
			printf("\rsw to help form\r");
			io_write_clear_ack(switch_to_help_form,6);
			set_prev_curr_form(form);
		break;
	}	
}

void ps_error(){
	switch_to_form(ERROR_FORM);
	
	if (adc_value < 1550){
		printf("\rset error text PS low\r");
		io_write_clear_ack(set_error_text_PS_low,6);
		printf("!!!!!!!!!!!!!!!!!!!");
		printf("ps voltage too low");
	}
	
	if (adc_value > 2000){
		printf("\rset error text PS high\r");
		io_write_clear_ack(set_error_text_PS_high,6);
		printf("!!!!!!!!!!!!!!!!!!!");
		printf("ps voltage too high");
	}
	printf("psup voltage is %d\r", adc_value);
}

void usb_error(){
	switch_to_form(ERROR_FORM);	
	
	printf("\rset error textUSB error\r");
	io_write_clear_ack(set_error_text_USB_error,6);
	printf("!!!!!!!!!!!!!!!!!!!\r");
	printf("USB error\r");
	
	while (usb_disconnected){
		delay_ms(1);		// delay n�lk�l release-ban nem m�k�dik, csak debug-ban!!!!!
	}
	
	printf("USB connection ok\r");
	switch_to_form(DEFAULT_FORM);
}

void start_simulation()
{
	printf("starting simulation\r");
	
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
	printf("setting current slice string\r");
	print_slices(SIMULATION_CURRENT_SLICE_STR);
	printf("setting current volume string\r");	
	print_volumes(SIMULATION_CURRENT_VOLUME_STR);
	
// 	printf("playing start sound\r");
// 	start_sound_length = START_SOUND_LENGT
	printf("delay 1000ms\r");
	delay_ms(1000);
	session_data.simulation_mode=true;	
}

void stop_simulation()
{
	printf("stopping simulation\r");
	pb_received = false;
	//stop_sound_length = STOP_SOUND_LENGTH;
	printf("\rsim trigger LED off\r");
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
	printf("starting manual trigger\r");
	
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
	printf("setting current slice string\r");
	print_slices(MAN_TRIG_CURRENT_SLICE_STR);
	printf("setting current volume string\r");
	print_volumes(MAN_TRIG_CURRENT_VOLUME_STR);

// 	printf("playing start sound\r");
// 	start_sound_length = START_SOUND_LENGTH;
	session_data.manual_trigger=true;	
}

void stop_man_trig()
{
	printf("stopping manual trigger\r");
	pb_received = false;
	//stop_sound_length = STOP_SOUND_LENGTH;
	printf("\rman trigg LED off\r");
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
			
		printf("starting experiment\r");
		
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
	
		printf("enable RX pwr\r");
		gpio_set_pin_level(RX_PWR, true);
		
		printf("enable TX ch. 1 power\r");
		gpio_set_pin_level(T1, true);
	
		printf("enable TX ch. 2 power\r");
		gpio_set_pin_level(T2, true);
	
		printf("\rtrig received LED off\r");
		io_write_clear_ack(trigger_received_LED_off,6);
		printf("\rtrig sent LED off\r");
		io_write_clear_ack(trigger_sent_LED_off,6);

		printf("setting current slice string\r");
		print_slices(CURRENT_SLICE_STR);
		printf("setting current volume string\r");
		print_volumes(CURRENT_VOLUME_STR);
	
// 		printf("playing start sound\r");
// 		start_sound_length = START_SOUND_LENGTH;
		session_data.session_running = true;
	}
	else {
		ps_error();
	}
}

void stop_session()
{
	printf("stopping session\r");
	//stop_sound_length = STOP_SOUND_LENGTH;
	switch_to_form(DEFAULT_FORM);
	
	session_data.session_running = false;
	session_data.simulation_mode = false;
	session_data.trigger = false;
	session_data.current_volume = 1;
	session_data.current_slice = 0;
	
	printf("powering down TX ch. 1\r");
	gpio_set_pin_level(T1, false);
	printf("powering down TX ch. 2\r");
	gpio_set_pin_level(T2, false);	
	printf("disable RX pwr\r");
	gpio_set_pin_level(RX_PWR, false);
}

static void button(void)	// called by ext irq
{
	if (!pb_debounce){
	printf("button pressed\r");
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
		
	if (response_beep_length){
		if (session_data.response_sound){
			dc_out = response_beep_sound_data[RESPONSE_BEEP_LENGTH - response_beep_length];
			dac_async_write(&DAC_0, 0, &dc_out, 1);
			response_beep_length --;
		}
		else{
			response_beep_length = 0;
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
			usb_disconnected_prescaler = 5;
			if (usb_disconnected < 500){
				usb_disconnected ++;
			}
		}
		if (ack_timeout_counter){
			ack_timeout_counter --;
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
	}
}

static void TIMER_task2_cb(const struct timer_task *const timer_task)  // 1.6Hz
{
	if (adc_read){
		adc_read=false;
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
	
	printf("\rLCD UART init:\r");
	uart_init();  // LCD communication
		
	page_size = flash_get_page_size(&FLASH_0);	
	printf("\rFLASH page size: %d\r", page_size);
	
// 	page_size=sizeof(session_data);
// 	printf("\rsession data size: %d\r", page_size);
	
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

	printf("\renabling DAC...\r");
	dac_async_enable_channel(&DAC_0, 0);
	
	printf("\rregister ext irq: sync trigger\r");
    ext_irq_register(I0, sync_trigger);
	
	printf("\rregister ext irq: button\r");
    ext_irq_register(PB2, button);
		
	printf("\rreset LCD\r");
	gpio_set_pin_level(DISP_RESET, true);
	delay_ms(2000);
	printf("\rset ACK after display reset\r");
	ACK_received = true;
	
	printf("\renabling audio amp\r");
	gpio_set_pin_level(AUDENB, true);

	printf("\r***************************************\r");
	printf("initialization done, starting main loop\r");
	usb_disconnected = 0;
	
	while (1) {		
		
		if ((adc_value < 1500) || (adc_value > 2000)) {
			ps_error_counter ++;
			printf("ps dip, voltage: %d, counter: %u\r", adc_value, ps_error_counter);
		}
		else{
			ps_error_counter = 0;
		}
		if (ps_error_counter > 2){
			ps_error();
		}
				
		if (usb_disconnected > 100){
			printf("USB disconnected!\r");
			usb_error();
		}
				
		if (session_data.session_running) {		

			if (session_data.trigger){
				if (!session_data.trig_sent_LED_on){
					session_data.trig_sent_LED_on = true;
					printf("\rtrigger sent LED on\r");
					io_write_clear_ack(trigger_sent_LED_on,6);
					trig_sent_LED_delay = TRIGGER_LED_DELAY;
				}
			}
			
			if (session_data.previous_slice != session_data.current_slice){
				session_data.previous_slice = session_data.current_slice;
				if (!session_data.trig_LED_on){
					session_data.trig_LED_on = true;
					printf("\rtrigger received LED on\r");
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
				printf("\rtrigger received LED off\r");
				io_write_clear_ack(trigger_received_LED_off,6);
				
			}

			if ((!trig_sent_LED_delay) && (session_data.trig_sent_LED_on)){
				session_data.trig_sent_LED_on = false;
				printf("\rtrigger sent LED off\r");
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
					printf("manual trigger mode, simulated S\r");
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
								printf("\rmanual trigger mode, switch trigger LED on\r");
								io_write_clear_ack(man_trig_trigger_LED_on,6);
								trig_LED_delay = TRIGGER_LED_DELAY;
							}
						}
					}
				}
			}
			
			if ((!trig_LED_delay) && (session_data.trig_LED_on)){
				session_data.trig_LED_on = false;
				printf("\rmanual trigger mode, switch trigger LED off\r");
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
				delay_ms(1500);
				stop_man_trig();
			}
		}
		
		else if (session_data.simulation_mode) {
			
			if ((session_data.trigger) && (!session_data.trig_LED_on)){
				session_data.trig_LED_on = true;
				printf("\rsimulation switch trigger LED on\r");
				io_write_clear_ack(simulation_trigger_LED_on,6);
				trig_LED_delay = TRIGGER_LED_DELAY;
			}			
			
			if ((!trig_LED_delay) && (session_data.trig_LED_on)){
				session_data.trig_LED_on = false;
				printf("\rsimulation switch trigger LED off\r");
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
				printf("stop experiment by pushbutton\r");
				stop_session();
			}
			else{
				printf("start experiment by pushbutton\r");
				start_session();
			}
		}
				
		if (event_report.report_valid){			
			event_report.report_valid=false;
	
			switch (event_report.object_id) {
				case WINBUTTON:								
					printf("\rLCD button pressed, index: %u\r", event_report.object_index);
					
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
							printf("switching to form 7\r");						
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
							printf("simulated A\r");
						break;
						
						case MAN_TRIG_SEND_B:
							if (event_report.value_lsb == ON){
								session_data.simulated_B = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_B = false;								
							}
							printf("simulated B\r");
						break;
						
						case MAN_TRIG_SEND_C:
							if (event_report.value_lsb == ON){
								session_data.simulated_C = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_C = false;							
							}
							printf("simulated C\r");
						break;
						
						case MAN_TRIG_SEND_D:
							if (event_report.value_lsb == ON){
								session_data.simulated_D = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_D = false;								
							}
							printf("simulated D\r");
						break;
						
						case MAN_TRIG_SEND_S:
							
							if ((event_report.value_lsb == ON) && (!session_data.simulation_mode) && (!session_data.session_running) && (session_data.manual_trigger) && (!trigger_debounce)){
								trigger_debounce = TRIGGER_DEBOUNCE_TIME;
								printf("simulated S\r");
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
											printf("\rman trig, trig LED on\r");
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
							printf("simulated A\r");
						break;
						
						case SIMULATION_SEND_B:
							if (event_report.value_lsb == ON){
								session_data.simulated_B = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_B = false;
							}
							printf("simulated B\r");
						break;
						
						case SIMULATION_SEND_C:
							if (event_report.value_lsb == ON){
								session_data.simulated_C = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_C = false;
							}
							printf("simulated C\r");
						break;
						
						case SIMULATION_SEND_D:
							if (event_report.value_lsb == ON){
								session_data.simulated_D = true;
								delay_ms(10);
							}
							else{
								session_data.simulated_D = false;
							}
							printf("simulated D\r");
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
							
							printf("\rset trig slice\r");
							io_write_clear_ack(set_trigg_slice_txt,6);
							print_no(kb_value, 11);
							printf("set slice trigg no, switch to form 6\r");
						break;
						
						case VOLUME_TRIGGERNO_BTN:
							session_temp.parameter = VOLUME_TRIGGERNO_BTN;
							switch_to_form(KEYBOARD_FORM);
							kb_value = session_temp.trig_on_volume;
							minvalue = 0;
							maxvalue = 9999;
						
							printf("\rset trig volume\r");
							io_write_clear_ack(set_trigg_volume_txt,6);
							print_no(kb_value, 11);
							printf("set volume trigg no, switch to form 6\r");
						break;
						
						case FORM4_MORE:
							switch_to_form(SETTINGS_2_FORM);
							printf("form4 more, switch to form 5\r");
						break;
						
						case FORM4_ACCEPT:
							switch_to_form(MENU_FORM);
							session_data.no_of_slices = session_temp.no_of_slices;
							session_data.no_of_volumes = session_temp.no_of_volumes;
							session_data.trig_on_slice = session_temp.trig_on_slice;
							session_data.trig_on_volume = session_temp.trig_on_volume;
							session_data.pulse_length = session_temp.pulse_length;
							session_data.TR_time = session_temp.TR_time;
							printf("form4 accept, write FLASH, switch to menu form\r");
							
							write_flash();
							printf("FLASH written\r");
						break;
						
						case FORM4_CANCEL:
							switch_to_form(MENU_FORM);
							printf("form4 cancel, switch to menu form\r");
						break;						
						
						// **************** form5
						case SET_SLICESNO_BTN:
							session_temp.parameter = SET_SLICESNO_BTN;
							switch_to_form(KEYBOARD_FORM);
							kb_value = session_temp.no_of_slices;
							minvalue = 1;
							maxvalue = 9999;

							printf("\rset no of slices\r");
							io_write_clear_ack(set_no_of_slices_txt,6);
							print_no(kb_value, 11);
						break;
						
						case SET_VOLUMESNO_BTN:
							session_temp.parameter = SET_VOLUMESNO_BTN;
							switch_to_form(KEYBOARD_FORM);
							kb_value = session_temp.no_of_volumes;
							minvalue = 1;
							maxvalue = 9999;
							
							printf("\rset no of volumes\r");
							io_write_clear_ack(set_no_of_volumes_txt,6);
							print_no(kb_value, 11);
						break;						
						
						case PULSE_LENGTH_BTN:
							session_temp.parameter = PULSE_LENGTH_BTN;
							switch_to_form(KEYBOARD_FORM);
							kb_value = session_temp.pulse_length;
							minvalue = 100;
							maxvalue = 999;
	
							printf("\rset pulse length\r");
							io_write_clear_ack(set_pulse_length_txt,6);							
							print_no(kb_value, 11);							
						break;
						
						case TR_TIME_BTN:
							session_temp.parameter = TR_TIME_BTN;
							switch_to_form(KEYBOARD_FORM);
							kb_value = session_temp.TR_time;
							uint32_t minvalue_long = session_temp.pulse_length * session_temp.no_of_slices;
							if (minvalue_long > 9999999){   // hibaellen�rz�s!!!!!!!!!!!!
								minvalue = 9999999;
							}
							else{
								minvalue = minvalue_long;
							}
							maxvalue = 9999999;
	
							printf("\rset TR time\r");
							io_write_clear_ack(set_tr_time_txt,6);							
							print_no(kb_value, 11);							
						break;
						
						case FORM5_MORE:
							switch_to_form(SETTINGS_1_FORM);
							printf("settings 2 form more, switch to settings 1 form\r");
						break;
						
						case FORM5_ACCEPT:
							switch_to_form(MENU_FORM);
							session_data.no_of_slices = session_temp.no_of_slices;
							session_data.no_of_volumes = session_temp.no_of_volumes;
							session_data.trig_on_slice = session_temp.trig_on_slice;
							session_data.trig_on_volume = session_temp.trig_on_volume;
							session_data.pulse_length = session_temp.pulse_length;
							session_data.TR_time = session_temp.TR_time;
							printf("settings 2 form accept, write FLASH, switch to menu form\r");
							
							write_flash();
							printf("FLASH written\r");
						break;
						
						case FORM5_CANCEL:
							switch_to_form(MENU_FORM);
							printf("settings 2 form cancel, switch to menu form\r");
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
							printf("menu: settings, switch to settings 1 form\r");
						break;
						
						case SIMULATION:
							start_simulation();
						break;
						
						case MANUAL_TRIGGER:
							start_man_trig();
						break;
						
						case MENU_BACK:
							switch_to_form(DEFAULT_FORM);
							printf("menu: back, switch to startup form\r");
						break;
						
						case ABOUT:
							switch_to_form(ABOUT_FORM);
							printf("menu: about, switch to about form\r");
						break;

						case HELP:
							switch_to_form(HELP_FORM);
							printf("menu: help, switch to help form\r");
						break;


						// **************** form8
// 						case FORM8_MORE:
// 							switch_to_form(SETTINGS_1_FORM);
// 							printf("form4 more, switch to form 4  \r");
// 						break;
						
// 						case FORM8_ACCEPT:
// 							switch_to_form(MENU_FORM);
// 							session_data.no_of_slices = session_temp.no_of_slices;
// 							session_data.no_of_volumes = session_temp.no_of_volumes;
// 							session_data.trig_on_slice = session_temp.trig_on_slice;
// 							session_data.trig_on_volume = session_temp.trig_on_volume;
// 							session_data.pulse_length = session_temp.pulse_length;
// 							session_data.TR_time = session_temp.TR_time;
// 							printf("form4 accept, switch to form 7\r");
// 							
// 							write_flash();
// 							printf("FLASH written\r");
// 							
// 						break;
						
						// **************** form9
						case FORM9_BACK:
							switch_to_form(MENU_FORM);
							printf("help: back, switch to menu form  \r");
						break;						

						// **************** form10
						case FORM10_BACK:
							switch_to_form(MENU_FORM);
							printf("about: back, switch to menu form  \r");
						break;
												
						break;
						default:
						;
					}
				break;
				
				case FOURDBUTTON:
				
					printf("4dbutton pressed, index: %u, value: %u\r", event_report.object_index, event_report.value_lsb);
					click_length = CLICK_SOUND_LENGTH;
					
					switch (event_report.object_index) {
						
						// **************** form1
						case FORM1_TRIGGER_SOUND_SW:
							switch (event_report.value_lsb){
								case ON:
									printf("\rset trigger snd switch on\r");
									io_write_clear_ack(set_form1_trigger_sound_on,6);
									session_data.trigger_sound = true;																
								break;
								
								case OFF:
									printf("\rset trigger snd switch off\r");
									io_write_clear_ack(set_form1_trigger_sound_off,6);
									session_data.trigger_sound = false;
								break;									
								}
							break;
												
						case FORM1_RESPONSE_SOUND_SW:
							switch (event_report.value_lsb){
								case ON:
									printf("\rset response snd switch on\r");
									io_write_clear_ack(set_form1_response_sound_on,6);
									session_data.response_sound = true;
								break;
								
								case OFF:
									printf("\rset response snd switch off\r");
									io_write_clear_ack(set_form1_response_sound_off,6);
									session_data.response_sound = false;
								break;								
							}
						break;
								
						// form4
						
						case TRIGGER_SOUND_SW:
						switch (event_report.value_lsb){
							case ON:
								printf("\rset trigger snd switch on\r");
								io_write_clear_ack(set_trigger_sound_on,6);
								session_data.trigger_sound = true;
							break;
													
							case OFF:
								printf("\rset trigger snd switch off\r");
								io_write_clear_ack(set_trigger_sound_off,6);
								session_data.trigger_sound = false;
							break;													
						}
						break;
												
						case RESPONSE_SOUND_SW:
						switch (event_report.value_lsb){
							case ON:
								printf("\rset response snd switch on\r");
								io_write_clear_ack(set_response_sound_on,6);
								session_data.response_sound = true;
							break;
													
							case OFF:
								printf("\rset response snd switch off\r");
								io_write_clear_ack(set_response_sound_off,6);
								session_data.response_sound = false;
							break;													
						}
						break;		
						default:
						;
					}
					
				break;
						
				case KEYBOARD:
				
					printf("keyboard button pressed, button value: %u\r", event_report.value_lsb);
					
					click_length = CLICK_SOUND_LENGTH;

					uint32_t prev_kb_value = kb_value;

					switch (event_report.value_lsb){
						case KEYBOARD_BACK:
							if (kb_value != 0){
								kb_value = kb_value / 10;
							}
							printf("back\r");
							
						break;
						
						case KEYBOARD_OK:
							printf("keyboard ok\r");							
							printf("kb value: %lu\r", kb_value);
							
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
							
							printf("kb return value: %lu\r", kb_value);
							
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
														
							printf("keyboard: ok, switch back to form %u\r", session_data.previous_form);							
						break;
						
						case KEYBOARD_CANCEL:
							switch_to_form(session_data.previous_form);
							printf("keyboard: cancel, switch back to form %u\r", session_data.previous_form);							
						break;
						
						default:
						if ((event_report.value_lsb>0x29) && (event_report.value_lsb<0x3a)){
							unsigned char digit=event_report.value_lsb-0x30;
													
							kb_value = kb_value * 10 + digit;
							if (kb_value > maxvalue){
								kb_value = prev_kb_value;
							}							
							printf("keyboard, number: %i\r", digit);							
						}
						break;
					}
					printf("kb value: %lu, prev kb value: %lu\r", kb_value, prev_kb_value);
					print_no(kb_value, 11);
					
				break;
				default:
				;					
			}
			
			//printf("Event received from %u, index: %u\r", event_report.object_id, event_report.object_index);
			
			//snprintf(buffer, 64, "%i\r", digit);
			//cdcdf_acm_write((uint8_t *) buffer, strnlen(buffer, 64));
		}		
	}
}