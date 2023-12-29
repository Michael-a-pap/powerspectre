 void cmd()
 {
  char ch;  
  while (Serial.available())
  {
    ch=Serial.read();
    sdata+=ch;
    if (ch=='\r')
    {
       // sdata.trim();
       Serial.println(sdata.substring(0,9));
      if(sdata.substring(0,9)==F("filename="))
      {
        sdata.trim();
        int i;
        i=sdata.length();
        ch=Serial.read();
        fileName=sdata.substring(9,i);
        Serial.println(fileName);
        sdata="";
        filenameChange=1;
      }
      else if (sdata.substring(0,5)==F("Vmax="))
      {
        int i;
        i=sdata.length();
        ch=Serial.read();
        vmax=sdata.substring(5,i).toInt();
        Serial.println(vmax);
        sdata="";
      }
      else if (sdata.substring(0,5)==F("Imax="))
      {
        int i;
        i=sdata.length();
        ch=Serial.read();
        Imax=sdata.substring(5,i).toFloat();
        current_source(Imax,0);
        Serial.println(Imax);
        sdata="";
      }
      else if (sdata.substring(0,3)==F("L%="))
      {
        int i;
        i=sdata.length();
        ch=Serial.read();
        L=sdata.substring(3,i).toInt();
        Serial.println(L);
        sdata="";
      }
      else if (sdata.substring(0,9)==F("duration="))
      {
        int i;
        i=sdata.length();
        ch=Serial.read();
        mtime=sdata.substring(9,i).toInt();
        Serial.println(mtime);
        sdata="";
      }
      else if (sdata.substring(0,7)==F("points="))
      {
        int i;
        i=sdata.length();
        ch=Serial.read();
        mpoints=sdata.substring(7,i).toInt();
        Serial.println(mpoints);
        sdata="";
      }      
      else if (sdata.substring(0,5)==F("START"))
      {
        colorSensor(INIT);
        Start=1;
        Serial.println("Started!");
        sdata="";
      }
      else if (sdata.substring(0,6)==F("single"))
      {
        colorSensor(INIT);
        sdata="";
      }
      else
      {
        Serial.println((String)"unknown command"+'"'+sdata+'"');
        sdata="";
      }
    }
  } 
 }
