#ifndef VictorBleModels_h
#define VictorBleModels_h

#include <Arduino.h>

namespace Victor::Components {

  enum VictorBleReportType {
    BLE_REPORT_TYPE_NONE      = 0,
    BLE_REPORT_TYPE_HEARTBEAT = 1,
    BLE_REPORT_TYPE_POWER     = 2,
    BLE_REPORT_TYPE_STATE     = 3,
  };

  struct VictorBleReport {
    String rawReport = "";
    VictorBleReportType type = BLE_REPORT_TYPE_NONE;
    String value = "";
  };

} // namespace Victor::Components

#endif // VictorBleModels_h
