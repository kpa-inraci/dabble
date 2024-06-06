#include "DabbleGamepad.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "YOUR_SERVICE_UUID"
#define CHARACTERISTIC_UUID "YOUR_CHARACTERISTIC_UUID"

DabbleGamepad::DabbleGamepad() : joystickX(0), joystickY(0), buttonState(0), pServer(nullptr), pService(nullptr), pCharacteristic(nullptr) {
    memset(incomingData, 0, sizeof(incomingData));
}

void DabbleGamepad::begin(long baudRate) 
{
    #ifdef ESP32
        // Initialisation BLE pour ESP32
        BLEDevice::init("DabbleGamepad");
        pServer = BLEDevice::createServer();
        pService = pServer->createService(SERVICE_UUID);
        pCharacteristic = pService->createCharacteristic(
                            CHARACTERISTIC_UUID,
                            BLECharacteristic::PROPERTY_READ |
                            BLECharacteristic::PROPERTY_WRITE
                          );
        pService->start();
        pServer->getAdvertising()->start();

        // Ajoutez des logs pour vérifier l'initialisation
        if (pServer == nullptr || pService == nullptr || pCharacteristic == nullptr) {
            Serial.println("Erreur d'initialisation BLE");
        }
    #else
        // Initialisation série pour autres cartes
        Serial1.begin(baudRate);
    #endif
}

void DabbleGamepad::update() {
    // Réinitialiser les variables avant de lire de nouvelles données
    buttonState = 0;
    crossState=0;
    joystickX=0;
    joystickY=0;

    #ifdef ESP32
        // Lecture des données via BLE pour ESP32
        if (pCharacteristic != nullptr && pCharacteristic->getValue().length() > 0) {
            memcpy(incomingData, pCharacteristic->getValue().c_str(), sizeof(incomingData));
            parseData();
        } else {
            Serial.println("pCharacteristic est nul ou valeur vide");
        }
    #else
        // Lecture des données série pour autres cartes
        while (Serial1.available()) {
            int len = Serial1.readBytes(incomingData, sizeof(incomingData));

            #ifdef PRINT_DEBUG
              // Print the raw data in hexadecimal format
              Serial.print("Raw data: ");
              for (int i = 0; i < len; i++) 
              {
                Serial.printf("0x%x ",incomingData[i]);
              }
              Serial.println();
            #endif

            parseData();  //traitement des données brut
        }
    #endif
}

uint16_t DabbleGamepad::isButtonPressed(uint16_t button) 
{ 
    if(buttonState==button)
        return buttonState;
    else
        return 0;    
}

int16_t DabbleGamepad::getJoystickX(void) 
{
    return (int16_t)(joystickX * 10);
}

int16_t DabbleGamepad::getJoystickY(void) 
{
    return (int16_t)(joystickY * 10);
}

uint16_t DabbleGamepad::getJoystickAngle(void) 
{
    return joystickAngle;
}

uint16_t DabbleGamepad::getJoystickRadius(void) 
{
    return joystickRadius;
}

void DabbleGamepad::parseData() {
    char buffIncomingData;
    // Vérifiez la séquence de début et de fin des données pour s'assurer qu'elles sont valides
    if (incomingData[0] == 0xFF && incomingData[8] == 0xFF) {

        buffIncomingData=incomingData[6] & 0x0F;

        // Filtre la croix directionnelle

        if (buffIncomingData != UP     && 
            buffIncomingData != DOWN   && 
            buffIncomingData != LEFT   && 
            buffIncomingData != RIGHT
            ) 
        {
            buffIncomingData = 0;
        }

        // Combine les états pour une vérification plus facile
        buttonState = (incomingData[5] << 4) | buffIncomingData;

        joystickAngle=((incomingData[14] >> 3)*15);
        joystickRadius=incomingData[14]&0x07;
        joystickX= float(joystickRadius*(float(cos(float(joystickAngle*PI/180)))));
        joystickY= float(joystickRadius*(float(sin(float(joystickAngle*PI/180)))));

        #ifdef PRINT_DEBUG
          Serial.printf("\n angle: %d\n",joystickAngle);   
          Serial.printf("\n radiu y %f\n",joystickRadius);           
          Serial.printf("\n x_value x %f\n",joystickX);
          Serial.printf("\n y_value y %f\n",joystickY);

          if(buttonState)   Serial.printf("buttonState -> 0x%x\n",buttonState);  
        #endif
    }
}
