
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
  
  writeParameterInFS(SSID_SAVED_PATH, readSSID);
  writeParameterInFS(PASSWORD_SAVED_PATH, readPassword);
  writeParameterInFS(GATEWAY_SAVED_PATH, readGateway);
  writeParameterInFS(GATEWAY_PORT_SAVED_PATH, readGatewayPort);
  writeParameterInFS(DEVICE_NAME_SAVED_PATH, readDeviceName);
  
  String s = SAVED_page;
  server.send(200, "text/html", s); 
}

void restartAcessPoint(){
  Serial.println("Restarting AP...");
  
  //isOnAccessPointMode = false;
  //ESP.restart(); //Ligth
  
  ESP.reset(); //PUNK
}

void startAccessPoint(){
  isOnAccessPointMode = true; 
  
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

void readSavedWifiParameters(){
  readSSID = readParameterFromFS(SSID_SAVED_PATH);
  readPassword = readParameterFromFS(PASSWORD_SAVED_PATH);
  readGateway = readParameterFromFS(GATEWAY_SAVED_PATH);
  readGatewayPort = readParameterFromFS(GATEWAY_PORT_SAVED_PATH);
  readDeviceName = readParameterFromFS(DEVICE_NAME_SAVED_PATH);
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
