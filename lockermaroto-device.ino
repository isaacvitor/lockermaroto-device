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

//PATHS
const String LCK_STATE_PATH = "/config/lckState.txt";
const String LCK_USERS_UID_BASE_PATH = "/usersUID/";
const String LCK_USERS_ID_BASE_PATH = "/usersID/";
const String LCK_ITENS_UID_BASE_PATH = "/itensUID/";



//Servo (Lock - lck) - dependencies and initializations
#include <Servo.h>
#include "fsHelpers.h"

Servo lock;
const int LOCK_PIN = 4; // D2
const int LOCK_POSITION = 90;  //Posição que representa o fechamento da tranca
const int UNLOCK_POSITION = 0; //Posição que representa a abertura da tranca

String lckState;
const String CODE_STATE_LOCKED = "111";
const String CODE_STATE_UNLOKED = "101";
const String CODE_STATE_OPEN = "000";

//RFID (Locker Input Interface-lckInputInterface) - dependencies and initializations -
#include <SPI.h>
#include <MFRC522.h>
const int RST_PIN = 0;
const int SS_PIN = 16;
MFRC522 lckInputInterface(SS_PIN, RST_PIN); //Criando uma estância da MFRC522
#include "lckInputInterfaceHelpers.h"

//ReedSwitch (Locker State Detector - lckStateDetector ) - dependencies and initializations
const int LOCKER_STATE_DETECTOR_PIN = 5; // D1
int lckStateDetector = 0;



void setup(){
  Serial.begin(115200);
  
  //lckInputInterface Init
  SPI.begin();
  lckInputInterface.PCD_Init();

  //lock
  openFileSystem();
  lock.attach(LOCK_PIN);

  //HARDCODE - REMOVE AFTER
  saveUserByUID(3496392741);
  lckUnlock();
  delay(500);
}

void loop(){
  unsigned long uid;
  if(lckInputInterface.PICC_IsNewCardPresent()) {
    uid = lckGetUID();
    if(uid != -1){
      //DO AN ACTION:
      //TODO - how does it will work?
      if(getUserByUIDSaved(uid) != ""){
        lckLock();
        delay(1000);
        lckUnlock();
        getItemByUIDSaved(uid);
        sendUIDToGateway(uid);
      }
      
    }
  }
}

void lckLock(){
  lock.write(LOCK_POSITION);
}

void lckUnlock(){
  lock.write(UNLOCK_POSITION);
}
/*
void doActionByState(String state){
  int angle = lock.read();
  //TODO - colocar estado do sensor da porta
  if(state.equals(stateLock) && angle != CODE_STATE_LOCKED){
    Serial.println("Locking");
    lckLock();
  }else if(state.equals(stateUnlock) && angle != CODE_STATE_UNLOCK){
    Serial.println("Unlocking");
    lckUnlock();
  }
}*/

void saveUserByUID(unsigned long uid){
  String path = LCK_USERS_UID_BASE_PATH + uid;
  String uidStr = String( uid );
  writeParameterOnFile(path, uidStr);
  Serial.println("saveUserByUID - " + uidStr);
}
