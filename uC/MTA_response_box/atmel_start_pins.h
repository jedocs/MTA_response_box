/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAMD21 has 8 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7

#define PWR_SENSE GPIO(GPIO_PORTA, 3)

#define LCD_IO1 GPIO(GPIO_PORTB, 31)
#define DISP_RESET GPIO(GPIO_PORTA, 28)

#define uC_AUD GPIO(GPIO_PORTA, 2)
#define AUDENB GPIO(GPIO_PORTB, 5)

#define TX1 GPIO(GPIO_PORTA, 16)
#define RX1 GPIO(GPIO_PORTA, 17)

#define IIC_SDA GPIO(GPIO_PORTA, 22)
#define IIC_SCL GPIO(GPIO_PORTA, 23)

#define SD_MOSI GPIO(GPIO_PORTA, 8)
#define SD_MISO GPIO(GPIO_PORTA, 11)
#define SD_CD GPIO(GPIO_PORTA, 27)
#define SD_SS GPIO(GPIO_PORTA, 10)
#define SD_SCK GPIO(GPIO_PORTA, 9)

#define EXT_INT GPIO(GPIO_PORTB, 0)

#define SPI_SS GPIO(GPIO_PORTA, 18)
#define SPI_INT GPIO(GPIO_PORTB, 1)
#define LCD_MISO GPIO(GPIO_PORTA, 19)

#define USB_SUSPEND GPIO(GPIO_PORTB, 4)

#define PB1 GPIO(GPIO_PORTB, 7)
#define PB2 GPIO(GPIO_PORTB, 3)
#define PB3 GPIO(GPIO_PORTA, 12)
#define PB4 GPIO(GPIO_PORTB, 30)
#define PB5 GPIO(GPIO_PORTB, 6)

#define RX_PWR GPIO(GPIO_PORTA, 13)

#define I0 GPIO(GPIO_PORTB, 2)
#define I1 GPIO(GPIO_PORTB, 8)
#define I3 GPIO(GPIO_PORTB, 9)
#define I5 GPIO(GPIO_PORTA, 4)
#define I7 GPIO(GPIO_PORTA, 5)
#define I9 GPIO(GPIO_PORTA, 6)
#define I11 GPIO(GPIO_PORTA, 7)
#define I2 GPIO(GPIO_PORTB, 15)
#define I4 GPIO(GPIO_PORTB, 14)
#define I6 GPIO(GPIO_PORTB, 13)
#define I8 GPIO(GPIO_PORTB, 12)
#define I10 GPIO(GPIO_PORTB, 11)
#define I12 GPIO(GPIO_PORTB, 10)

#define T6 GPIO(GPIO_PORTA, 21)
#define T5 GPIO(GPIO_PORTA, 20)
#define T4 GPIO(GPIO_PORTB, 17)
#define T3 GPIO(GPIO_PORTB, 16)
#define T2 GPIO(GPIO_PORTA, 14)
#define T1 GPIO(GPIO_PORTA, 15)

#define TXD GPIO(GPIO_PORTB, 22)
#define RXD GPIO(GPIO_PORTB, 23)

#define USB_DM GPIO(GPIO_PORTA, 24)
#define USB_DP GPIO(GPIO_PORTA, 25)

#endif // ATMEL_START_PINS_H_INCLUDED
