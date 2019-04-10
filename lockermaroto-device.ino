
/*

    LockerMaroto - (lm)
    
    O LockerMaroto é uma PoC(Proof of Concept) com objetivo de validar e estudar um sistema de controle de acesso a armários.
    A ideia básica é que dado um inventário de equipamentos X, devidamente catalogado, armazenado em compartimentos devidamente indentificados, 
    possam ser acessados de uma maneira inteligente e/ou menos burocrática do que o uso de uma chave convencional.

    Builded on Nodemcu Lolin v1.0

    Isaac Vitor - 2019

    STATES:

    O locker deverá manter seu estado nele próprio e uma cópia no servidor, os estados possível são:
    Trancado = (Porta fechada + trava ativada + LockerState=[True]/false) => lckCodeState="111"||"110"
    Destrancado = (Porta fechada + trava desativada  + LockerState=[False]/True ) => lckState="100"||"101"
    Aberto = (Porta aberta + trava aberta + LockerState=[False]/True) => lckCodeState="000"
    Desconhecido = Em tese apenas no servidor, caso o mesmo não tenha comunicação com o locker registrado.=> lckState=undefined

*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ESP8266WebServer.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;



#define MESSAGE_STATE_INTERVAL 3000
#define HEARTBEAT_INTERVAL 25000

uint64_t stateTimestamp = 0;
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

//Helpers
#include "fsHelpers.h"
#include <Servo.h>

#include <SPI.h>
#include <MFRC522.h>
  


//PATHS
  const String LCK_STATE_PATH = "/config/lckState.txt";
  const String LCK_USERS_UID_BASE_PATH = "/usersUID/";
  const String LCK_USERS_ID_BASE_PATH = "/usersID/";
  const String LCK_ITENS_UID_BASE_PATH = "/itensUID/";
  //Saved Parameters
  const String SSID_SAVED = "/config/ssid";
  const String PASSWORD_SAVED = "/config/password";
  const String GATEWAY_SAVED = "/config/gateway";
  const String GATEWAY_PORT_SAVED = "/config/port";
  const String DEVICE_NAME_SAVED = "/config/device";


//Operation Mode - HIGH = ClientMode LOW = AccessPointMode
  const int BT_CHANGE_MODE = A0;
  boolean accessPointMode = false;
  
  String mac = "APS-" + WiFi.softAPmacAddress();
  const char *ssidAP = mac.c_str();
  const char *passwordAP = "lockermaroto";
  String errorMessage;

  String readSSID;
  String readPassword;
  String readGateway;
  String readGatewayPort;
  String readDeviceName;

 
  #include "wsHelpers.h"
  #include "index.h" 
  #include "saved.h" 
  ESP8266WebServer server(80);
  #include "accessPointHelpers.h"
  
  boolean LockerState = false;
  const String LCK_LOCKER_MAC = WiFi.softAPmacAddress();
//Servo (Lock - lck) - dependencies and initializations
  Servo lock;
  boolean lckIsLocked = false;
  const int LOCK_PIN = 4; // D2
  const int LOCK_POSITION = 90;  //Posição que representa o fechamento da tranca
  const int UNLOCK_POSITION = 0; //Posição que representa a abertura da tranca
  
  String lckCodeState;
  const String CODE_STATE_LOCKED = "111";//lckIsDoorClosed + lckIsLocked + LockerState = lckCodeState
  const String CODE_STATE_UNLOCKED = "101";//lckIsDoorClosed + lckIsLocked + LockerState = lckCodeState
  const String CODE_STATE_OPEN = "000";//lckIsDoorClosed + lckIsLocked + LockerState = lckCodeState

//ReedSwitch (Locker State Detector - lckStateDetector ) - dependencies and initializations
  const int lckSwitchOpenDetectPin = 5; // D1
  boolean lckIsDoorClosed = 0;

//RFID (Locker Input Interface-lckInputInterface) - dependencies and initializations -
  const int RST_PIN = 0; //D3?
  const int SS_PIN = 16; //D0
  MFRC522 lckInputInterface(SS_PIN, RST_PIN); //Criando uma estância da MFRC522
  #include "lckInputInterfaceHelpers.h"
  


void setup(){
  Serial.begin(115200);
  //Abrindo sistema de arquivos
  openFileSystem();
  
  //Definindo o botão do modo de operação
  pinMode(BT_CHANGE_MODE, INPUT);
 
  //Definindo o modo de operação ACCESSPOINT X CLIENT
  if(analogRead(BT_CHANGE_MODE) == HIGH){
    Serial.println("AccessPoint MODE");
    startAccessPoint();
  } else {
    Serial.println("Client MODE");
    //lckStateDetector
    pinMode(lckSwitchOpenDetectPin, INPUT);
    
    //lckInputInterface Init
    SPI.begin();
    lckInputInterface.PCD_Init();
  
    //lock
    lock.attach(LOCK_PIN);

    //Conectando WiFi and WebSocket
    setupWiFiWebSocket();
    setupWebSocket();
      
    //HARDCODE - REMOVE AFTER
    lckLock();
    lckUpdateStateLocker(); //REMOVE AFTER
    /*saveUserByUID(3496392741);
    lckUnlock();
    delay(500);*/
  }
}

void loop(){
  if(accessPointMode){
    server.handleClient();
  }else {
    unsigned long uid;
    webSocket.loop();
    if(isConnected) {
      //Checando o estado da conexão com o gateway
      healthCheck();  
      lckUpdateStateLocker();
      if(lckInputInterface.PICC_IsNewCardPresent()) {
        uid = lckGetUID();
        if(uid != -1){
          //DO AN ACTION:
          //TODO - how does it will work?
          if(getUserByUIDSaved(uid) != ""){
            if(lckIsDoorClosed && !lckIsLocked){
              lckLock();
            }else if(lckIsDoorClosed && lckIsLocked){
              lckUnlock();
            }
          }
          
        }
      }
      sendLockerSateToGateway();

    }
    
  }
}

//Gateway methods
bool sendUIDToGateway(unsigned long uid){
  Serial.print("sendUIDToGateway - ");
  Serial.println(uid);
  return false;
}

void sendLockerSateToGateway(){
  uint64_t now = millis();
  if((now - stateTimestamp) > MESSAGE_STATE_INTERVAL) {
    stateTimestamp = now;
    String state = lckUpdateStateLocker();
    //lckIsDoorClosed + lckIsLocked + LockerState = lckCodeState
    String pinsState = "{\"lckIsDoorClosed\":"+String(lckIsDoorClosed)+",\"lckIsLocked\":"+lckIsLocked+",\"LockerState\":"+LockerState+"}";
    webSocket.sendTXT("42[\"lockerState\",{\"lockerMac\":\""+LCK_LOCKER_MAC+"\",\"lockerState\":\""+state+"\", \"pinsState\":"+pinsState+"}]");
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

boolean lckUpdateDoorState(){
 int s = digitalRead(lckSwitchOpenDetectPin);
 if (s == HIGH) {
  lckIsDoorClosed = true;
 } else {
  lckIsDoorClosed = false;
 }
 return lckIsDoorClosed;
}

boolean lckUpdateLockState(){
  int angle = lock.read();
  if(angle == LOCK_POSITION){
    lckIsLocked = true;
  }else if(angle == UNLOCK_POSITION){
    lckIsLocked = false;
  } else {
    lckIsLocked = false;
  }
  return lckIsLocked;
}

String lckUpdateStateLocker(){
  lckUpdateDoorState();
  lckUpdateLockState();
  
  if(lckIsLocked && lckIsDoorClosed){
    LockerState = true;
    lckCodeState = CODE_STATE_LOCKED;
  }else if(!lckIsLocked){
    LockerState = LockerState ? LockerState : false;
    lckCodeState = CODE_STATE_UNLOCKED;
  } else if(!lckIsLocked && !lckIsDoorClosed){
    LockerState = false;
    lckCodeState = CODE_STATE_OPEN;
  }
  return lckCodeState;
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
