#ifndef __VICTOR_GPIO_CONFIG_H__
#define __VICTOR_GPIO_CONFIG_H__

#include "stdint.h"
#include "ec_core_adc.h"
#include "ec_core_gpio.h"

extern ec_core_gpio_pin_e victor_gpio_uart_tx;
extern ec_core_gpio_pin_e victor_gpio_uart_rx;
extern ec_core_gpio_pin_e victor_gpio_input;
extern ec_core_gpio_pin_e victor_gpio_output;
extern ec_core_adc_ch_e   victor_gpio_adc;

#endif
