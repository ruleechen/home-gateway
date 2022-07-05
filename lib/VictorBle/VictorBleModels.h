#ifndef VictorBleModels_h
#define VictorBleModels_h

#include <Arduino.h>

namespace Victor::Components {

  enum ServerNotifyType {
    SERVER_NOTIFY_NONE      = 0,
    SERVER_NOTIFY_HEARTBEAT = 1,
    SERVER_NOTIFY_POWER     = 2,
    SERVER_NOTIFY_ON        = 3,
  };

  struct ServerNotification {
    ServerNotifyType type;
    String args;
    ServerNotification* next;
    String toStr() const {
      auto typeName = String("NA");
      if (type == SERVER_NOTIFY_HEARTBEAT) {
        typeName = "HB";
      } else if (type == SERVER_NOTIFY_POWER) {
        typeName = "PW";
      } else if (type == SERVER_NOTIFY_ON) {
        typeName = "ON";
      }
      return typeName + ":" + args;
    }
  };

} // namespace Victor::Components

#endif // VictorBleModels_h
