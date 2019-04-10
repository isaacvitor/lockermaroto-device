void handleHome(){
  String s = INDEX_page;
  server.send(200, "text/html", s);
}

void handleSave(){
  static String params = "";
  readSSID = server.arg("pSSID");
  readPassword = server.arg("pPassword");
  readGateway = server.arg("pGateway");
  readGatewayPort = server.arg("pGatewayPort");
  readDeviceName = server.arg("pDeviceName");
  
  writeParameterOnFile(SSID_SAVED, readSSID);
  writeParameterOnFile(PASSWORD_SAVED, readPassword);
  writeParameterOnFile(GATEWAY_SAVED, readGateway);
  writeParameterOnFile(GATEWAY_PORT_SAVED, readGatewayPort);
  writeParameterOnFile(DEVICE_NAME_SAVED, readDeviceName);
  
  String s = SAVED_page;
  server.send(200, "text/html", s); 
}

void restartAcessPoint(){
  Serial.println("Restarting AP...");
  ESP.restart();
}

void startAccessPoint(){
  accessPointMode = true; 
  
  WiFi.softAP(ssidAP, passwordAP);
  Serial.println();
  Serial.print("AccessPoint IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("AccessPoint MAC address: ");
  Serial.println(WiFi.softAPmacAddress());
  
  server.on("/", handleHome);
  server.on("/submit", handleSave);
  server.on("/restart", restartAcessPoint);
  server.begin();
  
  Serial.println("AccessPoint Listening");
}

void definedOperationMode(){
  if(digitalRead(BT_CHANGE_MODE) == HIGH){
    Serial.println("AccessPooint MODE");
    startAccessPoint();
  } else {
    Serial.println("Client MODE");
  }
}

void readSavedWifiParameters(){
  readSSID = readParameterOnFile(SSID_SAVED);
  delay(300);
  readPassword = readParameterOnFile(PASSWORD_SAVED);
  delay(300);
  readGateway = readParameterOnFile(GATEWAY_SAVED);
  delay(300);
  readGatewayPort = readParameterOnFile(GATEWAY_PORT_SAVED);
  delay(300);
  readDeviceName = readParameterOnFile(DEVICE_NAME_SAVED);
  delay(300);
}

void setupWiFi(){
  Serial.println("Loading saved parameters...");
  readSavedWifiParameters();
  Serial.print("Connecting to ");
  Serial.println(readSSID);
  
  //To MQTT
  WiFi.begin(readSSID.c_str(), readPassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupWiFiWebSocket(){
  Serial.println("Loading saved parameters...");
  readSavedWifiParameters();
  Serial.print("Connecting to ");
  Serial.println(readSSID);
 
  WiFiMulti.addAP(readSSID.c_str(), readPassword.c_str());
  while (WiFiMulti.run() != WL_CONNECTED) {
   Serial.print(".");
   delay(100);
  }
  
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
