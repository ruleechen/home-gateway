/******************************************************************************
 *  Copyright © 2019 Shenzhen ECIOT Technology Co., Ltd All Rights Reserved
 *-----------------------------------------------------------------------------
 * @file        ec_app_ble_peripheral.h
 * @brief       peripheral control
 * @author      mo@eciot.cn (qq:2201920828,v:eciot_mo)
 * @date        2022-02-13
 * @version     1.0
 ******************************************************************************/

#ifndef __EC_APP_BLE_PERIPHERAL_H__
#define __EC_APP_BLE_PERIPHERAL_H__

#include "stdint.h"

// 1:允许通过蓝牙无线升级程序，0：禁止无线升级程序，需要重新上电，拉高BOOT引脚才能进入下载模式。
extern uint8_t ec_app_ble_peripheral_ota_en;
extern uint8_t ec_app_ble_on_state;

extern void ec_app_ble_peripheral_set_ota_en(uint8_t p); //开启或关闭OTA 默认开启
extern void ec_app_ble_peripheral_register_event(void);  // 注册蓝牙事件回调

#endif
