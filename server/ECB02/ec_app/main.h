/******************************************************************************
 *  Copyright © 2019 Shenzhen ECIOT Technology Co., Ltd All Rights Reserved
 *-----------------------------------------------------------------------------
 * @file        main.h
 * @brief       app demo
 * @author      mo@eciot.cn (qq:2201920828,v:eciot_mo)
 * @date        2022-02-13
 * @version     1.0
 ******************************************************************************/

#ifndef __MAIN_H__
#define __MAIN_H__

//通过宏定义，选择需要参与编译的代码

#define EC_APP_MAIN // 串口透传App

#define _EC_APP_GPIO_TEST             // GPIO示例
#define _EC_APP_UART_TEST             // UART示例
#define _EC_APP_TIMER_TEST            // 定时器示例
#define _EC_APP_PWM_TEST              // PWM示例
#define _EC_APP_CLK_TEST              // 系统时钟频率设置示例
#define _EC_APP_SLEEP_TEST            // 睡眠控制示例
#define _EC_APP_ADC_TEST              // ADC示例
#define _EC_APP_FLASH_TEST            // flash示例
#define _EC_APP_RAND_TEST             // 随机数生成器示例
#define _EC_APP_POWER_OFF_TEST        // 关机模式示例
#define _EC_APP_PRINTF_TEST           // printf示例
#define _EC_APP_LED_TEST              // LED示例
#define _EC_APP_KEY_TEST              // KEY示例
#define _EC_APP_I2C_EEPROM_TEST       // I2C EEPROM示例
#define _EC_APP_SPI_SSD1306_OLED_TEST // SPI OLED显示屏示例
#define _EC_APP_HW_WDG_TEST           // 硬件看门狗示例
#define _EC_APP_SW_WDG_TEST           // 软件看门狗示例

#define _EC_APP_VER_TEST               // 获取软件版本示例
#define _EC_APP_MAC_TEST               // 获取MAC地址示例
#define _EC_APP_NAME_TEST              // 配置蓝牙名字示例
#define _EC_APP_OTA_TEST               // OTA无线升级配置示例
#define _EC_APP_BLE_PERIPHERAL_TEST    // 蓝牙从机通讯示例
#define _EC_APP_BLE_UUID_TEST          // 修改UUID示例
#define _EC_APP_MTU_TEST               // 查询MTU示例
#define _EC_APP_POWER_TEST             // 修改蓝牙发射功率示例
#define _EC_APP_ADV_INT_TEST           // 修改广播间隙示例
#define _EC_APP_MANUFACTURER_DATA_TEST // 修改厂商自定义数据示例
#define _EC_APP_CONNECT_PARAM_TEST     // 修改蓝牙连接参数示例
#define _EC_APP_CUSTOM_ADV_TEST        // 设置自定义蓝牙广播包、扫描应答包示例
#define _EC_APP_BEACON_TEST            // beacon蓝牙定位器示例
#define _EC_APP_CENTRAL_SCAN_TEST      // 蓝牙主机模式扫描示例
#define _EC_APP_CENTRAL_BOND_TEST      // 蓝牙主机模式绑定从机示例

#define _EC_APP_SCT_TEST // sct文件修改示例，合理使用flash空间
#define _EC_APP_RAM_TEST // RAM测试

#endif
