/******************************************************************************
 *  Copyright © 2019 Shenzhen ECIOT Technology Co., Ltd All Rights Reserved
 *-----------------------------------------------------------------------------
 * @file        ec_app_flash.h
 * @brief       flash control
 * @author      mo@eciot.cn (qq:2201920828,v:eciot_mo)
 * @date        2022-02-13
 * @version     1.0
 ******************************************************************************/

#ifndef __EC_APP_FLASH_H__
#define __EC_APP_FLASH_H__

#include "stdint.h"

extern void ec_app_flash_sys_param_write(void); //系统参数保存到flash
extern void ec_app_flash_sys_param_read(void);  //从flash中读取系统参数

#endif
