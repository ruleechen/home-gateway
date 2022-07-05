#include "ec_core.h"
#include "ec_app_flash.h"

// 1: 允许通过蓝牙无线升级程序
// 0: 禁止无线升级程序，需要重新上电，拉高BOOT引脚才能进入下载模式
uint8_t ec_app_ble_peripheral_ota_en = 1;

// 1: ON state is true
// 0: ON state is false
uint8_t victor_on_state = 0;
uint8_t victor_on_state_sent = 0;

void ec_app_ble_peripheral_set_ota_en(uint8_t p) { //开启或关闭OTA 默认开启
  ec_app_ble_peripheral_ota_en = p;                //修改内存
  ec_app_flash_sys_param_write();                  //保存到flash
  ec_core_delay_ms(20);                            //延迟20ms
  ec_core_sys_soft_reset();                        //系统复位
}

static uint8_t victor_start_with(uint8_t* data, uint8_t* find, uint8_t from) {
  uint8_t i = 0;
  while (data[i + from] == find[i]) {
    i++;
  }
  return i == sizeof(find) ? 1 : 0;
}

void victor_emit_state(void) {
  if (victor_on_state == 0) {
    ec_core_ble_send("ON:0", 4);
  } else {
    ec_core_ble_send("ON:1", 4);
  }
}

static void victor_heartbeat(void) {
  if (victor_on_state == 0) {
    ec_core_ble_send("HB:ON:0", 7);
  } else {
    ec_core_ble_send("HB:ON:1", 7);
  }
}

static void ec_app_ble_peripheral_connect_event(void) { //蓝牙连接回调
  ec_core_uart0_printf("ble peripheral connect\r\n");
  ec_core_sw_timer_stop(EC_CORE_SW_TIMER1);
  ec_core_sw_timer_start(EC_CORE_SW_TIMER1, 30000, victor_heartbeat); // heartbeat each 30s
}

static void ec_app_ble_peripheral_disconnect_event(void) { //蓝牙断开回调
  ec_core_uart0_printf("ble peripheral disconnect\r\n");
  ec_core_sw_timer_stop(EC_CORE_SW_TIMER1);
}

static void ec_app_ble_peripheral_notify_enable_event(void) { //蓝牙订阅打开回调
  ec_core_uart0_printf("ble peripheral notify enable\r\n");
}

static void ec_app_ble_peripheral_notify_disable_event(void) { //蓝牙订阅关闭回调
  ec_core_uart0_printf("ble peripheral notify disable\r\n");
}

static void ec_app_ble_peripheral_receive_event(uint8_t* data, uint8_t len) { //蓝牙数据接收回调
  ec_core_uart0_printf("ble peripheral receive len=%d\r\n", len);
  ec_core_uart_send(EC_CORE_UART0, data, len); //蓝牙数据转发到串口
  // ec_core_uart0_printf("\r\n");

  if (data[0] == '@') {
    ec_app_ble_peripheral_set_ota_en(1); //开启OTA
  } else if (data[0] == '#') {
    ec_app_ble_peripheral_set_ota_en(0); //关闭OTA
  }

  if (victor_start_with(data, "QY", 0)) { // QUERY
    victor_emit_state();
  } else if (victor_start_with(data, "AM", 0)) { // ALARM
    ec_core_gpio_write(EC_CORE_GPIO_P8, EC_CORE_GPIO_LEVEL_L);
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
