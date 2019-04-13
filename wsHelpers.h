void registerOnGateway(){
  webSocket.sendTXT("42[\"lockerRegister\",{\"mac\":\""+LCK_LOCKER_MAC+"\",\"name\":\""+readDeviceName+"}]");
}

void updateRegister(JsonObject data){
  isRegister = true;
  LOCKER_ID = data["_id"].as<String>();
}


//Handlers to emits
void lockRemote(JsonObject data){
  String uid = data["userID"].as<String>();
  if(canLock(uid)){
    //Do something
  }
}

void unlockRemote(JsonObject data){
  String uid = data["userID"].as<String>();
  if(canUnlock(uid)){
    //Do something
  }
}

void addUserRemote(JsonObject data){
  
}

void removeUserRemote(JsonObject data){
  
}

void handleRemoteEvent(String eventName, JsonObject data){
  Serial.print("Event: ");
  Serial.println(eventName);
  if(eventName.equals( "lockerRegister")){
   updateRegister(data); 
  } else if(eventName.equals( "lockRemote")){
   lockRemote(data); 
  }else if(eventName.equals( "unlockRemote")){
   unlockRemote(data); 
  }
  
}

void handlePayload(String payload){
  String command = payload;
  command.remove(0, 2);
  deserializeJson(jsonHelper, command);
  handleRemoteEvent(jsonHelper[0].as<String>(), jsonHelper[1]);
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length){
  switch (type){
  case WStype_DISCONNECTED:{
    Serial.println("[WS] Disconnected!");
    isConnected = false;
  }
  break;
  case WStype_CONNECTED:{
    Serial.println("[WS] Connected");
    isConnected = true;
    webSocket.sendTXT("5");
  }
  break;
  case WStype_TEXT:{
    Serial.print("[WS] Text message arrived (length): ");
    Serial.println(length);
    String strPayload = String((char *)payload);
    handlePayload(strPayload);
  }
  break;
  case WStype_BIN:
    Serial.print("[WS] Binary message arrived (length): ");
    Serial.println(length);
    hexdump(payload, length);
    break;
  }
}

void setupWebSocket(){
  uint16_t port = readGatewayPort.toInt();
  Serial.print("Gateway: ");
  Serial.println(readGateway);
  Serial.print("Gateway Port: ");
  Serial.println(port);
  //LOCKER_ID
  //char *ipGateway = readGateway.c_str();
  webSocket.beginSocketIO(readGateway.c_str(), port);
  //webSocket.beginSocketIO("192.168.15.2", 3000);
  webSocket.onEvent(webSocketEvent);
}

void healthCheck(){
  uint64_t now = millis();
  if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
      heartbeatTimestamp = now;
      // socket.io heartbeat message
      webSocket.sendTXT("2");
  }
}
