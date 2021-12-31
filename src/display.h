//
// Created by Strejda Jára on 31.12.2021.
//

#ifndef TOPENIV2_DISPLAY_H
#define TOPENIV2_DISPLAY_H

#include "Config.h"



void settingsPage(LiquidCrystal_I2C lcd, int cursorBTN);

void settingTime(LiquidCrystal_I2C lcd, SettingDateTime settingDateTime, int cursorBTN);

void settingDate(LiquidCrystal_I2C lcd, SettingDateTime settingDateTime, int cursorBTN);

void settingHeatingTime(LiquidCrystal_I2C lcd, Configuration configuration, int cursorBTN);

void printTimeAndDate(LiquidCrystal_I2C lcd, DateTime machineDateTime);

void printTemperature(LiquidCrystal_I2C lcd, float temp);

void heatingStatus(LiquidCrystal_I2C lcd, bool heating);

void mainInfoPageOne(LiquidCrystal_I2C lcd, DateTime machineDateTime, float temp, bool heating);

#endif //TOPENIV2_DISPLAY_H
