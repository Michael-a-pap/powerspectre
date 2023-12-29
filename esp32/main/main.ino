#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <FreqCountESP.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Ticker.h> 
#include "MCP4725.h"

#define PRNT 0 //THIS IS FOR PRINTING THE VALUES ON SD
#define INIT 1 //THIS IS FOR FETCHING THE INITIAL VALUES
#define S2 33
#define S3 32
#define timer_timeout 1000
#define colorSensor_freq_pin 34
#define chipSelect 5 // FOR the SD
//==FUNCTIONS==
void mstart();
//============

//==PUBLIC VARIABLES==
String sdata="";
String fileName="test"; //file of the name that is stored in the SD card
String extension=".csv";  // file extension of the same file
char root='/';
String dataString;
int vmax,L=50,mtime,mpoints; //this is for the serial commumication
float colorSum,icolorSum,Imax;
boolean filenameChange,Start=0;
int i; // for the file increment
///SERVER////
// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "TY2103062 1198";
const char* password = "mike79531";

const char* PARAM_FILENAME = "flname";
const char* PARAM_VMAX = "V_max";
const char* PARAM_IMAX = "I_max";
const char* PARAM_L = "L";
const char* PARAM_CONFCON = "conconf";
const char* PARAM_START = "strt";
////////////
//====================


Adafruit_INA219 ina219;
MCP4725 MCP(0x60);  // 0x62 or 0x63
Ticker measurement;
File dataFile;

//SERVER//
AsyncWebServer server(80);
WebSocketsServer webSocket=WebSocketsServer(81);
Ticker timer;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<body onload="javascript:init()">

<head>
  <title>LMD lifetime color measurement thingy!!!</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
  <style>
  body {
    background-color: #000000;
  }
  #input1[type=text] {
    color: grey;
    border: 2px solid grey;
    border-radius: 4px;
  }
  #input2[type=text] {
    color: black;
    border: 2px solid grey;
    border-radius: 4px;
  }
  #input3[type=text] {
    color: black;
    border: 2px solid grey;
    border-radius: 4px;
  }
  #input4[type=text] {
    color: black;
    border: 2px solid grey;
    border-radius: 4px;
  }
  #input5[type=text] {
    color: black;
    border: 2px solid grey;
    border-radius: 4px;
  }        
  label{
    color:white;
  }
  </style>
  
  <form action="/get">
  <label for="filename" >Filename:</label><br>
  <input type="text" name="flname" id="filename" value="data"><br>
  <label for="V_max">Vmax(V):</label><br>
  <input type="text" name="V_max" id="V_max"><br>
  <label for="I_max">Imax(mA):</label><br>
  <input type="text" name="I_max" id="I_max"><br>
  <label for="Lifetime">L%(mW):</label><br>
  <input type="text" name="L" id="Lifetime"><br>
  <label for="confidence_constrain">Confidence constrain(%):</label><br>
  <input type="text" name="conconf" id="confidence_constrain"><br><br>
  <input type="submit" value="Submit">

  </form><br>
  <form action="/get">
  <button name="strt" value="1">START!</button>
  </form><br>
    
  <script>
  var webSocket;
    function init() {
        webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');
        webSocket.onmessage = function(event) {
        var data = JSON.parse(event.data);
        console.log(data.value);
      }
    console.log("den to yhnei re");
    }
  </script>
</body></html>)rawliteral";
/////////

void setup() {
  setup_main();
  setup_server();
}

void loop() {
   if(filenameChange==1)
  {
    i=fileChecker();
  }
  
  if (filenameChange==1)
  {
    dataString = "";
    Serial.println(fileName+i+extension);
    dataString += String(F("R,G,B,W,V(V),I(mA),Lifetime(%),Confidence constrain(%),Time(s)"));
    dataFile.println(dataString);
    colorSensor(INIT);
    Serial.print("color sum=");
    Serial.println(icolorSum);
    //dataFile.close();
    filenameChange=0;
   // Start=1;
  }
  cmd(); //Serial Communication/command prompt 
  webSocket.loop();
}
