#ifndef VictorBleModels_h
#define VictorBleModels_h

#include <Arduino.h>

namespace Victor::Components {

  enum ServerNotifyType {
    SERVER_NOTIFY_NONE      = 0,
    SERVER_NOTIFY_READY     = 1,
    SERVER_NOTIFY_BATTERY   = 2,
    SERVER_NOTIFY_ON        = 3,
  };

  enum ServerCommandType {
    SERVER_COMMAND_NONE          = 0,
    SERVER_COMMAND_RESET         = 1,
    SERVER_COMMAND_AUTHENTICATE  = 2,
    SERVER_COMMAND_QUERY_BATTERY = 3,
    SERVER_COMMAND_SET_OTA       = 4,
    SERVER_COMMAND_SET_ALARM     = 5,
    SERVER_COMMAND_QUERY_ON      = 6,
  };

  struct ServerNotification {
    ServerNotifyType type;
    String args;
    String raw;
    ServerNotification* next;
  };

  struct ServerCommand {
    ServerCommandType type;
    String args;
    String serialize() const {
      auto typeName = String("N/A");
      if (type == SERVER_COMMAND_RESET) {
        typeName = "RST";
      } else if (type == SERVER_COMMAND_AUTHENTICATE) {
        typeName = "AUTH";
      } else if (type == SERVER_COMMAND_QUERY_ON) {
        typeName = "ON";
      } else if (type == SERVER_COMMAND_SET_ALARM) {
        typeName = "AM";
      } else if (type == SERVER_COMMAND_QUERY_BATTERY) {
        typeName = "BTY";
      } else if (type == SERVER_COMMAND_SET_OTA) {
        typeName = "OTA";
      }
      return typeName + ":" + (args.length() ? args : "?");
    }
  };

} // namespace Victor::Components

#endif // VictorBleModels_h
