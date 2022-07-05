/******************************************************************************
 *  Copyright © 2019 Shenzhen ECIOT Technology Co., Ltd All Rights Reserved
 *-----------------------------------------------------------------------------
 * @file        main.c
 * @brief       app demo
 * @author      mo@eciot.cn (qq:2201920828,v:eciot_mo)
 * @date        2022-02-13
 * @version     1.0
 ******************************************************************************/

#include "main.h"

#ifdef EC_APP_MAIN

#include "ec_core.h"
#include "ec_app_flash.h"
#include "ec_app_ble_peripheral.h"
#include "ec_app_ble.h"

#define EC_APP_UART0_TX_BUF_SIZE 1024                 //串口0发送缓冲区大小，可以根据需要调整
#define EC_APP_UART0_RX_BUF_SIZE 1024                 //串口0接收缓冲区大小，可以根据需要调整
#define EC_APP_UART1_TX_BUF_SIZE 1024                 //串口1发送缓冲区大小，可以根据需要调整
#define EC_APP_UART1_RX_BUF_SIZE 1024                 //串口1接收缓冲区大小，可以根据需要调整
uint8_t uart0_tx_buf[EC_APP_UART0_TX_BUF_SIZE] = {0}; //串口0发送缓冲区
uint8_t uart0_rx_buf[EC_APP_UART0_RX_BUF_SIZE] = {0}; //串口0接收缓冲区
uint8_t uart1_tx_buf[EC_APP_UART1_TX_BUF_SIZE] = {0}; //串口1发送缓冲区
uint8_t uart1_rx_buf[EC_APP_UART1_RX_BUF_SIZE] = {0}; //串口1接收缓冲区

void uart0_rx(uint8_t *buf, uint16_t len)
{
  ec_core_ble_send(buf, len); //串口数据转发到蓝牙

  if (strcmp((const char *)buf, "DISC") == 0)
    ec_core_ble_disconnect(); //主动断开蓝牙连接
}

void uart1_rx(uint8_t *buf, uint16_t len) //串口1接收数据中断
{
  ec_core_ble_send(buf, len); //串口数据转发到蓝牙
}

void uart1_init(void) //串口1初始化 波特率精度受时钟频率影响
{
  ec_core_uart_init(EC_CORE_UART1, 115200, EC_CORE_UART_PARITY_NONE,
                    EC_CORE_GPIO_P2, EC_CORE_GPIO_P1,
                    uart1_tx_buf, EC_APP_UART1_TX_BUF_SIZE, uart1_rx_buf, EC_APP_UART1_RX_BUF_SIZE,
                    uart1_rx);
}

uint8_t ec_app_ble_on_state_sent = 0;

void report_handler()
{
  if (ec_app_ble_on_state != ec_app_ble_on_state_sent)
  {
    ec_app_ble_on_state_sent = ec_app_ble_on_state;
    victor_emit_state();
  }
  ec_core_sw_timer_stop(EC_CORE_SW_TIMER2);
}
void report_state(uint8_t state)
{
  ec_app_ble_on_state = state;
  // debounce 100ms
  ec_core_sw_timer_stop(EC_CORE_SW_TIMER2);
  ec_core_sw_timer_start(EC_CORE_SW_TIMER2, 100, report_handler);
}
void int_rising(void) //上升沿中断
{
  ec_core_uart0_printf("int_rising\r\n");
  report_state(0);
}
void int_falling(void) //下降沿中断
{
  ec_core_uart0_printf("int_falling\r\n");
  report_state(1);
}

int main(void)
{
  ec_core_sys_clk_set(EC_CORE_SYS_CLK_48M); //配置系统时钟

  ec_app_flash_sys_param_read(); // 从flash读取系统参数
  ec_app_ble_param_init();       // 初始化蓝牙相关的参数

  ec_core_init(); //蓝牙内核初始化

  //串口0初始化 波特率精度受时钟频率影响
  ec_core_uart_init(EC_CORE_UART0, 115200, EC_CORE_UART_PARITY_NONE,
                    EC_CORE_GPIO_P4, EC_CORE_GPIO_P5,
                    uart0_tx_buf, EC_APP_UART0_TX_BUF_SIZE, uart0_rx_buf, EC_APP_UART0_RX_BUF_SIZE,
                    uart0_rx);
  uart1_init();

  uint8_t ver[3] = {0};
  ec_core_ver(ver);                                                       //读取软件版本
  ec_core_uart0_printf("ECB02 SDK %d.%d.%d\r\n", ver[0], ver[1], ver[2]); //串口0 printf打印

  // input
  ec_core_gpio_in_init(EC_CORE_GPIO_P7, EC_CORE_GPIO_PULL_UP_S);       // 初始化 上拉输入
  ec_core_gpio_int_register(EC_CORE_GPIO_P7, int_rising, int_falling); // 中断使能
  // output
  ec_core_gpio_out_init(EC_CORE_GPIO_P8, EC_CORE_GPIO_PULL_UP_S);       // 初始化 上拉输出

  ec_core_sw_watchdog_init(EC_CORE_SW_TIMER6, 2, 3); //初始化软件看门狗，广播超时时间2分钟，蓝牙连接超时时间3分钟

  // ec_core_sleep_disable(); //禁止睡眠，串口可以接收数据
  ec_core_sleep_enable();
  ec_core_main_loop_run(); //系统主循环
}

#endif
