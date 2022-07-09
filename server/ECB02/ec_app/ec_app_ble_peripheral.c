#include "ec_core.h"
#include "ec_app_flash.h"

ec_core_gpio_pin_e vic_gpio_uart_tx = EC_CORE_GPIO_P1; // UART0 tx pin
ec_core_gpio_pin_e vic_gpio_uart_rx = EC_CORE_GPIO_P2; // UART0 rx pin
ec_core_gpio_pin_e vic_gpio_input = EC_CORE_GPIO_P3;   // switch on/off input pin
ec_core_gpio_pin_e vic_gpio_output = EC_CORE_GPIO_P4;  // alarm on/off output pin
ec_core_adc_ch_e vic_gpio_adc = EC_CORE_ADC_CH4_P10;   // battery detection

// 1: 允许通过蓝牙无线升级程序
// 0: 禁止无线升级程序，需要重新上电，拉高BOOT引脚才能进入下载模式
uint8_t vic_ble_ota_en = 1;

//寄存器值/电压值
uint16_t vic_adc_value = 0;
uint16_t vic_adc_voltage = 0;

// 1: ON state is true
// 0: ON state is false
uint8_t vic_on_state = 0;
uint8_t vic_on_state_sent = 0;

void ec_app_ble_peripheral_set_ota_en(uint8_t p) { //开启或关闭OTA 默认开启
  vic_ble_ota_en = p;                              //修改内存
  ec_app_flash_sys_param_write();                  //保存到flash
  ec_core_delay_ms(20);                            //延迟20ms
  ec_core_sys_soft_reset();                        //系统复位
}

static void vic_measure_battery(void) {
  uint16_t value, voltage; //寄存器值和电压值
  ec_core_adc_get(vic_gpio_adc, EC_CORE_ADC_RANGE_3200MV, EC_CORE_ADC_CALIBRATION_ENABLED, &value, &voltage);
  vic_adc_value = value;
  vic_adc_voltage = voltage;
}

static uint8_t vic_start_with(uint8_t* data, uint8_t from, char* find) {
  uint8_t i = 0;
  while (data[i + from] == find[i]) {
    i++;
  }
  return i == strlen(find) ? 1 : 0;
}

static void vic_uart_emit(uint8_t* title, uint8_t* data, uint16_t len) {
  ec_core_uart0_printf("%s [", title);
  ec_core_uart_send(EC_CORE_UART0, data, len); //蓝牙数据转发到串口
  ec_core_uart0_printf("]\r\n");
}

static void vic_ble_emit(uint8_t* data, uint16_t len) {
  ec_core_ble_send(data, len);
  vic_uart_emit("ble peripheral emit", data, len);
}

// https://www.runoob.com/cprogramming/c-function-sprintf.html
static void vic_emit_battery_state(void) {
  char buf[7] = {0};
  sprintf(buf, "BT:%04d", vic_adc_voltage);
  vic_ble_emit((uint8_t*)buf, 7);
}

void vic_emit_on_state(void) {
  if (vic_on_state == 0) {
    vic_ble_emit("ON:0", 4);
  } else {
    vic_ble_emit("ON:1", 4);
  }
}

static void vic_emit_heartbeat_state(void) {
  if (vic_on_state == 0) {
    vic_ble_emit("HB:ON:0", 7);
  } else {
    vic_ble_emit("HB:ON:1", 7);
  }
}

static void vic_heartbeat(void) {
  vic_measure_battery();
  vic_emit_battery_state();
  vic_emit_heartbeat_state();
}

static void ec_app_ble_peripheral_connect_event(void) { //蓝牙连接回调
  vic_measure_battery();
  ec_core_uart0_printf("ble peripheral connect\r\n");
}

static void ec_app_ble_peripheral_disconnect_event(void) { //蓝牙断开回调
  ec_core_sw_timer_stop(EC_CORE_SW_TIMER1);
  ec_core_uart0_printf("ble peripheral disconnect\r\n");
}

static void ec_app_ble_peripheral_notify_enable_event(void) { //蓝牙订阅打开回调
  ec_core_sw_timer_stop(EC_CORE_SW_TIMER1);
  ec_core_sw_timer_start(EC_CORE_SW_TIMER1, 30000, vic_heartbeat); // heartbeat each 30s
  ec_core_uart0_printf("ble peripheral notify enable\r\n");
}

static void ec_app_ble_peripheral_notify_disable_event(void) { //蓝牙订阅关闭回调
  ec_core_sw_timer_stop(EC_CORE_SW_TIMER1);
  ec_core_uart0_printf("ble peripheral notify disable\r\n");
}

static void ec_app_ble_peripheral_receive_event(uint8_t* data, uint8_t len) { //蓝牙数据接收回调
  vic_uart_emit("ble peripheral receive", data, len);

  if (vic_start_with(data, 0, "ON")) { // QUERY_ON
    vic_emit_on_state();
  } else if (vic_start_with(data, 0, "BT")) { // QUERY_BATTERY
    vic_emit_battery_state();
  } else if (vic_start_with(data, 0, "AM")) { // SET_ALARM
    ec_core_gpio_write(vic_gpio_output, (data[3] == '1' ? EC_CORE_GPIO_LEVEL_L : EC_CORE_GPIO_LEVEL_H));
  } else if (vic_start_with(data, 0, "OTA")) { // SET_OTA
    ec_app_ble_peripheral_set_ota_en(data[4] == '1' ? 1 : 0);
  }

  ec_core_sw_watchdog_feed(); //软件看门狗喂狗
}

void ec_app_ble_peripheral_register_event(void) {                                                                               //注册蓝牙事件回调
  ec_core_ble_peripheral_set_event_cb(EC_CORE_BLE_PERIPHERAL_EVENT_CONNECT, ec_app_ble_peripheral_connect_event);               //注册蓝牙连接的回调
  ec_core_ble_peripheral_set_event_cb(EC_CORE_BLE_PERIPHERAL_EVENT_DISCONNECT, ec_app_ble_peripheral_disconnect_event);         //注册蓝牙断开的回调
  ec_core_ble_peripheral_set_event_cb(EC_CORE_BLE_PERIPHERAL_EVENT_NOTIFY_ENABLE, ec_app_ble_peripheral_notify_enable_event);   //注册蓝牙订阅打开的回调
  ec_core_ble_peripheral_set_event_cb(EC_CORE_BLE_PERIPHERAL_EVENT_NOTIFY_DISABLE, ec_app_ble_peripheral_notify_disable_event); //注册蓝牙订阅关闭的回调
  ec_core_ble_peripheral_set_receive_cb(ec_app_ble_peripheral_receive_event);                                                   //注册蓝牙数据接收的回调
}
