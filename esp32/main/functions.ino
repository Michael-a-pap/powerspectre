
void mstart()
{
  static int k;
  if (((icolorSum)*(L/100.0))<(colorSum)&& Start==1)
  {
    current_source(Imax,1);
    k++; 
    Serial.println("Started");
    Serial.println(k);
    if (dataFile) { 
      colorSensor(PRNT);
      electrical_measurement();
      dataString="";
      dataFile.println(dataString);  
    }
    else{
    Serial.println("error opening ");
    Serial.print(fileName+i+extension);
    }
    //colorSensor();
    //electrical_measurement();
  }
  else if (Start==1)
  {
    current_source(Imax,0);
    dataFile.close();
    Serial.println("file closed");
    Serial.println(k);
    Start=0;
    k=0;
  }
}

int fileChecker() 
{
  int i=0;
    while (SD.exists(root+fileName+i+extension))
  {
    i++;
  }
  
  if (SD.exists(root+fileName+extension)) {
    dataFile = SD.open(root+fileName+i+extension, FILE_WRITE);
  }
  else 
  {
    dataFile = SD.open(root+fileName+extension, FILE_WRITE);
  }
  return i;
}

void electrical_measurement()
{
 // String dataString="";
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  /*
  dataString+=loadvoltage;
  dataString+=',';
  dataString+=current_mA;
  dataString+=',';
  */
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");
  //dataFile.print(dataString);
}

void current_source(float Imax, boolean enable)
{
  static int dac_value;
  dac_value=(0.56177+16.7147*Imax+(0.12596*pow(Imax,2))+(-0.00415*pow(Imax,3))+(7.20418*pow(10,-5)*pow(Imax,4))+(-7.16147*pow(10,-7)*pow(Imax,5))+(4.21227*pow(10,-9)*pow(Imax,6))+(-1.43979*pow(10,-11)*pow(Imax,7))+(2.61529*pow(10,-14)*pow(Imax,8))+(-1.91803*pow(10,-17)*pow(Imax,9)));
  if (enable==1) MCP.setValue(dac_value);  else MCP.setValue(0);
  
}


void colorSensor (boolean prnt){
  static bool flag;
  
  if(prnt==0){
     uint32_t frequency;
    dataString="";
    Serial.print("RAW");
    Serial.print("RGBW [");  //we are printing only RAW i.e. frequency
    digitalWrite (S2,0);
    digitalWrite (S3,0); // RED
    //uint32_t frequency = FreqCountESP.read(); //discard the previous measurement
    while(!FreqCountESP.available());
    frequency = FreqCountESP.read();
    Serial.print(frequency); Serial.print(',');
    dataString+=frequency; dataString+=',';
    colorSum=frequency;
    digitalWrite (S2,1);
    digitalWrite (S3,1); // GREEN
    while(!FreqCountESP.available());
    frequency = FreqCountESP.read();
    Serial.print(frequency); Serial.print(',');
    dataString+=frequency; dataString+=',';
    colorSum+=frequency;
    digitalWrite (S2,0);
    digitalWrite (S3,1); // BLUE
    while(!FreqCountESP.available());
    frequency = FreqCountESP.read();
    Serial.print(frequency); Serial.print(',');
    dataString+=frequency; dataString+=',';
    colorSum+=frequency;
    digitalWrite (S2,1);
    digitalWrite (S3,0); // WHITE
    while(!FreqCountESP.available());
    frequency = FreqCountESP.read();
    Serial.print(frequency); 
    dataString+=frequency; dataString+=',';
    colorSum+=frequency;
    Serial.println(']');
    dataFile.print(dataString);
    digitalWrite (S2,0);
    digitalWrite (S3,0); // RED
  }
  else if (prnt==1){
    current_source(Imax,1);
     uint32_t frequency;
    Serial.print("RAW");
    Serial.print("RGBW [");  //we are printing only RAW i.e. frequency
    digitalWrite (S2,0);
    digitalWrite (S3,0); // RED
   // uint32_t frequency = FreqCountESP.read(); //discard the previous measurement
    while(!FreqCountESP.available());
    frequency = FreqCountESP.read();
    Serial.print(frequency); Serial.print(',');
    icolorSum=frequency;
    digitalWrite (S2,1);
    digitalWrite (S3,1); // GREEN
    while(!FreqCountESP.available());
    frequency = FreqCountESP.read();
    Serial.print(frequency); Serial.print(',');
    icolorSum+=frequency;
    digitalWrite (S2,0);
    digitalWrite (S3,1); // BLUE
    while(!FreqCountESP.available());
    frequency = FreqCountESP.read();
    Serial.print(frequency); Serial.print(',');
    icolorSum+=frequency;
    digitalWrite (S2,1);
    digitalWrite (S3,0); // WHITE
    while(!FreqCountESP.available());
    frequency = FreqCountESP.read();
    Serial.print(frequency);
    icolorSum+=frequency;
    colorSum=icolorSum;
    Serial.println(']');
    digitalWrite (S2,0);
    digitalWrite (S3,0); // RED
    current_source(Imax,0);
  }
}


////SERVER////
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
void getData()
{
  /*
  static int i;
  String json= "{\"value\":";
  json+='"';
  json+=i;
  json+='"';
  json+="}";
  webSocket.broadcastTXT(json.c_str(), json.length());
  i++;
  */
  if (Start==1)
  {
    String json= "{\"value\":";
    json+='"';
    json+="RUNNING";
    json+='"';
    json+="}";
    webSocket.broadcastTXT(json.c_str(), json.length()); 
  }
  else
  {
    String json= "{\"value\":";
    json+='"';
    json+="IDLE";
    json+='"';
    json+="}";
    webSocket.broadcastTXT(json.c_str(), json.length());     
  }
}

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length)
{
  /*
  if (Start==1)
  {
    
  }
  */
}
