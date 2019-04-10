void handlePayload(String payload){
  Serial.print("PAYLOAD: ");
  Serial.println(payload);  
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
  int port = readGatewayPort.toInt() || 3000;
  Serial.print("Gateway: ");
  Serial.println(readGateway);
  Serial.print("Gateway Port: ");
  Serial.println(readGatewayPort);
  
  //webSocket.beginSocketIO(readGateway, port);
  webSocket.beginSocketIO("192.168.15.2", 3000);
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

/*
uint64_t now = millis();

if(now - messageTimestamp > MESSAGE_INTERVAL) {
    messageTimestamp = now;
    // example socket.io message with type "messageType" and JSON payload
    webSocket.sendTXT("42[\"messageType\",{\"greeting\":\"hello\"}]");
}
*/
