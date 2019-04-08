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

bool getUserByUIDSaved(unsigned long uid){
  String path = LCK_USERS_UID_BASE_PATH + uid;
  Serial.println("getUserByUIDSaved - " + path);
  return false;
}
bool getItemByUIDSaved(unsigned long uid){
  String path = LCK_ITENS_UID_BASE_PATH + uid;
  Serial.println("getItemByUIDSaved - " + path);
  return false;
}

bool sendUIDToGateway(unsigned long uid){
  Serial.print("sendUIDToGateway - ");
  Serial.println(uid);
  return false;
}
