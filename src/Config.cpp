//
// Created by Strejda Jára on 31.12.2021.
//

#include <EEPROM.h>
#include "Config.h"

Configuration loadConfiguration() {
    Configuration configuration{};
    EEPROM.get(eepromAdress::cfgAddress, configuration);
    configuration = validateConfiguration(configuration);
    return configuration;
}

Configuration validateConfiguration(Configuration cfg) {
    Duration duration = cfg.heatingDuration;
    Time time = cfg.BeginHeatingTime;
    if (time.hour > 23) {
        time.hour = 6;
    }
    if (time.minute > 59) {
        time.minute = 1;
    }
    if (time.second > 59) {
        time.second = 1;
    }
    if (duration.hour > 23) {
        duration.hour = 6;
    }
    if (duration.minute > 59) {
        duration.minute = 1;
    }
    if (duration.second > 59) {
        duration.second = 1;
    }
    cfg.heatingDuration = duration;
    cfg.BeginHeatingTime = time;
    return cfg;
}

void saveConfiguration(int adress, Configuration value) {
    EEPROM.put(adress, value);
}