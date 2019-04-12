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

#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ESP8266WebServer.h>
#include <Hash.h>

#include <SPI.h>
#include <MFRC522.h>

#include <Servo.h>

//Helpers sem dependencias
#include "fsHelpers.h" //helpers para uso do FileSystem
#include "index.h" //Página index do serviço de configuração
#include "saved.h" //Página save do serviço de configuração

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

//DynamicJsonDocument para serializar e desserializar JSON
const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + 220;
DynamicJsonDocument jsonHelper(capacity);

//Definição de PINS
  #define PIN_BUZZER D8 //Pin do Buzzer
  const int CHANGE_MODE_PIN = A0; //Pin do controle de MODO: AP or Client
  const int LOCK_PIN = 4; // D2 //Pin da tarava - Lock

  const int RST_PIN = 0; //D3? Pin reset do MFRC522
  const int SS_PIN = 16; //D0 Pin SS fo MFRC522
  const int DETECT_DOOR_STATE_PIN = 5; // D1 Pin da chave que controla se a porta está aberta ou fechada

//Controle do estado da aplicação
  #define MESSAGE_STATE_INTERVAL 5000
  uint64_t stateTimestamp = 0;

  #define HEARTBEAT_INTERVAL 25000
  uint64_t heartbeatTimestamp = 0;

  bool isConnected = false; //Controla a conexão do websocket
  bool isRegister = false; //Controla se o locker está registrado no gateway
  boolean LockerState = false; //Responsável pelo estado da aplicação
  boolean lckIsLocked = false; //Controla o estado da trava do locker
  boolean isOnAccessPointMode = false; //Controla o estado do WiFi AccessPoint[true] / Client[false]
  boolean lckIsDoorClosed = 0;

  String LOCKER_ID = "";//ID do locker do gateway
  const String LCK_LOCKER_MAC = WiFi.softAPmacAddress();// MAC Address do Locker

//PATHS
  const String LCK_STATE_PATH = "/config/lckState.txt";
  const String LCK_USERS_UID_BASE_PATH = "/usersUID/";
  const String LCK_USERS_ID_BASE_PATH = "/usersID/";
  const String LCK_ITENS_UID_BASE_PATH = "/itensUID/";
  //Saved Parameters
  const String SSID_SAVED_PATH = "/config/ssid";
  const String PASSWORD_SAVED_PATH = "/config/password";
  const String GATEWAY_SAVED_PATH = "/config/gateway";
  const String GATEWAY_PORT_SAVED_PATH = "/config/port";
  const String DEVICE_NAME_SAVED_PATH = "/config/device";


//Variáveis para AccessPoint Mode
  String mac = "LOCKER-" + WiFi.softAPmacAddress(); //Composição do nome do AP
  const char *ssidAP = mac.c_str(); //SSID do AP
  const char *passwordAP = "lockermaroto";//Senha do AP

  String readSSID;//Global
  String readPassword;//Global
  String readGateway;//Global
  String readGatewayPort;//Global
  String readDeviceName;//Global

  ESP8266WebServer server(80);//Intanciando o webserver na porta 80
  #include "accessPointHelpers.h"
  
//Servo (Lock - lck) - dependencies and initializations
  Servo lock;
  
  const int LOCK_POSITION = 90;  //Posição que representa o fechamento da tranca
  const int UNLOCK_POSITION = 0; //Posição que representa a abertura da tranca
  
  String lckCodeState;
  const String CODE_STATE_LOCKED = "111";//lckIsDoorClosed + lckIsLocked + LockerState = lckCodeState
  const String CODE_STATE_UNLOCKED = "101";//lckIsDoorClosed + lckIsLocked + LockerState = lckCodeState
  const String CODE_STATE_OPEN = "000";//lckIsDoorClosed + lckIsLocked + LockerState = lckCodeState
  
//ReedSwitch (Locker State Detector - lckStateDetector ) - dependencies and initializations
  

//RFID (Locker Input Interface-lckInputInterface) - dependencies and initializations -
  
  MFRC522 lckInputInterface(SS_PIN, RST_PIN); //Criando uma estância da MFRC522
  #include "lckInputInterfaceHelpers.h"
  
 //WS
  void sendUIDToGateway(unsigned long uid){
    String uidStr = String( uid );
    webSocket.sendTXT("42[\"lockerUID\",{\"lockerMac\":\""+LCK_LOCKER_MAC+"\",\"uid\":\""+uidStr+"}]");
  }
  
  void updateStateFromGateway(){
    String state = lckUpdateStateLocker();
    String pinsState = "{\"lckIsDoorClosed\":"+String(lckIsDoorClosed)+",\"lckIsLocked\":"+lckIsLocked+",\"LockerState\":"+LockerState+",\"isRegister\":"+isRegister+"}";
    webSocket.sendTXT("42[\"lockerState\",{\"mac\":\""+LCK_LOCKER_MAC+"\",\"name\":\""+readDeviceName+"\",\"codeState\":\""+state+"\",\"_id\":\""+LOCKER_ID+"\", \"pins\":"+pinsState+"}]");
  }
  
  void sendLockerStateToGateway(){
    //Serial.println("sendLockerStateToGateway");
    uint64_t now = millis();
    if((now - stateTimestamp) > MESSAGE_STATE_INTERVAL) {
      stateTimestamp = now;
      updateStateFromGateway();
    }
  }
  
  void updateStateByUser(String userUID){
    String state = lckUpdateStateLocker();
    String user = "{\"ekey\":\""+userUID+"\"}";
    String pinsState = "{\"lckIsDoorClosed\":"+String(lckIsDoorClosed)+",\"lckIsLocked\":"+lckIsLocked+",\"LockerState\":"+LockerState+",\"isRegister\":"+isRegister+"}";
    webSocket.sendTXT("42[\"lockerState\",{\"mac\":\""+LCK_LOCKER_MAC+"\",\"name\":\""+readDeviceName+"\",\"codeState\":\""+state+"\",\"_id\":\""+LOCKER_ID+"\", \"pins\":"+pinsState+",\"user\":"+user+"}]");
  }

//Lock
  //Buzzer Functions
  void buzzerToLock(){
    digitalWrite(PIN_BUZZER, HIGH);
    delay(100);
    digitalWrite(PIN_BUZZER, LOW);
    delay(100);
    digitalWrite(PIN_BUZZER, HIGH);
    delay(100);
    digitalWrite(PIN_BUZZER, LOW);  
  }
  
  void buzzerToUnlock(){
    digitalWrite(PIN_BUZZER, HIGH);
    delay(500);
    digitalWrite(PIN_BUZZER, LOW);
  }
  
  void lckLock(){
    lock.write(LOCK_POSITION);
    lckIsLocked = true;
  }
  
  void lckUnlock(){
    lock.write(UNLOCK_POSITION);
    lckIsLocked = false;
  }

  bool canLock(String userUID){
    Serial.println("canLock");
    lckUpdateDoorState();
    lckUpdateLockState();
    if(lckIsDoorClosed && !lckIsLocked){
      lckLock();
      buzzerToLock();
      updateStateByUser(userUID);
      return true;
    }
    return false;
  }

  bool canUnlock(String userUID){
    Serial.println("canUnlock");
    lckUpdateDoorState();
    lckUpdateLockState();
    if(lckIsDoorClosed && lckIsLocked){
      lckUnlock();
      updateStateByUser(userUID);
      buzzerToUnlock();
      return true;
    }
    return false;
  }
  
  boolean lckUpdateDoorState(){
   int s = digitalRead(DETECT_DOOR_STATE_PIN);
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
  
  //Método que atualiza todos os sensores do Locker
  String lckUpdateStateLocker(){
    lckUpdateDoorState();
    lckUpdateLockState();
    
    if(lckIsLocked && lckIsDoorClosed){
      LockerState = true;
      lckCodeState = CODE_STATE_LOCKED;
    }else if(!lckIsLocked && lckIsDoorClosed){
      LockerState = LockerState ? LockerState : false;
      lckCodeState = CODE_STATE_UNLOCKED;
    } else if(!lckIsLocked && !lckIsDoorClosed){
      LockerState = false;
      lckCodeState = CODE_STATE_OPEN;
    } else {
      lckCodeState = "XXX";
   }
    return lckCodeState;
  }
  
  void saveUserByUID(String uid){
    String path = LCK_USERS_UID_BASE_PATH + uid;
    writeParameterInFS(path, uid);
    Serial.println("saveUserByUID - " + uid);
  }

  
  void checkRFID(){
    
    unsigned long uid;
    if(lckInputInterface.PICC_IsNewCardPresent()) {
      Serial.println("checkRFID() running" );
      uid = lckGetUID();
      if(uid != -1){
        String userUID = getUserByUIDSaved(uid);
        if(userUID != ""){
          Serial.println(lckIsDoorClosed);
          Serial.println(lckIsLocked);
          if(lckIsDoorClosed && !lckIsLocked){
            if(canLock(userUID)){
              //Fazer alguma coisa depois de trancar
              return;
            }
            
          } else if(lckIsDoorClosed && lckIsLocked){
            Serial.println("uid louco");
            Serial.println(lckIsDoorClosed);
            Serial.println(lckIsLocked);
            
            if(canUnlock(userUID)){
              //Fazer alguma coisa depois de destrancar
              return;
            }
          }
          
        } else{
          //sendUIDToGateway();
          digitalWrite(PIN_BUZZER, HIGH);
          delay(10);
          digitalWrite(PIN_BUZZER, LOW);
          delay(10);
          digitalWrite(PIN_BUZZER, HIGH);
          delay(10);
          digitalWrite(PIN_BUZZER, LOW);
          delay(10);
          digitalWrite(PIN_BUZZER, HIGH);
          delay(10);
          digitalWrite(PIN_BUZZER, LOW);
          delay(10);
        }
      }
    }
  }
  
  #include "wsHelpers.h"


void setup(){
  Serial.begin(115200);
  //Abrindo sistema de arquivos
  openFileSystem();
  
  //Definindo o botão do modo de operação
  pinMode(CHANGE_MODE_PIN, INPUT);
 
  //Definindo o modo de operação ACCESSPOINT X CLIENT
  if(analogRead(CHANGE_MODE_PIN) == HIGH){
    Serial.println("AccessPoint MODE");
    startAccessPoint();
  } else {
    Serial.println("Client MODE");
    //lckStateDetector
    pinMode(DETECT_DOOR_STATE_PIN, INPUT);
    
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
    // lckUnlock();
    lckUpdateStateLocker(); //REMOVE AFTER
    pinMode(PIN_BUZZER, OUTPUT);
    //saveUserByUID("3496392741");
    /*lckUnlock();
    delay(500);*/
  }
}

void loop(){
  if(isOnAccessPointMode){
    server.handleClient();
  }else {
    webSocket.loop();
    unsigned long uid;
    //Atualizadno os states
    lckUpdateStateLocker();

    //Caso conectado
    if(isConnected) {
      //Checando o estado da conexão com o gateway
      healthCheck(); 
      checkRFID();
      //Enviando o state do locker
      sendLockerStateToGateway();
    }else{
      //Descobrir o que fazer caso não esteja conectado
      checkRFID();
    }
    
  }
}
