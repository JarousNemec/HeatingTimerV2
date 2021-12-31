#include "../lib/LiquidCrystal_I2C-1.1.2/LiquidCrystal_I2C.h"
#include "../lib/RTClib-1.2.2/RTClib.h"
#include "../lib/DHT_sensor_library-1.0.0/DHT.h"
#include <Arduino.h>
#include <EEPROM.h>

#define pinDHT 2
#define typDHT11 DHT11

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

//Fields
const int defaultCursorPosition = 4;
int cursorBTN = defaultCursorPosition;

bool heating = true;
bool backlighting = true;
bool mainInfo = true;
bool Settings = false;
bool setTime = false;
bool setDate = false;
bool setHeatingTime = false;


float tep = 0;
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
//Time BeginHeatingTime = {18, 1, 1};
//Duration heatingDuration = {1, 1, 1};
Configuration configuration;
Time EndHeatingTime = {6, 1, 1};
Time BackLighting = {0, 0, 0};

int Year = 2005;
int Month = 10;
int Day = 26;
int Hour = 1;
int Minute = 1;
int Second = 1;

enum eepromAdress {
    cfgAddress = 0, htDuration = sizeof(Time),
};

enum pins {
    heatingLed = 7,
    heatingButton = 5,
    rele = 8,
    increaseButton = 4,
    decreaseButton = 3,
    tempSensor = 2,
    downArrowButton = 6,
    enterButton = 10,
    upArrowButton = 9,
    backLightsButton = 12
};

DateTime datumCas;
LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(pinDHT, typDHT11);
RTC_DS1307 DS1307;

//MethodsDeclarations

void checkHeatingTime();

void getMainInfo();

void heatingControl();

void settingHeatingTime();

void settingDate();

void settingTime();

void settingsPage();

void mainInfoPageOne();

void eventHandler();

void EnableOrDisableTextLight();

void setEndingHeatingTime();

void heatingStatus();

void printTemperature();

void printTimeAndDate();

void increaseValue();

void decreaseValue();

void switchBackLight();

void downArrow();

void upArrow();

void switchHeatingStatus();

void enter();

void drawMenu(String row1, String row2, String row3, String value1, String value2, String value3);

Configuration loadConfiguration();

Configuration validateConfiguration(Configuration cfg);

void saveConfiguration(int adress, Configuration value);

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
    lcd.print("load data");
    configuration = loadConfiguration();
    lcd.print("loaded");
    setEndingHeatingTime();
    lcd.print("setting");
    lcd.clear();
}

void loop() {
    eventHandler();
    if (mainInfo) {
        mainInfoPageOne();
    } else if (Settings) {
        settingsPage();
        // drawMenu("Set time","Set date","Set heating","","","");
    } else if (setTime) {
        settingTime();
        //drawMenu("Hour  : ","Minute: ","Second: ",Hour,Minute,Second);
    } else if (setDate) {
        settingDate();
        //drawMenu("Year  : ","Month : ","Day   : ",Year,Month,Day);
    } else if (setHeatingTime) {
        settingHeatingTime();
        //drawMenu("Set hour: ","Set minute: ","Set Interval: ",BeginHeatingTime[0],BeginHeatingTime[1],BeginHeatingTime[2]);
    }
    getMainInfo();
    checkHeatingTime();
    EnableOrDisableTextLight();
    heatingControl();
    //delay(300);
}

Configuration loadConfiguration() {
    Configuration configuration;
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

Time loadTime() {
    Time time;
    EEPROM.get(eepromAdress::cfgAddress, time);
    if (time.hour > 23) {
        time.hour = 6;
    }
    if (time.minute > 59) {
        time.minute = 1;
    }
    if (time.second > 59) {
        time.second = 1;
    }
    return time;
}

void checkHeatingTime() {
    if (!heating && datumCas.hour() == configuration.BeginHeatingTime.hour &&
        datumCas.minute() == configuration.BeginHeatingTime.minute) {
        heating = true;
    } else if (heating && datumCas.hour() == EndHeatingTime.hour && datumCas.minute() == EndHeatingTime.minute) {
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
            Settings = false;
        }
        if (cursorBTN == 1) {
            setDate = true;
            Settings = false;
        }
        if (cursorBTN == 2) {
            setHeatingTime = true;
            Settings = false;
        }
    }
    //vrati vse na hlavni stranku
    if (cursorBTN == 3) {
        if (setTime || setDate) {
            DS1307.adjust(DateTime(Year, Month, Day, Hour, Minute, Second));
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
        int number = ((configuration.BeginHeatingTime.hour + configuration.heatingDuration.hour) - 24);
        EndHeatingTime.hour = number;
    }
}

void mainInfoPageOne() {
    printTimeAndDate();
    printTemperature();
    heatingStatus();
}

void getMainInfo() {
    datumCas = DS1307.now();
    tep = dht.readTemperature();
}

void EnableOrDisableTextLight() {
    if ((BackLighting.minute + 10) == datumCas.minute()) {
        lcd.noBacklight();
    }
}

void TimeOfBeginLighting() {
    BackLighting.hour = datumCas.hour();
    BackLighting.minute = datumCas.minute();
    BackLighting.second = datumCas.second();
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
        if (cursorBTN == 0 && Hour > 0) {
            Hour = Hour - 1;
        } else if (cursorBTN == 1 && Minute > 0) {
            Minute = Minute - 1;
        } else if (cursorBTN == 2 && Second > 0) {
            Second = Second - 1;
        }
    } else if (setDate) {
        if (cursorBTN == 0 && Year > 2005) {
            Year = Year - 1;
        } else if (cursorBTN == 1 && Month > 1) {
            Month = Month - 1;
        } else if (cursorBTN == 2 && Day > 1) {
            Day = Day - 1;
        }
    } else if (setHeatingTime) {
        if (cursorBTN == 0 && configuration.BeginHeatingTime.hour > 0) {
            configuration.BeginHeatingTime.hour = configuration.BeginHeatingTime.hour - 1;
        } else if (cursorBTN == 1 && configuration.BeginHeatingTime.minute > 0) {
            configuration.BeginHeatingTime.minute = configuration.BeginHeatingTime.minute - 1;
        } else if (cursorBTN == 2 && configuration.heatingDuration.hour > 0) {
            configuration.heatingDuration.hour = configuration.heatingDuration.hour - 1;
        }
    }
}

void increaseValue() {
    if (setTime) {
        if (cursorBTN == 0 && Hour < 23) {
            Hour = Hour + 1;
        } else if (cursorBTN == 1 && Minute < 59) {
            Minute = Minute + 1;
        } else if (cursorBTN == 2 && Second < 59) {
            Second = Second + 1;
        }
    }
    if (setDate) {
        if (cursorBTN == 0 && Year < 3000) {
            Year = Year + 1;
        } else if (cursorBTN == 1 && Month < 12) {
            Month = Month + 1;
        } else if (cursorBTN == 2 && Day < 31) {
            Day = Day + 1;
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

//GraphicsRender

void settingsPage() {
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

void settingTime() {
    lcd.setCursor(0, 0);
    lcd.print("Hour  : ");
    lcd.print(Hour);
    lcd.setCursor(0, 1);
    lcd.print("Minute: ");
    lcd.print(Minute);
    lcd.setCursor(0, 2);
    lcd.print("Second: ");
    lcd.print(Second);
    lcd.setCursor(0, 3);
    lcd.print("Back");

    lcd.createChar(0, leftArrow);
    lcd.home();
    lcd.setCursor(14, cursorBTN);
    lcd.write(0);
}

void settingDate() {
    lcd.setCursor(0, 0);
    lcd.print("Year  : ");
    lcd.print(Year);
    lcd.setCursor(0, 1);
    lcd.print("Month : ");
    lcd.print(Month);
    lcd.setCursor(0, 2);
    lcd.print("Day   : ");
    lcd.print(Day);
    lcd.setCursor(0, 3);
    lcd.print("Back");

    lcd.createChar(0, leftArrow);
    lcd.home();
    lcd.setCursor(14, cursorBTN);
    lcd.write(0);
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

void printTimeAndDate() {
    //time and date
    lcd.setCursor(0, 0);
    lcd.print("Time ");
    lcd.print(datumCas.hour());
    lcd.print(":");
    lcd.print(datumCas.minute());
    lcd.print(":");
    lcd.print(datumCas.second());
    lcd.print("   ");
    lcd.setCursor(0, 1);
    lcd.print("Date ");
    lcd.print(datumCas.day());
    lcd.print(":");
    lcd.print(datumCas.month());
    lcd.print(":");
    lcd.print(datumCas.year());
    lcd.print("   ");
}

void printTemperature() {
    //temperature
    if (isnan(tep)) {
        lcd.setCursor(0, 2);
        lcd.print("Teplota: ");
        lcd.print("--.--C");
    } else {
        lcd.setCursor(0, 2);
        lcd.print("Teplota: ");
        lcd.print(tep);
        lcd.print("C");
    }
}

void heatingStatus() {
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

void settingHeatingTime() {
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

void drawMenu(String row1, String row2, String row3, String value1, String value2, String value3) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(row1);
    lcd.print(value1);
    lcd.setCursor(0, 1);
    lcd.print(row2);
    lcd.print(value2);
    lcd.setCursor(0, 2);
    lcd.print(row3);
    lcd.print(value3);
    lcd.setCursor(0, 3);
    lcd.print("Back");

    lcd.createChar(0, leftArrow);
    lcd.home();
    lcd.setCursor(14, cursorBTN);
    lcd.write(0);
}