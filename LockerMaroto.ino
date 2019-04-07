/*

    LockerMaroto - (lm)
    
    O LockerMaroto é uma PoC(Proof of Concept) com objetivo de validar e estudar um sistema de controle de acesso a armários.
    A ideia básica é que dado um inventário de equipamentos X, devidamente catalogado, armazenado em compartimentos devidamente indentificados, 
    possam ser acessados de uma maneira inteligente e/ou menos burocrática do que o uso de uma chave convencional.

    Builded on Nodemcu Lolin v1.0

    Isaac Vitor - 2019

    STATES:

    O locker deverá manter seu estado nele próprio e uma cópia no servidor, os estados possível são:
    Trancado = (Porta fechada + trava ativada + lckStateDetector=True) => lckState=[1,1,1]
    Destrancado = (Porta fechada + trava desativada + lckStateDetector=True) => lckState=[1,0,1]
    Aberto = (Porta aberta + trava aberta + lckStateDetector=False) => lckState=[0,0,0]
    Desconhecido = Em tese apenas no servidor, caso o mesmo não tenha comunicação com o locker registrado.=> lckState=undefined

*/

//RFID (Locker Input Interface-lckInputInterface) - dependencies and initializations - 
  #include <SPI.h>
  #include <MFRC522.h>

  const int RST_PIN 0 // RST = GPIO 0
  const int SS_PIN 16 // D0
  MFRC522 mfrc522(SS_PIN, RST_PIN);  //Criando uma estância da MFRC522

//Servo (Lock - lck) - dependencies and initializations
  #include <Servo.h>
  #include "fsHelpers.h"
  
  Servo lock;
  const int LOCK_POSITION = 90; //Posição que representa o fechamento da tranca
  const int UNLOCK_POSITION = 0; //Posição que representa a abertura da tranca
  
  String lckState;
  const String CODE_STATE_LOCKED = "111";
  const String CODE_STATE_UNLOKED = "101";
  const String CODE_STATE_OPEN = "000";
  String lckStatePath = "/config/lckState.txt";

//ReedSwitch (Locker State Detector - lckStateDetector ) - dependencies and initializations
  const int LOCKER_STATE_DETECTOR_PIN = 5;
  int lckStateDetector = 0;



void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
