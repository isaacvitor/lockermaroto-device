void handleHome(){
  String s = INDEX_page;
  server.send(200, "text/html", s);
}

void handleSave(){
  static String params = "";
  readSSID = server.arg("pSSID");
  readPassword = server.arg("pPassword");
  readGateway = server.arg("pGateway");
  readDeviceName = server.arg("pDeviceName");
  
  writeParameterOnFile(SAVED_SSID, readSSID);
  writeParameterOnFile(SAVED_PASSWORD, readPassword);
  writeParameterOnFile(SAVED_GATEWAY, readGateway);
  writeParameterOnFile(SAVED_DEVICE_NAME, readDeviceName);
  
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
  readSSID = readParameterOnFile(SAVED_SSID);
  delay(300);
  readPassword = readParameterOnFile(SAVED_PASSWORD);
  delay(300);
  readGateway = readParameterOnFile(SAVED_GATEWAY);
  delay(300);
  readDeviceName = readParameterOnFile(SAVED_DEVICE_NAME);
  delay(300);
}

void setupWiFi(){
  Serial.println("Loading saved parameters...");
  readSavedWifiParameters();
  Serial.print("Connecting to ");
  Serial.println(readSSID);
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
