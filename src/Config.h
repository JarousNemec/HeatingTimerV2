//
// Created by Strejda Jára on 31.12.2021.
//

#ifndef TOPENIV2_CONFIG_H
#define TOPENIV2_CONFIG_H


#include <Arduino.h>

struct Time {
    byte hour;
    byte minute;
    byte second;
};
struct Duration {
    byte hour;
    byte minute;
    byte second;
};
struct Configuration {
    Time BeginHeatingTime;
    Duration heatingDuration;
};
enum eepromAdress {
    cfgAddress = 0
};

struct SettingDateTime {
    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
};

Configuration loadConfiguration();

Configuration validateConfiguration(Configuration cfg);

void saveConfiguration(int adress, Configuration value);
#endif //TOPENIV2_CONFIG_H
