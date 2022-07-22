#include "main.h"

#ifdef EC_APP_MAIN

#include "ec_core.h"
#include "ec_app_flash.h"
#include "ec_app_ble_peripheral.h"
#include "ec_app_ble.h"

#define EC_APP_UART0_TX_BUF_SIZE 1024                 //串口0发送缓冲区大小，可以根据需要调整
#define EC_APP_UART0_RX_BUF_SIZE 1024                 //串口0接收缓冲区大小，可以根据需要调整
uint8_t uart0_tx_buf[EC_APP_UART0_TX_BUF_SIZE] = {0}; //串口0发送缓冲区
uint8_t uart0_rx_buf[EC_APP_UART0_RX_BUF_SIZE] = {0}; //串口0接收缓冲区

void vic_uart0_rx(uint8_t* buf, uint16_t len) {
  ec_core_uart_send(EC_CORE_UART0, buf, len); // ECHO 回显
  vic_handle_incoming_message((char*)buf, len);
}

void vic_uart0_init(void) {
  ec_core_uart_init(EC_CORE_UART0, 115200, EC_CORE_UART_PARITY_NONE, vic_gpio_uart_tx, vic_gpio_uart_rx, uart0_tx_buf, EC_APP_UART0_TX_BUF_SIZE, uart0_rx_buf, EC_APP_UART0_RX_BUF_SIZE, vic_uart0_rx);
  // print core version
  uint8_t ver[3] = {0};
  ec_core_ver(ver);
  ec_core_uart0_printf("ECB02 SDK %d.%d.%d\r\n", ver[0], ver[1], ver[2]);
}

void vic_debounce_handler() {
  ec_core_sw_timer_stop(EC_CORE_SW_TIMER2);
  if (
    vic_client_authenticated &&
    vic_on_state != vic_on_state_sent
  ) {
    vic_emit_on_state();
  }
}

void vic_debounce_emit() {
  ec_core_sw_timer_stop(EC_CORE_SW_TIMER2);
  ec_core_sw_timer_start(EC_CORE_SW_TIMER2, 50, vic_debounce_handler);
}

void vic_set_on_state(uint8_t state) {
  vic_on_state = state;
  if (vic_client_authenticated) {
    vic_debounce_emit();
  }
}

void input_rising(void) { vic_set_on_state(0); }
void input_falling(void) { vic_set_on_state(1); }

void vic_gpio_init(void) {
  // input
  ec_core_gpio_in_init(vic_gpio_input, EC_CORE_GPIO_PULL_UP_S);           // 初始化 上拉输入
  ec_core_gpio_int_register(vic_gpio_input, input_rising, input_falling); // 中断使能
  vic_set_on_state(ec_core_gpio_read(vic_gpio_input) == EC_CORE_GPIO_LEVEL_L ? 1 : 0);
  // output
  ec_core_gpio_out_init(vic_gpio_output, EC_CORE_GPIO_LEVEL_L); // 初始化 上拉输出
}

int main(void) {
  ec_core_sys_clk_set(EC_CORE_SYS_CLK_48M); //配置系统时钟

  ec_app_flash_sys_param_read(); // 从flash读取系统参数
  ec_app_ble_param_init();       // 初始化蓝牙相关的参数

  ec_core_init(); //蓝牙内核初始化
  ec_core_adc_init();

  vic_uart0_init();
  vic_gpio_init();

  // ec_core_sw_watchdog_init(EC_CORE_SW_TIMER6, 2, 3); //初始化软件看门狗，广播超时时间2分钟，蓝牙连接超时时间3分钟
  ec_core_sleep_enable(); // ec_core_sleep_disable(); //禁止睡眠，串口可以接收数据
  ec_core_main_loop_run(); //系统主循环
}

#endif
