#include "ec_core.h"
#include "ec_app_ble_peripheral.h"

void ec_app_ble_param_init(void) { //蓝牙参数初始化
  // ec_core_ble_set_power(EC_CORE_BLE_POWER_3DBM);//配置发射功率，默认3dbm
  // ec_core_set_role(EC_CORE_BLE_ROLE_PERIPHERAL); //默认从机，PERIPHERAL

  // 配置UUID
  // 16bit uuid
  ec_core_ble_set_suuid("1002");
  ec_core_ble_set_ruuid("1003");
  ec_core_ble_set_wuuid("1004");

  // 128bit uuid
  // ec_core_ble_set_suuid("9317FB6C-E8BB-4943-B777-236B5AD78128");
  // ec_core_ble_set_ruuid("F53CB4F8-0EFC-4EC2-87B8-F330CD97A6FC");
  // ec_core_ble_set_wuuid("AAA5DC3F-5D20-463E-9B25-F56274DF0B7C");

  ec_core_ble_peripheral_set_ota_en(vic_ble_ota_en); //打开或关闭OTA升级

  uint8_t mac[6] = {0};
  ec_core_ble_get_mac(mac); //获取MAC地址
  char buf[25] = {0};
  sprintf(buf, "RS019_%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  ec_core_ble_peripheral_set_name((uint8_t *)buf, 18); //根据MAC地址修改蓝牙名字
  // ec_core_ble_peripheral_set_name("BT_123", strlen("BT_123"));

  uint8_t data[11] = {0x86, 0x05, 0x99, 0x16, 0x11, 0x14, 0x83, 0x09, 0x97, 0x14, 0x16};
  ec_core_ble_peripheral_set_manufacturer_data(data, sizeof(data)); //修改厂商自定义数据

  ec_app_ble_peripheral_register_event(); //注册蓝牙回调
}
