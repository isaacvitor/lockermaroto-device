
/*

    LockerMaroto - (lm)
    
    O LockerMaroto é uma PoC(Proof of Concept) com objetivo de validar e estudar um sistema de controle de acesso a armários.
    A ideia básica é que dado um inventário de equipamentos X, devidamente catalogado, armazenado em compartimentos devidamente indentificados, 
    possam ser acessados de uma maneira inteligente e/ou menos burocrática do que o uso de uma chave convencional.

    Builded on Nodemcu Lolin v1.0

    Isaac Vitor - 2019

    STATES:

    O locker deverá manter seu estado nele próprio e uma cópia no servidor, os estados possível são:
    Trancado = (Porta fechada + trava ativada + lckStateDetector=True) => lckState="11"
    Destrancado = (Porta fechada + trava desativada ) => lckState="10"
    Aberto = (Porta aberta + trava aberta) => lckState="00"
    Desconhecido = Em tese apenas no servidor, caso o mesmo não tenha comunicação com o locker registrado.=> lckState=undefined

*/
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include "fsHelpers.h"
#include <Servo.h>

#include <SPI.h>
#include <MFRC522.h>
  
#include "index.h" //Home to device config
#include "saved.h" //Home to device config

//PATHS
  const String LCK_STATE_PATH = "/config/lckState.txt";
  const String LCK_USERS_UID_BASE_PATH = "/usersUID/";
  const String LCK_USERS_ID_BASE_PATH = "/usersID/";
  const String LCK_ITENS_UID_BASE_PATH = "/itensUID/";
  //Saved Parameters
  const String SAVED_SSID = "/config/ssid";
  const String SAVED_PASSWORD = "/config/password";
  const String SAVED_GATEWAY = "/config/gateway";
  const String SAVED_DEVICE_NAME = "/config/device";


//Operation Mode
  const int BT_CHANGE_MODE = A0;
  boolean accessPointMode = false;
  
  String mac = "APS-" + WiFi.softAPmacAddress();
  const char *ssidAP = mac.c_str();
  const char *passwordAP = "lockermaroto";
  String errorMessage;

  String readSSID;
  String readPassword;
  String readGateway;
  String readDeviceName;

  ESP8266WebServer server(80);
  WiFiClient espClient;
  PubSubClient client(espClient);
  #include "accessPointHelpers.h"
  

//Servo (Lock - lck) - dependencies and initializations
  Servo lock;
  boolean lckIsLocked = false;
  const int LOCK_PIN = 4; // D2
  const int LOCK_POSITION = 90;  //Posição que representa o fechamento da tranca
  const int UNLOCK_POSITION = 0; //Posição que representa a abertura da tranca
  
  String lckState;
  const String CODE_STATE_LOCKED = "11";
  const String CODE_STATE_UNLOKED = "10";
  const String CODE_STATE_OPEN = "00";

//RFID (Locker Input Interface-lckInputInterface) - dependencies and initializations -
  
  const int RST_PIN = 0; //D3?
  const int SS_PIN = 16; //D0
  MFRC522 lckInputInterface(SS_PIN, RST_PIN); //Criando uma estância da MFRC522
  #include "lckInputInterfaceHelpers.h"
  
//ReedSwitch (Locker State Detector - lckStateDetector ) - dependencies and initializations
  const int lckSwitchOpenDetectPin = 5; // D1
  boolean lckIsDoorClosed = 0;



void setup(){
  Serial.begin(115200);
  //Abrindo sistema de arquivos
  openFileSystem();
  
  //Definindo o botão de modo de operação
  pinMode(BT_CHANGE_MODE, INPUT);
 
  //Definindo o modo de operação ACCESSPOINT X CLIENT
  if(analogRead(BT_CHANGE_MODE) == HIGH){
    
    Serial.println("Client MODE");
    //lckStateDetector
    pinMode(lckSwitchOpenDetectPin, INPUT);
    
    //lckInputInterface Init
    SPI.begin();
    lckInputInterface.PCD_Init();
  
    //lock
    lock.attach(LOCK_PIN);

    //Connecting to WiFi
    setupWiFi();
  
    //HARDCODE - REMOVE AFTER
    /*saveUserByUID(3496392741);
    lckUnlock();
    delay(500);*/
    
  } else {
    Serial.println("AccessPooint MODE");
    startAccessPoint();
  }
  
  
}

void loop(){
  if(accessPointMode){
    server.handleClient();
  }else {
    unsigned long uid;
    if(lckInputInterface.PICC_IsNewCardPresent()) {
      uid = lckGetUID();
      if(uid != -1){
        //DO AN ACTION:
        //TODO - how does it will work?
        if(getUserByUIDSaved(uid) != ""){
          lckDoorClosed();
          if(lckIsDoorClosed && !lckIsLocked){
            lckLock();
          }else if(lckIsDoorClosed && lckIsLocked){
            lckUnlock();
          }
          
          delay(1000);
          
          //getItemByUIDSaved(uid);
          //sendUIDToGateway(uid);
        }
        
      }
    }
  }
}



//Lock
void lckLock(){
    lock.write(LOCK_POSITION);
    lckIsLocked = true;
}

void lckUnlock(){
  lock.write(UNLOCK_POSITION);
  lckIsLocked = false;
}

boolean lckDoorClosed(){
 int s = digitalRead(lckSwitchOpenDetectPin);
 if (s == HIGH) {
  lckIsDoorClosed = true;
  Serial.println("Closed Door");
 } else {
  lckIsDoorClosed = false;
  Serial.println("Door Open");
 }
 return lckIsDoorClosed;
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
