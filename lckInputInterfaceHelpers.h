//lckInputInterface
unsigned long lckGetUID(){
  if ( ! lckInputInterface.PICC_ReadCardSerial()) {
    return -1;
  }
  unsigned long hex_num;
  hex_num =  lckInputInterface.uid.uidByte[0] << 24;
  hex_num += lckInputInterface.uid.uidByte[1] << 16;
  hex_num += lckInputInterface.uid.uidByte[2] <<  8;
  hex_num += lckInputInterface.uid.uidByte[3];
  lckInputInterface.PICC_HaltA();
  return hex_num;
}

String getUserByUIDSaved(unsigned long uid){
  String parameter = "";
  String path = LCK_USERS_UID_BASE_PATH + uid;
  parameter = readParameterFromFS(path);
  Serial.println("getUserByUIDSaved - " + parameter);
  return parameter;
}
String getItemByUIDSaved(unsigned long uid){
  String parameter = "";
  String path = LCK_ITENS_UID_BASE_PATH + uid;
  parameter = readParameterFromFS(path);
  Serial.println("getItemByUIDSaved - " + parameter);
  return parameter;
}
