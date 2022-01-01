#include "../lib/LiquidCrystal_I2C-1.1.2/LiquidCrystal_I2C.h"
#include "../lib/RTClib-1.2.2/RTClib.h"
#include "../lib/DHT_sensor_library-1.0.0/DHT.h"
#include "Config.h"
#include "main.h"
#include "display.h"
#include <Arduino.h>

#define pinDHT 2
#define typDHT11 DHT11

//Fields
const int defaultCursorPosition = 4;
int cursorBTN = defaultCursorPosition;

bool heating = false;
bool backlighting = true;
bool mainInfo = true;
bool Settings = false;
bool setTime = false;
bool setDate = false;
bool setHeatingTime = false;

Configuration configuration;
Time EndHeatingTime = {6, 1, 1};
Time BackLighting = {0, 0, 0};

SettingDateTime settingDateTime{2005, 10, 26, 1, 1, 1};

DateTime machineDateTime;
LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(pinDHT, typDHT11);



void setup() {
    dht.begin();

    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("Starting...");

    //Serial.begin(9600);

    pinMode(pins::heatingLed, OUTPUT);
    pinMode(pins::rele, OUTPUT);

    pinMode(pins::heatingButton, INPUT);
    pinMode(pins::upArrowButton, INPUT);
    pinMode(pins::enterButton, INPUT);
    pinMode(pins::downArrowButton, INPUT);
    pinMode(pins::backLightsButton, INPUT);
    pinMode(pins::increaseButton, INPUT);
    pinMode(pins::decreaseButton, INPUT);

    configuration = loadConfiguration();
    setEndingHeatingTime();

    lcd.clear();
}

void loop() {
    eventHandler();
    updateData();
    if (mainInfo) {
        mainInfoPageOne(lcd, machineDateTime, dht.readTemperature(), heating);
    } else if (Settings) {
        settingsPage(lcd, cursorBTN);
        // drawMenu("Set time","Set date","Set heating","","","");
    } else if (setTime) {
        settingTime(lcd, settingDateTime, cursorBTN);
        //drawMenu("Hour  : ","Minute: ","Second: ",Hour,Minute,Second);
    } else if (setDate) {
        settingDate(lcd, settingDateTime, cursorBTN);
        //drawMenu("Year  : ","Month : ","Day   : ",Year,Month,Day);
    } else if (setHeatingTime) {
        settingHeatingTime(lcd, configuration, cursorBTN);
        //drawMenu("Set hour: ","Set minute: ","Set Interval: ",BeginHeatingTime[0],BeginHeatingTime[1],BeginHeatingTime[2]);
    }

    checkHeatingTime();
    DisableBackLight();
    heatingControl();
    delay(300);
}

void updateData() {
    machineDateTime = RTC_DS1307::now();
}

void checkHeatingTime() {
    if (!heating && machineDateTime.hour() == configuration.BeginHeatingTime.hour &&
        machineDateTime.minute() == configuration.BeginHeatingTime.minute) {
        heating = true;
    } else if (heating && machineDateTime.hour() == EndHeatingTime.hour &&
               machineDateTime.minute() == EndHeatingTime.minute) {
        heating = false;
    }
}

void enter() {
    //vstoupi do nastaveni
    if (mainInfo) {
        mainInfo = false;
        Settings = true;
    }
        //zvoli co se ma stat
    else if (Settings) {
        if (cursorBTN == 0) {
            setTime = true;
            settingDateTime.Hour = machineDateTime.hour();
            settingDateTime.Minute = machineDateTime.minute();
            settingDateTime.Second = machineDateTime.second();
        }
        if (cursorBTN == 1) {
            setDate = true;
            settingDateTime.Year = machineDateTime.year();
            settingDateTime.Month = machineDateTime.month();
            settingDateTime.Day = machineDateTime.day();
        }
        if (cursorBTN == 2) {
            setHeatingTime = true;
        }
        if (cursorBTN != defaultCursorPosition) {
            Settings = false;
        }
    }
    //vrati vse na hlavni stranku
    if (cursorBTN == 3) {
        if (setDate) {
            RTC_DS1307::adjust(
                    DateTime(settingDateTime.Year, settingDateTime.Month, settingDateTime.Day, machineDateTime.hour(),
                             machineDateTime.minute(), machineDateTime.second()));
        }
        if (setTime) {
            RTC_DS1307::adjust(
                    DateTime(machineDateTime.year(), machineDateTime.month(), machineDateTime.day(), settingDateTime.Hour,
                             settingDateTime.Minute, settingDateTime.Second));
        }
        if (setHeatingTime) {
            saveConfiguration(eepromAdress::cfgAddress, configuration);
        }
        mainInfo = true;
        Settings = false;
        setTime = false;
        setDate = false;
        setHeatingTime = false;
        cursorBTN = defaultCursorPosition;
        lcd.home();
        lcd.clear();
        setEndingHeatingTime();
    }
}

void setEndingHeatingTime() {
    if ((configuration.BeginHeatingTime.hour + configuration.heatingDuration.hour) < 24) {
        EndHeatingTime.hour = configuration.BeginHeatingTime.hour + configuration.heatingDuration.hour;
    } else if ((configuration.BeginHeatingTime.hour + configuration.heatingDuration.hour) > 24) {
        int intervalEnd = ((configuration.BeginHeatingTime.hour + configuration.heatingDuration.hour) - 24);
        EndHeatingTime.hour = intervalEnd;
    }
}


void DisableBackLight() {
    if ((BackLighting.minute + 10) == machineDateTime.minute()) {
        lcd.noBacklight();
    }
}

void TimeOfBeginLighting() {
    BackLighting.hour = machineDateTime.hour();
    BackLighting.minute = machineDateTime.minute();
    BackLighting.second = machineDateTime.second();
}

void eventHandler() {
    if (digitalRead(pins::heatingButton) == HIGH) {
        //topeni
        lcd.clear();
        switchHeatingStatus();
    }
    if (digitalRead(pins::upArrowButton) == HIGH) {
        //sipka nahoru
        lcd.clear();
        upArrow();
    }
    if (digitalRead(pins::enterButton) == HIGH) {
        //potvrzeni
        lcd.clear();
        enter();
    }
    if (digitalRead(pins::downArrowButton) == HIGH) {
        //sipka dolu
        lcd.clear();
        downArrow();
    }
    if (digitalRead(pins::backLightsButton) == HIGH) {
        //podsviceni
        lcd.clear();
        switchBackLight();
    }
    if (digitalRead(pins::increaseButton) == HIGH) {
        //increase button
        lcd.clear();
        increaseValue();
    }
    if (digitalRead(pins::decreaseButton) == HIGH) {
        //decrease button
        lcd.clear();
        decreaseValue();
    }
}

void switchHeatingStatus() {
    if (heating) {
        heating = false;
    } else {
        heating = true;
    }
}

void upArrow() {
    if (!mainInfo && cursorBTN > 0) {
        cursorBTN = cursorBTN - 1;
    }
}

void downArrow() {
    if (!mainInfo && cursorBTN < 3) {
        cursorBTN = cursorBTN + 1;
    }
}

void decreaseValue() {
    if (setTime) {
        if (cursorBTN == 0 && settingDateTime.Hour > 1) {
            settingDateTime.Hour = settingDateTime.Hour - 1;
        } else if (cursorBTN == 1 && settingDateTime.Minute > 1) {
            settingDateTime.Minute = settingDateTime.Minute - 1;
        } else if (cursorBTN == 2 && settingDateTime.Second > 1) {
            settingDateTime.Second = settingDateTime.Second - 1;
        }
    } else if (setDate) {
        if (cursorBTN == 0 && settingDateTime.Year > 2005) {
            settingDateTime.Year = settingDateTime.Year - 1;
        } else if (cursorBTN == 1 && settingDateTime.Month > 1) {
            settingDateTime.Month = settingDateTime.Month - 1;
        } else if (cursorBTN == 2 && settingDateTime.Day > 1) {
            settingDateTime.Day = settingDateTime.Day - 1;
        }
    } else if (setHeatingTime) {
        if (cursorBTN == 0 && configuration.BeginHeatingTime.hour > 1) {
            configuration.BeginHeatingTime.hour = configuration.BeginHeatingTime.hour - 1;
        } else if (cursorBTN == 1 && configuration.BeginHeatingTime.minute > 1) {
            configuration.BeginHeatingTime.minute = configuration.BeginHeatingTime.minute - 1;
        } else if (cursorBTN == 2 && configuration.heatingDuration.hour > 1) {
            configuration.heatingDuration.hour = configuration.heatingDuration.hour - 1;
        }
    }
}

void increaseValue() {
    if (setTime) {
        if (cursorBTN == 0 && settingDateTime.Hour < 23) {
            settingDateTime.Hour = settingDateTime.Hour + 1;
        } else if (cursorBTN == 1 && settingDateTime.Minute < 59) {
            settingDateTime.Minute = settingDateTime.Minute + 1;
        } else if (cursorBTN == 2 && settingDateTime.Second < 59) {
            settingDateTime.Second = settingDateTime.Second + 1;
        }
    }
    if (setDate) {
        if (cursorBTN == 0 && settingDateTime.Year < 3000) {
            settingDateTime.Year = settingDateTime.Year + 1;
        } else if (cursorBTN == 1 && settingDateTime.Month < 12) {
            settingDateTime.Month = settingDateTime.Month + 1;
        } else if (cursorBTN == 2 && settingDateTime.Day < 31) {
            settingDateTime.Day = settingDateTime.Day + 1;
        }
    }
    if (setHeatingTime) {
        if (cursorBTN == 0 && configuration.BeginHeatingTime.hour < 23) {
            configuration.BeginHeatingTime.hour = configuration.BeginHeatingTime.hour + 1;
        } else if (cursorBTN == 1 && configuration.BeginHeatingTime.minute < 59) {
            configuration.BeginHeatingTime.minute = configuration.BeginHeatingTime.minute + 1;
        } else if (cursorBTN == 2 && configuration.heatingDuration.hour < 59) {
            configuration.heatingDuration.hour = configuration.heatingDuration.hour + 1;
        }
    }
}

void heatingControl() {
    if (heating) {
        digitalWrite(8, HIGH);
        digitalWrite(7, HIGH);
    } else {
        digitalWrite(8, LOW);
        digitalWrite(7, HIGH);
    }
}

void switchBackLight() {
    if (backlighting) {
        lcd.noBacklight();
        backlighting = false;
        delay(10);
    } else if (!backlighting) {
        lcd.backlight();
        backlighting = true;
        TimeOfBeginLighting();
        delay(10);
    }
}