#include "FS.h"
//#include "SPIFFS.h"

bool openFileSystem(void){
  if (!SPIFFS.begin()){
    Serial.println("\nOpen File System Error");
    return false;
  }else{
    Serial.println("\nFile System Opened");
     return true;
  }
}

void formatFileSystem(){
  SPIFFS.format();
}

void writeFile(String content, String path) {
  File mFile = SPIFFS.open(path, "w+");
  if (!mFile){
    Serial.println("writeFile - Open File Error");
  } else {
    mFile.println(content);
    Serial.print("Content Saved");
  }
  mFile.close();
}

void writeParameterOnFile(String pathParameter, String parameter) {
  File mFile = SPIFFS.open(pathParameter, "w+");
  if (!mFile){
    Serial.println("writeParameterOnFile - Open File Error");
  } else {
    mFile.println(pathParameter);
    Serial.println("Parameter Saved");
  }
  mFile.close();
}

String readParameterOnFile(String pathParameterFile) {
  File parameterFile = SPIFFS.open(pathParameterFile, "r");
  if (!parameterFile){
    Serial.println("readParameterOnFile - Open File Error");
  }
  String parameter = parameterFile.readStringUntil('\r');
  parameterFile.close();
  return parameter;
}

String readStringOnFile(String path){
  File stringFile = SPIFFS.open(path, "r");
  if (!stringFile){
    Serial.println("readStringOnFile - Open File Error");
  }
  String content = stringFile.readString();
  stringFile.close();
  return content;
}
