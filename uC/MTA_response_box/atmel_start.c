#include <atmel_start.h>

/**
 * Initializes MCU, drivers and middleware in the project
 **/
void atmel_start_init(void)
{
	system_init();
	//stdio_redirect_init();
	usb_init();
	
	printf("\rcomposite device start:\r");
	composite_device_start();	
	printf("\rpersistent storage init\r");
	persistent_storage_init();
}
