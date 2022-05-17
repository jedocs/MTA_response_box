/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

static void convert_cb_ADC_0(const struct adc_async_descriptor *const descr, const uint8_t channel)
{
}

/**
 * Example of using ADC_0 to generate waveform.
 */
void ADC_0_example(void)
{
	adc_async_register_callback(&ADC_0, 0, ADC_ASYNC_CONVERT_CB, convert_cb_ADC_0);
	adc_async_enable_channel(&ADC_0, 0);
	adc_async_start_conversion(&ADC_0);
}

static void button_on_PB00_pressed(void)
{
}

static void button_on_PB01_pressed(void)
{
}

static void button_on_PB02_pressed(void)
{
}

static void button_on_PB03_pressed(void)
{
}

static void button_on_PA04_pressed(void)
{
}

static void button_on_PA05_pressed(void)
{
}

static void button_on_PA06_pressed(void)
{
}

static void button_on_PA07_pressed(void)
{
}

static void button_on_PB08_pressed(void)
{
}

static void button_on_PB09_pressed(void)
{
}

static void button_on_PB10_pressed(void)
{
}

static void button_on_PB11_pressed(void)
{
}

static void button_on_PB12_pressed(void)
{
}

static void button_on_PB13_pressed(void)
{
}

static void button_on_PB14_pressed(void)
{
}

static void button_on_PB15_pressed(void)
{
}

/**
 * Example of using EXTERNAL_IRQ_0
 */
void EXTERNAL_IRQ_0_example(void)
{

	ext_irq_register(PIN_PB00, button_on_PB00_pressed);
	ext_irq_register(PIN_PB01, button_on_PB01_pressed);
	ext_irq_register(PIN_PB02, button_on_PB02_pressed);
	ext_irq_register(PIN_PB03, button_on_PB03_pressed);
	ext_irq_register(PIN_PA04, button_on_PA04_pressed);
	ext_irq_register(PIN_PA05, button_on_PA05_pressed);
	ext_irq_register(PIN_PA06, button_on_PA06_pressed);
	ext_irq_register(PIN_PA07, button_on_PA07_pressed);
	ext_irq_register(PIN_PB08, button_on_PB08_pressed);
	ext_irq_register(PIN_PB09, button_on_PB09_pressed);
	ext_irq_register(PIN_PB10, button_on_PB10_pressed);
	ext_irq_register(PIN_PB11, button_on_PB11_pressed);
	ext_irq_register(PIN_PB12, button_on_PB12_pressed);
	ext_irq_register(PIN_PB13, button_on_PB13_pressed);
	ext_irq_register(PIN_PB14, button_on_PB14_pressed);
	ext_irq_register(PIN_PB15, button_on_PB15_pressed);
}

static uint8_t src_data[128];
static uint8_t chk_data[128];
/**
 * Example of using FLASH_0 to read and write Flash main array.
 */
void FLASH_0_example(void)
{
	uint32_t page_size;
	uint16_t i;

	/* Init source data */
	page_size = flash_get_page_size(&FLASH_0);

	for (i = 0; i < page_size; i++) {
		src_data[i] = i;
	}

	/* Write data to flash */
	flash_write(&FLASH_0, 0x3200, src_data, page_size);

	/* Read data from flash */
	flash_read(&FLASH_0, 0x3200, chk_data, page_size);
}

/**
 * Example of using SPI_0 to write "Hello World" using the IO abstraction.
 */
static uint8_t example_SPI_0[12] = "Hello World!";

void SPI_0_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_0, &io);

	spi_m_sync_enable(&SPI_0);
	io_write(io, example_SPI_0, 12);
}

/**
 * Example of using USART_1 to write "Hello World" using the IO abstraction.
 *
 * Since the driver is asynchronous we need to use statically allocated memory for string
 * because driver initiates transfer and then returns before the transmission is completed.
 *
 * Once transfer has been completed the tx_cb function will be called.
 */

static uint8_t example_USART_1[12] = "Hello World!";

static void tx_cb_USART_1(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

void USART_1_example(void)
{
	struct io_descriptor *io;

	usart_async_register_callback(&USART_1, USART_ASYNC_TXC_CB, tx_cb_USART_1);
	/*usart_async_register_callback(&USART_1, USART_ASYNC_RXC_CB, rx_cb);
	usart_async_register_callback(&USART_1, USART_ASYNC_ERROR_CB, err_cb);*/
	usart_async_get_io_descriptor(&USART_1, &io);
	usart_async_enable(&USART_1);

	io_write(io, example_USART_1, 12);
}

void I2C_0_example(void)
{
	struct io_descriptor *I2C_0_io;

	i2c_m_sync_get_io_descriptor(&I2C_0, &I2C_0_io);
	i2c_m_sync_enable(&I2C_0);
	i2c_m_sync_set_slaveaddr(&I2C_0, 0x12, I2C_M_SEVEN);
	io_write(I2C_0_io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using USART_0 to write "Hello World" using the IO abstraction.
 */
void USART_0_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_0, &io);
	usart_sync_enable(&USART_0);

	io_write(io, (uint8_t *)"Hello World!", 12);
}

static struct timer_task TIMER_0_task1, TIMER_0_task2;
/**
 * Example of using TIMER_0.
 */
static void TIMER_0_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_0_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_0_example(void)
{
	TIMER_0_task1.interval = 100;
	TIMER_0_task1.cb       = TIMER_0_task1_cb;
	TIMER_0_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_0_task2.interval = 200;
	TIMER_0_task2.cb       = TIMER_0_task2_cb;
	TIMER_0_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_0, &TIMER_0_task1);
	timer_add_task(&TIMER_0, &TIMER_0_task2);
	timer_start(&TIMER_0);
}

static uint16_t example_DAC_0[10] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900};

static void tx_cb_DAC_0(struct dac_async_descriptor *const descr, const uint8_t ch)
{
	dac_async_write(descr, 0, example_DAC_0, 10);
}

/**
 * Example of using DAC_0 to generate waveform.
 */
void DAC_0_example(void)
{
	dac_async_enable_channel(&DAC_0, 0);
	dac_async_register_callback(&DAC_0, DAC_ASYNC_CONVERSION_DONE_CB, tx_cb_DAC_0);
	dac_async_write(&DAC_0, 0, example_DAC_0, 10);
}
