void setup_main()
{
 // uint32_t currentFrequency;
  FreqCountESP.begin(colorSensor_freq_pin,timer_timeout);
  measurement.attach_ms(1000,mstart);

  
  Serial.begin(115200);
  
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
   // don't do anything more:
  //  while (1);
  }
  if (! ina219.begin()) 
  {
    Serial.println("Failed to find INA219 chip");
    while (1)
    delay(10); 
  }
  if (MCP.begin() == false)
  {
    Serial.println("Could not find MCP4725");
  }
  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  digitalWrite(S2,0);
  digitalWrite(S3,0);
}

void setup_server(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_FILENAME)) {
      inputMessage = request->getParam(PARAM_FILENAME)->value();
      Serial.print("filename=");
      Serial.print('\r');
      Serial.print(inputMessage);
      fileName=inputMessage;
      filenameChange=1;
      inputParam = PARAM_FILENAME;
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
     if (request->hasParam(PARAM_VMAX)) {
      inputMessage = request->getParam(PARAM_VMAX)->value();
      //Serial.print("Vmax=");
      //Serial.print('\r');
      //Serial.print(inputMessage);
      inputParam = PARAM_VMAX;
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
     if (request->hasParam(PARAM_IMAX)) {
      inputMessage += request->getParam(PARAM_IMAX)->value();
      Serial.print(inputMessage);
      Imax=inputMessage.toFloat();
      Serial.print(Imax);

    }
     if (request->hasParam(PARAM_START)) {
      inputMessage += request->getParam(PARAM_START)->value();
      inputParam += PARAM_START;
      Start=1;
      Serial.println("Started!");
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    //Serial.println(inputMessage);
    request->send(200,"index_html",index_html);
    });
   /* request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });*/
  server.onNotFound(notFound);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  timer.attach(5,getData);
}
