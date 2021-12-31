//
// Created by Strejda Jára on 31.12.2021.
//

#include <Arduino.h>
#include "../lib/RTClib-1.2.2/RTClib.h"
#include "../lib/LiquidCrystal_I2C-1.1.2/LiquidCrystal_I2C.h"
#include "display.h"
#include "main.h"
#include "Config.h"

//GraphicsObjects
byte three[8] = {
        0b00000,
        0b10000,
        0b01000,
        0b00100,
        0b00010,
        0b00001,
        0b00000,
        0b00000
};
byte two[8] = {
        0b00000,
        0b00000,
        0b00000,
        0b11111,
        0b00000,
        0b00000,
        0b00000,
        0b00000
};
byte one[8] = {
        0b00000,
        0b00001,
        0b00010,
        0b00100,
        0b01000,
        0b10000,
        0b00000,
        0b00000
};
byte leftArrow[8] = {
        0b00000,
        0b00000,
        0b01000,
        0b11111,
        0b01000,
        0b00000,
        0b00000,
        0b00000
};

void settingsPage(LiquidCrystal_I2C lcd, int cursorBTN) {
    lcd.setCursor(0, 0);
    lcd.print("Set time");
    lcd.setCursor(0, 1);
    lcd.print("Set date");
    lcd.setCursor(0, 2);
    lcd.print("Set heating");
    lcd.setCursor(0, 3);
    lcd.print("Back");

    lcd.createChar(0, leftArrow);
    lcd.home();
    lcd.setCursor(14, cursorBTN);
    lcd.write(0);

}

void settingTime(LiquidCrystal_I2C lcd, SettingDateTime settingDateTime, int cursorBTN) {
    lcd.setCursor(0, 0);
    lcd.print("Hour  : ");
    lcd.print(settingDateTime.Hour);
    lcd.setCursor(0, 1);
    lcd.print("Minute: ");
    lcd.print(settingDateTime.Minute);
    lcd.setCursor(0, 2);
    lcd.print("Second: ");
    lcd.print(settingDateTime.Second);
    lcd.setCursor(0, 3);
    lcd.print("Back");

    lcd.createChar(0, leftArrow);
    lcd.home();
    lcd.setCursor(14, cursorBTN);
    lcd.write(0);
}

void settingDate(LiquidCrystal_I2C lcd, SettingDateTime settingDateTime, int cursorBTN) {
    lcd.setCursor(0, 0);
    lcd.print("Year  : ");
    lcd.print(settingDateTime.Year);
    lcd.setCursor(0, 1);
    lcd.print("Month : ");
    lcd.print(settingDateTime.Month);
    lcd.setCursor(0, 2);
    lcd.print("Day   : ");
    lcd.print(settingDateTime.Day);
    lcd.setCursor(0, 3);
    lcd.print("Back");

    lcd.createChar(0, leftArrow);
    lcd.home();
    lcd.setCursor(14, cursorBTN);
    lcd.write(0);
}

void settingHeatingTime(LiquidCrystal_I2C lcd, Configuration configuration, int cursorBTN) {
    lcd.setCursor(0, 0);
    lcd.print("Set hour: ");
    lcd.print(configuration.BeginHeatingTime.hour);
    lcd.setCursor(0, 1);
    lcd.print("Set minute: ");
    lcd.print(configuration.BeginHeatingTime.minute);
    lcd.setCursor(0, 2);
    lcd.print("Set Interval: ");
    lcd.print(configuration.heatingDuration.hour);
    lcd.setCursor(0, 3);
    lcd.print("Back");

    lcd.createChar(0, leftArrow);
    lcd.home();
    lcd.setCursor(17, cursorBTN);
    lcd.write(0);
}

void printTimeAndDate(LiquidCrystal_I2C lcd, DateTime machineDateTime) {
    //time and date
    lcd.setCursor(0, 0);
    lcd.print("Time ");
    lcd.print(machineDateTime.hour());
    lcd.print(":");
    lcd.print(machineDateTime.minute());
    lcd.print(":");
    lcd.print(machineDateTime.second());
    lcd.print("   ");
    lcd.setCursor(0, 1);
    lcd.print("Date ");
    lcd.print(machineDateTime.day());
    lcd.print(":");
    lcd.print(machineDateTime.month());
    lcd.print(":");
    lcd.print(machineDateTime.year());
    lcd.print("   ");
}

void printTemperature(LiquidCrystal_I2C lcd, float temp) {
    //temperature
    if (isnan(temp)) {
        lcd.setCursor(0, 2);
        lcd.print("Teplota: ");
        lcd.print("--.--C");
    } else {
        lcd.setCursor(0, 2);
        lcd.print("Teplota: ");
        lcd.print(temp);
        lcd.print("C");
    }
}

void heatingStatus(LiquidCrystal_I2C lcd, bool heating) {
    //heating status
    lcd.setCursor(0, 3);
    lcd.print("Status: ");
    if (heating) {
        lcd.print("heating ");
        lcd.createChar(0, one);
        lcd.home();
        lcd.setCursor(17, 3);
        lcd.write(0);
        delay(200);
        lcd.createChar(0, two);
        lcd.home();
        lcd.setCursor(17, 3);
        lcd.write(0);
        delay(200);
        lcd.createChar(0, three);
        lcd.home();
        lcd.setCursor(17, 3);
        lcd.write(0);
    } else {
        lcd.setCursor(8, 3);
        lcd.print("cooling ");
    }
}

void mainInfoPageOne(LiquidCrystal_I2C lcd, DateTime machineDateTime, float temp, bool heating) {
    printTimeAndDate(lcd, machineDateTime);
    printTemperature(lcd, temp);
    heatingStatus(lcd, heating);
}