//
// Created by Strejda Jára on 31.12.2021.
//

#ifndef TOPENIV2_MAIN_H
#define TOPENIV2_MAIN_H

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
//MethodsDeclarations

void checkHeatingTime();

void heatingControl();

void eventHandler();

void EnableOrDisableTextLight();

void setEndingHeatingTime();

void increaseValue();

void decreaseValue();

void switchBackLight();

void downArrow();

void upArrow();

void switchHeatingStatus();

void enter();

void updateData();

//void drawMenu(String row1, String row2, String row3, String value1, String value2, String value3);

#endif //TOPENIV2_MAIN_H
