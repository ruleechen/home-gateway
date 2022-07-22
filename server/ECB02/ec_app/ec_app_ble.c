#include "ec_core.h"
#include "ec_app_ble_peripheral.h"

void ec_app_ble_param_init(void) {
  // 默认从机，PERIPHERAL
  // ec_core_set_role(EC_CORE_BLE_ROLE_PERIPHERAL);

  // 配置发射功率，默认3dbm
  // ec_core_ble_set_power(EC_CORE_BLE_POWER_3DBM);

  // 配置UUID - 16bit uuid
  ec_core_ble_set_suuid("1002");
  ec_core_ble_set_ruuid("1003");
  ec_core_ble_set_wuuid("1004");

  // 配置UUID - 128bit uuid
  // ec_core_ble_set_suuid("9317FB6C-E8BB-4943-B777-236B5AD78128");
  // ec_core_ble_set_ruuid("F53CB4F8-0EFC-4EC2-87B8-F330CD97A6FC");
  // ec_core_ble_set_wuuid("AAA5DC3F-5D20-463E-9B25-F56274DF0B7C");

  // 打开或关闭OTA升级
  ec_core_ble_peripheral_set_ota_en(vic_ble_ota_en);

  // 根据MAC地址修改蓝牙名字
  uint8_t mac[6] = {0};
  ec_core_ble_get_mac(mac); //获取MAC地址
  char buf[25] = {0};
  sprintf(buf, "RS019_%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  ec_core_ble_peripheral_set_name((uint8_t *)buf, 18);
  // ec_core_ble_peripheral_set_name("BT_123", strlen("BT_123"));

  // 厂商自定义数据 - "RuleeSmart"
  uint8_t data[10] = {0x52, 0x75, 0x6C, 0x65, 0x65, 0x53, 0x6D, 0x61, 0x72, 0x74};
  ec_core_ble_peripheral_set_manufacturer_data(data, sizeof(data));

  // 设置广播间隔 200ms
  // ec_core_ble_peripheral_set_adv_int(320);
  // ec_core_ble_peripheral_reset_adv();

  // 注册蓝牙回调
  ec_app_ble_peripheral_register_event();
}
