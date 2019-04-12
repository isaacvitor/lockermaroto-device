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

bool existsParameter(String pathParameter){
  return SPIFFS.exists(pathParameter);
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

void writeParameterInFS(String pathParameterFile, String parameter) {
  File mFile = SPIFFS.open(pathParameterFile, "w+");
  if (!mFile){
    Serial.println("writeParameterInFS - Open File Error");
  } else {
    mFile.println(parameter);
    Serial.println("Parameter Saved");
  }
  mFile.close();
}

String readParameterFromFS(String pathParameterFile) {
  File parameterFile = SPIFFS.open(pathParameterFile, "r");
  if (!parameterFile){
    Serial.println("readParameterFromFS - Open File Error");
  }
  String parameter = parameterFile.readStringUntil('\r');
  parameterFile.close();
  return parameter;
}

bool removeParameterFromFS(String pathParameterFile){
  if(SPIFFS.exists(pathParameterFile)){
    return SPIFFS.remove(pathParameterFile);
  }
}
