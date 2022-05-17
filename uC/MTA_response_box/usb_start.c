
//char hello[] = "S down\r\n";
//cdcdf_acm_write((uint8_t *)hello, 8);

#include "atmel_start.h"
#include "usb_start.h"
#include <global.h>


/* Max LUN number */
#define CONF_USB_MSC_MAX_LUN 0

static uint8_t multi_desc_bytes[] = {COMPOSITE_DESCES_LS_FS};
static struct usbd_descriptors multi_desc[] = {{multi_desc_bytes, multi_desc_bytes + sizeof(multi_desc_bytes)}};

/** Ctrl endpoint buffer */
static uint8_t ctrl_buffer[64];

static struct hiddf_kb_key_descriptors key_array[5]={	{HID_A, false, HID_KB_KEY_UP},
														{HID_B, false, HID_KB_KEY_UP},
														{HID_C, false, HID_KB_KEY_UP},
														{HID_D, false, HID_KB_KEY_UP},
														{HID_S, false, HID_KB_KEY_UP}
														};
static bool key_changed;	
static uint8_t trigger_delay;
static uint8_t repress_delay;
extern struct session session_data;
extern uint16_t usb_disconnected;

static uint8_t *cdcdf_demo_buf;

static bool     cdcdf_demo_cb_bulk_out(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	cdcdf_acm_write(cdcdf_demo_buf, count); /* Echo data */
	return false;                           /* No error. */
}

static bool cdcdf_demo_cb_bulk_in(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	cdcdf_acm_read((uint8_t *)cdcdf_demo_buf, CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKIN_MAXPKSZ_HS); /* Another read */
	return false;                                                                                 /* No error. */
}

static bool cdcdf_demo_cb_state_c(usb_cdc_control_signal_t state)
{
	if (state.rs232.DTR) {
		/* After connection the R/W callbacks can be registered */
		cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)cdcdf_demo_cb_bulk_out);
		cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)cdcdf_demo_cb_bulk_in);
		/* Start Rx */
		cdcdf_acm_read((uint8_t *)cdcdf_demo_buf, CONF_USB_COMPOSITE_CDC_ACM_DATA_BULKIN_MAXPKSZ_HS);
	}
	return false; /* No error. */
}

void cdcdf_acm_demo_init(uint8_t *bulk_packet_buffer)
{
	cdcdf_demo_buf = bulk_packet_buffer;
	cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)cdcdf_demo_cb_state_c);
}

static void hiddf_sof_event(void)
{
	if (repress_delay){
		repress_delay --;
	}
	if (usb_disconnected){
		usb_disconnected --;
	}
	
	if (session_data.session_running) {		

		if (session_data.trigger){
			if (key_array[4].state == HID_KB_KEY_UP){
				key_array[4].state = HID_KB_KEY_DOWN;
				key_changed=true;
				sync_beep_length = SYNC_BEEP_LENGTH;
			}
			else{
				trigger_delay += 1;
				if (trigger_delay > 10){
					session_data.trigger = false;
					trigger_delay = 0;
				}
			}
		}			
				
		else if (key_array[4].state == HID_KB_KEY_DOWN){
			key_array[4].state = HID_KB_KEY_UP;
			key_array[0].state = HID_KB_KEY_UP;
			key_array[1].state = HID_KB_KEY_UP;
			key_array[2].state = HID_KB_KEY_UP;
			key_array[3].state = HID_KB_KEY_UP;
			key_changed=true;
			repress_delay=10;
		}				
					
		else if (!repress_delay){ 
					
			if (gpio_get_pin_level(I1) == true){
				if ((key_array[0].state == HID_KB_KEY_UP) && (key_array[4].state == HID_KB_KEY_UP)){		//****************************** késleltetés!!!!!!!!!!!!!!!!!!!!!
					key_array[0].state = HID_KB_KEY_DOWN;
					session_data.simulated_A=false;
					key_changed=true;
					beep_sound_length = BEEP_SOUND_LENGTH;
				}
			}
			else {
				if (key_array[0].state == HID_KB_KEY_DOWN){
					key_array[0].state = HID_KB_KEY_UP;				//****************************** késleltetés!!!!!!!!!!!!!!!!!!!!!
					key_changed=true;
				}
			}	
			
			if (gpio_get_pin_level(I2) == true){
				if ((key_array[1].state == HID_KB_KEY_UP) && (key_array[4].state == HID_KB_KEY_UP)){		//****************************** késleltetés!!!!!!!!!!!!!!!!!!!!!
					key_array[1].state = HID_KB_KEY_DOWN;
					session_data.simulated_B=false;
					key_changed=true;
					beep_sound_length = BEEP_SOUND_LENGTH;
				}
			}
			else {
				if (key_array[1].state == HID_KB_KEY_DOWN){
					key_array[1].state = HID_KB_KEY_UP;				//****************************** késleltetés!!!!!!!!!!!!!!!!!!!!!
					key_changed=true;
				}
			}				
				
				if (gpio_get_pin_level(I3) == true){
					if ((key_array[2].state == HID_KB_KEY_UP) && (key_array[4].state == HID_KB_KEY_UP)){
						key_array[2].state = HID_KB_KEY_DOWN;
						session_data.simulated_C=false;
						key_changed=true;
						beep_sound_length = BEEP_SOUND_LENGTH;
					}
				}
				else {
					if (key_array[2].state == HID_KB_KEY_DOWN){
						key_array[2].state = HID_KB_KEY_UP;
						key_changed=true;
					}
				} 
		 
			if (gpio_get_pin_level(I4) == true){
				if ((key_array[3].state == HID_KB_KEY_UP) && (key_array[4].state == HID_KB_KEY_UP)){
					key_array[3].state = HID_KB_KEY_DOWN;
					session_data.simulated_D=false;
					key_changed=true;
					beep_sound_length = BEEP_SOUND_LENGTH;
				}
			}
			else {
				if (key_array[3].state == HID_KB_KEY_DOWN){
					key_array[3].state = HID_KB_KEY_UP;
					key_changed=true;
				}
			}
		}
	}
		
	else if ((session_data.manual_trigger) || (session_data.simulation_mode)) {		
		
		
		if (session_data.trigger){

			if (key_array[4].state == HID_KB_KEY_UP){
				key_array[4].state = HID_KB_KEY_DOWN;
				key_changed=true;
			}
			else{
				trigger_delay += 1;
				if (trigger_delay > 10){
					session_data.trigger = false;
					trigger_delay = 0;
				}
			}
		}	
		else if (key_array[4].state == HID_KB_KEY_DOWN){
			key_array[4].state = HID_KB_KEY_UP;
			key_array[0].state = HID_KB_KEY_UP;
			key_array[1].state = HID_KB_KEY_UP;
			key_array[2].state = HID_KB_KEY_UP;
			key_array[3].state = HID_KB_KEY_UP;
			key_changed=true;
			repress_delay=10;
		}			
			
		else if (!repress_delay){ 
			if  (session_data.simulated_A){
				if ((key_array[0].state == HID_KB_KEY_UP) && (key_array[4].state == HID_KB_KEY_UP)){		//****************************** késleltetés!!!!!!!!!!!!!!!!!!!!!
					key_array[0].state = HID_KB_KEY_DOWN;
				
					key_changed=true;
				}
			}
			else {
				if (key_array[0].state == HID_KB_KEY_DOWN){
					key_array[0].state = HID_KB_KEY_UP;				//****************************** késleltetés!!!!!!!!!!!!!!!!!!!!!
					key_changed=true;
				}
			}	
			
			if (session_data.simulated_B){
				if ((key_array[1].state == HID_KB_KEY_UP) && (key_array[4].state == HID_KB_KEY_UP)){		//****************************** késleltetés!!!!!!!!!!!!!!!!!!!!!
					key_array[1].state = HID_KB_KEY_DOWN;
				
					key_changed=true;
				}
			}
			else {
				if (key_array[1].state == HID_KB_KEY_DOWN){
					key_array[1].state = HID_KB_KEY_UP;				//****************************** késleltetés!!!!!!!!!!!!!!!!!!!!!
					key_changed=true;
				}
			}				
				
				if (session_data.simulated_C){
					if ((key_array[2].state == HID_KB_KEY_UP) && (key_array[4].state == HID_KB_KEY_UP)){
						key_array[2].state = HID_KB_KEY_DOWN;
					
						key_changed=true;
					}
				}
				else {
					if (key_array[2].state == HID_KB_KEY_DOWN){
						key_array[2].state = HID_KB_KEY_UP;
						key_changed=true;
					}
				} 
		 
			if (session_data.simulated_D){
				if ((key_array[3].state == HID_KB_KEY_UP) && (key_array[4].state == HID_KB_KEY_UP)){
					key_array[3].state = HID_KB_KEY_DOWN;
				
					key_changed=true;
				}
			}
			else {
				if (key_array[3].state == HID_KB_KEY_DOWN){
					key_array[3].state = HID_KB_KEY_UP;
					key_changed=true;
				}
			}
		}
	}
		
	else{
		if (key_array[0].state == HID_KB_KEY_DOWN){
			key_array[0].state = HID_KB_KEY_UP;
			key_changed=true;
		}
		if (key_array[1].state == HID_KB_KEY_DOWN){
			key_array[1].state = HID_KB_KEY_UP;
			key_changed=true;
		}
		if (key_array[2].state == HID_KB_KEY_DOWN){
			key_array[2].state = HID_KB_KEY_UP;
			key_changed=true;
		}
		if (key_array[3].state == HID_KB_KEY_DOWN){
			key_array[3].state = HID_KB_KEY_UP;
			key_changed=true;			
		}		
		if (key_array[4].state == HID_KB_KEY_DOWN){
			key_array[4].state = HID_KB_KEY_UP;
			key_changed=true;			
		}
	}
	
	if (key_changed) {
		hiddf_keyboard_keys_state_change(key_array, 5);
		key_changed = false;
	}
}


static struct usbdc_handler hiddf_sof_event_h = {NULL, (FUNC_PTR)hiddf_sof_event};

void hiddf_init()
{
	usbdc_register_handler(USBDC_HDL_SOF, &hiddf_sof_event_h);
}

void composite_device_init(void)
{
	printf("\r\nusbdc init\r\n");
	/* usb stack init */
	usbdc_init(ctrl_buffer);

	/* usbdc_register_funcion inside */
	printf("\r\ncdcf_acm init\r\n");
	cdcdf_acm_init();

	hiddf_mouse_init(); //kell!!!!!!
	printf("\r\nkeyboard init\r\n");
	key_changed=false;
	hiddf_keyboard_init();
	hiddf_init();
}

void composite_device_start(void)
{
	printf("\r\nusbdc start\r\n");
	usbdc_start(multi_desc);
	printf("\r\nusbdc attach\r\n");
	usbdc_attach();
}

void usb_init(void)
{
	printf("\r\ncomposite device init\r\n");
	composite_device_init();
}
