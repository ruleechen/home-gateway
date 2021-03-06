#ifndef __EC_APP_BLE_PERIPHERAL_H__
#define __EC_APP_BLE_PERIPHERAL_H__

#include "stdint.h"

extern ec_core_gpio_pin_e vic_gpio_uart_tx;
extern ec_core_gpio_pin_e vic_gpio_uart_rx;
extern ec_core_gpio_pin_e vic_gpio_input;
extern ec_core_gpio_pin_e vic_gpio_output;
extern ec_core_adc_ch_e vic_gpio_adc;

// 1:允许通过蓝牙无线升级程序，0：禁止无线升级程序，需要重新上电，拉高BOOT引脚才能进入下载模式。
extern uint8_t vic_ble_ota_en;

extern uint16_t vic_adc_value;
extern uint16_t vic_adc_voltage;

extern uint8_t vic_client_authenticated;
extern uint8_t vic_on_state;
extern uint8_t vic_on_state_sent;

extern void vic_emit_on_state(void);
extern void vic_handle_incoming_message(char* data, uint8_t len);

extern void ec_app_ble_peripheral_set_ota_en(uint8_t p); //开启或关闭OTA 默认开启
extern void ec_app_ble_peripheral_register_event(void);  // 注册蓝牙事件回调

#endif
