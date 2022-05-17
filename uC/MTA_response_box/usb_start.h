/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#ifndef USB_DEVICE_MAIN_H
#define USB_DEVICE_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "cdcdf_acm.h"
#include "hiddf_mouse.h"
#include "hiddf_keyboard.h"
#include "hiddf_generic.h"
#include "mscdf.h"
#include "composite_desc.h"

/**
 * \brief Initialize device and attach functions
 */
void composite_device_init(void);
/**
 * \brief Start the device
 */
void composite_device_start(void);
/**
 * Example of using Composite Function.
 * \note
 * In this example, we will use a PC as a USB host:
 * - Connect the DEBUG USB on XPLAINED board to PC for program download.
 * - Connect the TARGET USB on XPLAINED board to PC for running program.
 */
void composite_device_example(void);

/**
 * \brief Initialize CDC ACM Echo demo
 * \param[in] bulk_packet_buffer Pointer to buffer for a bulk packet
 */
void cdcdf_acm_demo_init(uint8_t *bulk_packet_buffer);

/**
 * \brief Initialize HID

 */
void hiddf_init();

/**
 * \berif Initialize USB
 */
void usb_init(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // USB_DEVICE_MAIN_H
