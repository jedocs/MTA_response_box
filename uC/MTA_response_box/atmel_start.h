#ifndef ATMEL_START_H_INCLUDED
#define ATMEL_START_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "driver_init.h"
#include "usb_start.h"
#include "stdio_start.h"
#include "persistent_storage_start.h"

/**
 * Initializes MCU, drivers and middleware in the project
 **/
void atmel_start_init(void);
//static void sync_trigger(void);
//static void button(void);
//void start_session(void);
//void stop_session(void);
//void ADC_init(void);
//static void convert_cb_ADC_0(const struct adc_async_descriptor *const descr, const uint8_t channel);

//static void TIMER_task1_cb(const struct timer_task *const timer_task);
//static void TIMER_task2_cb(const struct timer_task *const timer_task);
void TIMER_init(void);
void uart_init(void);

#ifdef __cplusplus
}
#endif
#endif
