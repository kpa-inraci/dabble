#include "DabbleGamepad.h"




DabbleGamepad::DabbleGamepad() : joystickX(0), joystickY(0), buttonState(0) {
    memset(incomingData, 0, sizeof(incomingData));
}

void DabbleGamepad::begin(long baudRate) 
{
    Serial1.begin(baudRate);
}

void DabbleGamepad::update() {
    // Réinitialiser les variables avant de lire de nouvelles données
    buttonState = 0;
    crossState=0;
    joystickX=0;
    joystickY=0;
    
    
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

void envoyerDabble(const char* format, ...) //fonction qui permet de reproduire le printf
{
   char buffer[128];  // Taille max du message (à ajuster si besoin)

  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);  // Formatage dans le buffer
  va_end(args);

  int longueur = strlen(buffer);

  Serial1.write(0xFF);           // Début de trame
  Serial1.write(0x0E);           // Module Terminal
  Serial1.write(0x01);           // Fonction "print"
  Serial1.write(0x01);           // 1 argument
  Serial1.write(longueur);       // Longueur du message
  Serial1.print(buffer);         // Message formaté
  Serial1.write((uint8_t)0x00);  // Fin de trame
}

bool verifierCommandeDabble(const char* commandeAttendue) 
{
  static enum {
    ATTENTE,
    MODULE_ID,
    FUNCTION_ID,
    ARG_COUNT,
    ARG_LENGTH,
    LECTURE_DATA,
    FIN
  } etat = ATTENTE;

  static byte moduleID, functionID, argCount, argLen, index;
  static char message[64]; // buffer du message reçu

  while (Serial1.available()) {
    byte data = Serial1.read();

    switch (etat) {
      case ATTENTE:
        if (data == 0xFF) etat = MODULE_ID;
        break;

      case MODULE_ID:
        if (data == 0x0E) etat = FUNCTION_ID;
        else etat = ATTENTE;
        break;

      case FUNCTION_ID:
        if (data == 0x01) etat = ARG_COUNT;
        else etat = ATTENTE;
        break;

      case ARG_COUNT:
        if (data == 1) etat = ARG_LENGTH;
        else etat = ATTENTE;
        break;

      case ARG_LENGTH:
        if (data < sizeof(message)) {
          argLen = data;
          index = 0;
          etat = LECTURE_DATA;
        } else {
          etat = ATTENTE;
        }
        break;

      case LECTURE_DATA:
        message[index++] = (char)data;
        if (index >= argLen) {
          message[argLen] = '\0';
          etat = FIN;
        }
        break;

      case FIN:
        if (data == 0x00) {
          etat = ATTENTE;
          return (strcmp(message, commandeAttendue) == 0);
        }
        break;
    }
  }

  return false;
}



