#include <atmel_start.h>

/**
 * Initializes MCU, drivers and middleware in the project
 **/
void atmel_start_init(void)
{
	system_init();
	stdio_redirect_init();
	usb_init();
	
	printf("\r\nbefore composite device start\r\n");
	composite_device_start();	
	persistent_storage_init();
}
