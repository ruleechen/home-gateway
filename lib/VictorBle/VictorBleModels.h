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

  enum ServerCommandType {
    SERVER_COMMAND_NONE  = 0,
    SERVER_COMMAND_QUERY = 1,
    SERVER_COMMAND_ALARM = 2,
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

  struct ServerCommand {
    ServerCommandType type;
    String args;
    String toStr() const {
      auto typeName = String("NA");
      if (type == SERVER_COMMAND_QUERY) {
        typeName = "QY";
      } else if (type == SERVER_COMMAND_ALARM) {
        typeName = "AM";
      }
      return typeName + ":" + args;
    }
  };

} // namespace Victor::Components

#endif // VictorBleModels_h
