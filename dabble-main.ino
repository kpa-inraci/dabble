#include "DabbleGamepad.h"

unsigned int compteur=0;

DabbleGamepad gamepad;

void setup() 
{
    Serial.begin(9600);
    gamepad.begin(9600);
}

void loop() 
{
    gamepad.update();   //fonction qui raffraichie la lecture du bluetooth
   
    if (gamepad.isButtonPressed(UP))        { Serial.println("Up button is pressed");       }
    if (gamepad.isButtonPressed(DOWN))      { Serial.println("Down button is pressed");     }
    if (gamepad.isButtonPressed(LEFT))      { Serial.println("Left button is pressed");     }
    if (gamepad.isButtonPressed(RIGHT))     { Serial.println("Right button is pressed");    }
    if (gamepad.isButtonPressed(START))     { Serial.println("Start button is pressed");    }
    if (gamepad.isButtonPressed(SELECT))    { Serial.println("Select button is pressed");   }
    if (gamepad.isButtonPressed(TRIANGLE))  { Serial.println("Triangle button is pressed"); }
    if (gamepad.isButtonPressed(CIRCLE))    { Serial.println("Circle button is pressed");   }
    if (gamepad.isButtonPressed(CROSS))     { Serial.println("Cross button is pressed");    }
    if (gamepad.isButtonPressed(SQUARE))    { Serial.println("Square button is pressed");   }
  

  if(gamepad.getJoystickY()||gamepad.getJoystickX())
  {
    Serial.printf("X:%3d  ",gamepad.getJoystickX());
    Serial.printf("Y:%3d  ",gamepad.getJoystickY());
    Serial.printf("a:%3d°\n",gamepad.getJoystickAngle());
   // Serial.printf("r:%3d°\n",gamepad.getJoystickRadius());
  }    
  

 envoyerDabble("valeur du compteur %d",compteur);   //envoie de donnée vers le terminal dabble
 compteur+=1;

  delay(2000);
}


