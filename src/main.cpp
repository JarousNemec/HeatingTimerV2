#include "../lib/LiquidCrystal_I2C-1.1.2/LiquidCrystal_I2C.h"
#include "../lib/DHT_sensor_library-1.4.3/DHT.h"
#include "../lib/RTClib-2.0.0/src/RTClib.h"
#include <Arduino.h>

#define pinDHT 2
#define typDHT11 DHT11

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT mojeDHT(pinDHT, typDHT11);
RTC_DS1307 DS1307;

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
byte backArrow[8] = {
        0b00000,
        0b00000,
        0b11100,
        0b00100,
        0b10101,
        0b01110,
        0b00100,
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

int cursorBTN = 0;
int BTN = 0;
int page = 0;

bool heating = true;
bool backlighting = true;
bool setting = false;
bool mainInfo = true;
bool Settings = false;
bool setTime = false;
bool setDate = false;
bool setHeatingTime = false;

float tep = 0;

int BeginHeatingTime[3] = {18, 1, 0};
int EndHeatingTime[3] = {6, 1, 0};
int BackLighting[3] = {0, 0, 0};

int Year = 2005;
int Month = 10;
int Day = 26;
int Hour = 1;
int Minute = 1;
int Second = 1;
DateTime datumCas;

void HeatingEnableOrDisable();

void setMainInfo();

void heatingControl();

void settingHeatingTime();

void settingDate();

void settingTime();

void settingsPage();

void mainInfoPageOne();

void readKeyboardAndComplateSimpleOperations();

void EnableOrDisableTextLight();

void EndingHeatingTime();

void heatingStatus();

void printTemperature();

void printTimeAndDate();

void setup() {

    mojeDHT.begin();
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Starting...");
    Serial.begin(9600);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    delay(1000);

}

void loop() {
    setMainInfo();
    HeatingEnableOrDisable();
    EnableOrDisableTextLight();
    if (BTN >= 10 || digitalRead(3) == HIGH || digitalRead(4) == HIGH || digitalRead(5) == HIGH) {
        readKeyboardAndComplateSimpleOperations();
    }
    if (mainInfo == true) {
        mainInfoPageOne();
    } else if (Settings == true) {
        settingsPage();
    } else if (setTime == true) {
        settingTime();
    } else if (setDate == true) {
        settingDate();
    } else if (setHeatingTime == true) {
        settingHeatingTime();
    }
    heatingControl();
    delay(5);
    HeatingEnableOrDisable();
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

void HeatingEnableOrDisable() {
    if (heating == false && datumCas.hour() == BeginHeatingTime[0] && datumCas.minute() == BeginHeatingTime[1]) {
        heating = true;
    } else if (heating == true && datumCas.hour() == EndHeatingTime[0] && datumCas.minute() == EndHeatingTime[1]) {
        heating = false;
    }
}

void enterButton() {
    //vstoupi do nastaveni
    if (mainInfo == true) {
        mainInfo = false;
        Settings = true;
    }
        //zvoli co se ma stat
    else if (Settings == true) {
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
        if (setTime == true || setDate == true) {
            DS1307.adjust(DateTime(Year, Month, Day, Hour, Minute, Second));
        }
        mainInfo = true;
        Settings = false;
        setTime = false;
        setDate = false;
        setHeatingTime = false;
        cursorBTN = 0;
        lcd.home();
        lcd.clear();
        EndingHeatingTime();
    }

}

void EndingHeatingTime() {
    if ((BeginHeatingTime[0] + 6) < 24) {
        EndHeatingTime[0] = BeginHeatingTime[0] + 6;
    } else if ((BeginHeatingTime[0] + 6) > 24) {
        int number = ((BeginHeatingTime[0] + 6) - 24);
        EndHeatingTime[0] = number;
    }
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

void settingHeatingTime() {
    lcd.setCursor(0, 0);
    lcd.print("Set hour: ");
    lcd.print(BeginHeatingTime[0]);
    lcd.setCursor(0, 1);
    lcd.print("Set minute: ");
    lcd.print(BeginHeatingTime[1]);
    lcd.setCursor(0, 2);
    lcd.print("Set second: ");
    lcd.print(BeginHeatingTime[2]);
    lcd.setCursor(0, 3);
    lcd.print("Back");

    lcd.createChar(0, leftArrow);
    lcd.home();
    lcd.setCursor(14, cursorBTN);
    lcd.write(0);
}

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

void mainInfoPageOne() {
    printTimeAndDate();
    printTemperature();
    heatingStatus();
}

void setMainInfo() {
    BTN = 0;
    BTN = analogRead(A0);
    datumCas = DS1307.now();
    tep = mojeDHT.readTemperature();
}

void EnableOrDisableTextLight() {
    if ((BackLighting[1] + 10) == datumCas.minute()) {
        lcd.noBacklight();
    }
}

void TimeOfBeginLighting() {
    BackLighting[0] = datumCas.hour();
    BackLighting[1] = datumCas.minute();
    BackLighting[2] = datumCas.second();
}

void readKeyboardAndComplateSimpleOperations() {
    if (digitalRead(5) == HIGH) {
        //topeni
        lcd.clear();
        if (heating == true) {
            heating = false;
        } else if (heating == false) {
            heating = true;
        }
    }

    if (digitalRead(9) == HIGH) {
        //sipka nahoru
        lcd.clear();
        if (mainInfo == false && cursorBTN > 0) {
            cursorBTN = cursorBTN - 1;
        }
    }
    if (digitalRead(10) == HIGH) {
        enterButton();
        lcd.clear();
    }
    if (digitalRead(6) == HIGH) {
        //sipka dolu
        lcd.clear();
        if (mainInfo == false && cursorBTN < 3) {
            cursorBTN = cursorBTN + 1;
        }
    }
    if (digitalRead(12) == HIGH) {
        //podsviceni
        lcd.clear();
        if (backlighting == true) {
            lcd.noBacklight();
            backlighting = false;
            delay(10);
        } else if (backlighting == false) {
            lcd.backlight();
            backlighting = true;
            TimeOfBeginLighting();
            delay(10);
        }
    }
    if (digitalRead(4) == HIGH) {
        if (setTime == true) {
            if (cursorBTN == 0 && Hour < 23) {
                Hour = Hour + 1;
            } else if (cursorBTN == 1 && Minute < 59) {
                Minute = Minute + 1;
            } else if (cursorBTN == 2 && Second < 59) {
                Second = Second + 1;
            }
        }
        if (setDate == true) {
            if (cursorBTN == 0 && Year < 3000) {
                Year = Year + 1;
            } else if (cursorBTN == 1 && Month < 12) {
                Month = Month + 1;
            } else if (cursorBTN == 2 && Day < 31) {
                Day = Day + 1;
            }
        }
        if (setHeatingTime == true) {
            if (cursorBTN == 0 && BeginHeatingTime[0] < 23) {
                BeginHeatingTime[0] = BeginHeatingTime[0] + 1;
            } else if (cursorBTN == 1 && BeginHeatingTime[1] < 59) {
                BeginHeatingTime[1] = BeginHeatingTime[1] + 1;
            } else if (cursorBTN == 2 && BeginHeatingTime[2] < 59) {
                BeginHeatingTime[2] = BeginHeatingTime[2] + 1;
            }
        }
    }
    if (digitalRead(3) == HIGH) {
        lcd.clear();
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
            if (cursorBTN == 0 && BeginHeatingTime[0] > 0) {
                BeginHeatingTime[0] = BeginHeatingTime[0] - 1;
            } else if (cursorBTN == 1 && BeginHeatingTime[1] > 0) {
                BeginHeatingTime[1] = BeginHeatingTime[1] - 1;
            } else if (cursorBTN == 2 && BeginHeatingTime[2] > 0) {
                BeginHeatingTime[2] = BeginHeatingTime[2] - 1;
            }
        }
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
    if (heating == true) {
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

void heatingControl() {
    if (heating) {
        digitalWrite(8, HIGH);

    } else {
        digitalWrite(8, LOW);
    }
}