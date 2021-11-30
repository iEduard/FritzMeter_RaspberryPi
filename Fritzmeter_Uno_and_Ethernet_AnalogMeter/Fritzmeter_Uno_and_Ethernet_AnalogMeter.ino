

/* Fritzmeter with Arduino UNO
 *  
 * Author:  Eduard Schmidt
 * Date:    01.05.2016
 * 
 * Discription:
 * 
 * Based on the Project: Fritzmeter www.reischle.net
 * 
 * This Project is used to read the overall Send and Receaved Bytes from an AVM Fr!tzBox. 
 * The Post Request through the Linux Co-Processor takes some time so you will have a new Value after 
 * 2 or more seconds
 * To smoothen out the Voltmeter Needle the software feeds the Analogoutput in smal Steps every 
 * 10ms.
 * 
 * 
 * 
 * ------------------------------------
 * Hardware:
 * 
 * µC Board: Arduino Yun
 * External Hardware: Pin 9:  =>  Analog Voltmeters 0-5V // Showing the Upload Value 
 * External Hardware: Pin 10: =>  Analog Voltmeters 0-5V // Showing the Download Value
 * 
 * 
 * Router: FRITZ!Box 6590 Cable 
 * Firmware: FRITZ!OS 07.02
 *
 */

// -----------------------------------
// Library's
// -----------------------------------
//#include <YunClient.h>
#include <Bridge.h>
#include <Process.h>
#include <tr064.h>
//#include <Console.h>

// -----------------------------------
// Constants
// -----------------------------------
#define MAX_STRING_LEN  35
#define SWITCHING_TIME 3000
#define DEBUG_OUTPUT 0 // 0 = Serial connection // 1 = Console
#define UPLOAD_ANALOG_PIN 9
#define DOWNLOAD_ANALOG_PIN 10

// -----------------------------------
// Global variables
// -----------------------------------

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };// MAC address for controller.
byte host_ip[] = {  192, 168, 178, 212 };// IP Adresse des Routers 192.168.178.49
String GatewayIP = "";

// most variables are global for convenience
String nsb; //newSentBytes
String nrb; //nweReceiveBytes
unsigned long nsbl; //bits per second send
unsigned long nrbl; //bits persecond received
unsigned long nsbl_lastCycle; //bits per second send
unsigned long nrbl_lastCycle; //bits persecond received

// Analog Value Helper
byte oldUploadValue = 0;
byte oldDownloadValue = 0;

byte currentDownloadValue = 0;
byte currentUploadValue = 0;

//Helpers Helper
bool rxValid = false;
bool txValid = false;

// Setup vars for serialEvent
char tagStr[MAX_STRING_LEN] = "";
char dataStr[MAX_STRING_LEN] = "";
char tmpStr[MAX_STRING_LEN] = "";
char endTag[3] = {'<', '/', '\0'};
int len;

// Flags to differentiate XML tags from document elements (ie. data)
boolean tagFlag = false;
boolean dataFlag = false;

// Variables for the LCD Display
// unsigned long currentTime;
unsigned long lastCycleTime = 0;

// Initialize the Ethernet client library
// with the IP address and port of the server 
YunClient client;

// Debug variables
byte debugLevel = 0;

// -----------------------------------
// Setup of the Project
// -----------------------------------
void setup(){


  // -------------------------------------
  // Start the Bridge between the Ardoino and the Linux Processor
  Bridge.begin();

  // -------------------------------------
  // Set the Debuglevel
  bitSet(debugLevel, 0); // Connection status to the router
  bitSet(debugLevel, 1); // Filtered fritzbox TX and RX data 
  bitSet(debugLevel, 2); // Raw fritzbox data
  // bitSet(debugLevel, 3);
  // bitSet(debugLevel, 4);  
  // bitSet(debugLevel, 5);
  // bitSet(debugLevel, 6);  
  bitSet(debugLevel, 7); // Debug function msg. To check if the debug thing is working
  // Start the debug output
  DebugOutput(DEBUG_OUTPUT);
  

  // -------------------------------------
  // Define the two Analog Outputs
  pinMode(UPLOAD_ANALOG_PIN, OUTPUT); 
  pinMode(DOWNLOAD_ANALOG_PIN, OUTPUT);

  // -------------------------------------
  // Set the Client TimeOut
  bool _gateWayIpRecived = false;
  client.setTimeout(200);

/*
  while (!_gateWayIpRecived){
    _gateWayIpRecived = setRouterIp();

    if (!_gateWayIpRecived){
      println("No gateway recived...", 0);
      delay(3000);
    }
  }*/
 
  // Get the Current Value and set the buffer Value
  nrbl_lastCycle = nrbl;
  nsbl_lastCycle = nsbl;
  lastCycleTime = millis();
  
  
}


// -----------------------------------
// Main Loop here is the place where the magic happens...
// -----------------------------------
void loop(){
     
  // Check if we need to change the Display-Information  
  if (((lastCycleTime + SWITCHING_TIME) < millis()) || ((lastCycleTime) > millis())){

    
    // Call the xml Processor to obtain currently used bandwidth
    txValid = netread(1); // Get the send Bytes  
    rxValid = netread(2); // Get the recived Bytes 

    // Get the Current Time and calculate the Time Difference from the last measurement
    unsigned long timeSpan = millis() - lastCycleTime;    

    // Info with the Current TX/RX Return from the Fr!tzBox

    //INFO over Serial Send Speed
    println("----------------------------------------------", 1);
    println("Unfiltered Data", 1);
    print("Fr!tzBox Total TX bits/s:", 1);
    println((String)nsbl, 1);
  
    //INFO over Serial Receive Speed
    print("Fr!tzBox Total RX bits/s:", 1);
    println((String)nrbl, 1);

    //debugOutput.println("----------------------------------------------", 1);
    println("Aktueller Tick", 1);
    print("millis()", 1);
    println((String)millis(), 1);

    //--------------------
    // Set the Downstream Rate
    if (rxValid && (nrbl_lastCycle>0)){

      // Set the Analog Output: with 0-60 [mB/s]
      // analogWrite values from 0 to 255
      currentDownloadValue = byte(rxRate(timeSpan, 2) * 255.0/60.0);
    }else{
      // Set the Analog Value to 0
      currentDownloadValue = 0;
    }


    //--------------------
    // Set the Upstream Rate
    if(txValid && (nsbl_lastCycle>0)){

      // Set the Analog Output: With 0-6 [mB/s]
      // analogWrite values from 0 to 255
      // txRate(timeSpan, 2)
      currentUploadValue = byte(txRate(timeSpan, 2) * 255.0/6.0);
    }else{
      // Set the Analog Value to 0
      currentUploadValue = 0;
    }

    switchAnalogValues();

    //--------------------
    // Safe the Data for the next Cycle
    lastCycleTime = millis();
    nrbl_lastCycle = nrbl;
    nsbl_lastCycle = nsbl;
     
  }
}


// -----------------------------------
// Control the Analog Outputs
// -----------------------------------
void switchAnalogValues(){

  byte tempDown = oldDownloadValue;
  byte tempUp = oldUploadValue;

  
  while(  (tempUp != currentUploadValue) || (tempDown != currentDownloadValue) ){


    if(tempUp != currentUploadValue){
      if(tempUp < currentUploadValue){
        tempUp++;
      }
      else{
        tempUp--;
      }
    }

    if(tempDown != currentDownloadValue){
      if(tempDown < currentDownloadValue){
        tempDown++;
      }
      else{
        tempDown--;
      }
    }
    analogWrite(DOWNLOAD_ANALOG_PIN, tempDown);
    analogWrite(UPLOAD_ANALOG_PIN, tempUp);

    delay(10);
  }
  


  // Safe the last Values
  oldUploadValue = currentUploadValue;
  oldDownloadValue = currentDownloadValue;
  
}


// -----------------------------------
// Calculation of the Transfer Rate wit the given 
// base = 0 [b/s]
// base = 1 [Kb/s]
// base = 2 [Mb/s]
// -----------------------------------
float rxRate(unsigned long timeSpan, int base){

  // Calculate the diference from the last and the new Toal Recieved Bits
  float recivedDataAsFloat = nrbl-nrbl_lastCycle;

  // Info with the values from the Calculation
  //debugOutput.println("----------------------------------------------", 200);
  //debugOutput.println("Recived Data", 200);
  //debugOutput.print("Data in the Timespan [b]: ", 200);
  //debugOutput.println((String)recivedDataAsFloat, 200);
    
  return rate(recivedDataAsFloat, timeSpan, base);
  
}

// -----------------------------------
// Calculation of the Transfer Rate wit the given 
// base = 0 [b/s]
// base = 1 [Kb/s]
// base = 2 [Mb/s]
// -----------------------------------
float txRate(unsigned long timeSpan, int base){

  // Calculate the diference from the last and the new Toal Recieved Bits
  float sendDataAsFloat = nsbl-nsbl_lastCycle;

  // Info with the values from the Calculation
  //debugOutput.println("----------------------------------------------", 200);
  //debugOutput.println("Send Data", 200);
  //debugOutput.print("Data in the Timespan [b]: ", 200);
  //debugOutput.println((String)sendDataAsFloat, 200);
    
  return rate(sendDataAsFloat, timeSpan, base);
  
}

// -----------------------------------
// Calculation of the Transfer Rate wit the given 
// base = 0 [b/s]
// base = 1 [Kb/s]
// base = 2 [Mb/s]
// -----------------------------------
float rate(float dataBytesAsFloat, unsigned long timeSpan, int base){

  // Convert some Data to an Float
  float timeSpanAsFloat = timeSpan;


  // Calculate Bits per Second
  //    [Bits/second] =  [Byte] *    [Bits/Bytes]  * [ms]   *  [s/ms] 
  float dataPerSecond = (dataBytesAsFloat*1)/((timeSpanAsFloat/1000));

  // Nominate the Data
  float nominatedDataPerSecond = 0;

  switch (base) {
    case 0:
      nominatedDataPerSecond = dataPerSecond;
      break;
    case 1:
      nominatedDataPerSecond = dataPerSecond/1000;
      break;
    case 2:
      nominatedDataPerSecond = dataPerSecond/1000000;
      break;
    default: 
      nominatedDataPerSecond = dataPerSecond/1000000;
    break;
  }


  //debugOutput.print("Timespan in [ms]: ", 210);
  //debugOutput.println((String)timeSpanAsFloat, 210);  

  //debugOutput.print("Data [b/s]: ", 210);
  //debugOutput.println((String)dataPerSecond, 210);

  //debugOutput.print("Nominated Data [Mb/s]: ", 210);
  //debugOutput.println((String)nominatedDataPerSecond, 210);
    

  // Return the calculated Value
  return nominatedDataPerSecond;
  
}


// -----------------------------------
// Serial Event read individual chars from the ethernet session and assemble xml tags  
// -----------------------------------
void serialEvent() {

  // Read a char
  char inChar = client.read();
 
  if (inChar == '<') {
    addChar(inChar, tmpStr);
    tagFlag = true;
    dataFlag = false;

  } else if (inChar == '>') {
    addChar(inChar, tmpStr);

    if (tagFlag) {      
      strncpy(tagStr, tmpStr, strlen(tmpStr)+1);
    }

    // Clear tmp
    clearStr(tmpStr);

    tagFlag = false;
    dataFlag = true;      
      
    } else if (inChar != 10) {
      if (tagFlag) {
        // Add tag char to string
        addChar(inChar, tmpStr);

        // Check for </XML> end tag, ignore it
        if ( tagFlag && strcmp(tmpStr, endTag) == 0 ) {
          clearStr(tmpStr);
          tagFlag = false;
          dataFlag = false;
        }
      }
      
      if (dataFlag) {
        // Add data char to string
        addChar(inChar, dataStr);
      }
    }  
  
    // If a LF, process the line
    if (inChar == 10 ) {

      print("tagStr: ", 2);
      println(tagStr, 2);
      print("dataStr: ", 2);
      println(dataStr, 2);

      // Find specific tags and print data

      if (matchTag("<NewTotalBytesSent>")) {
        print("TX: ", 2);
        print(dataStr, 2);
        nsbl = strtoul(dataStr,NULL,0);
        nsbl=nsbl*8; //convert bytes to bits
      }
    
      if (matchTag("<NewTotalBytesReceived>")) {
        print("RX: ", 2);
        print(dataStr, 2);
        nrbl = strtoul(dataStr,NULL,0);
        nrbl=nrbl*8;  //mach bits
      }
      
   
      // Clear all strings
      clearStr(tmpStr);
      clearStr(tagStr);
      clearStr(dataStr);

      // Clear Flags
      tagFlag = false;
      dataFlag = false;
   }
}


// -----------------------------------
// Function to clear a string
// -----------------------------------
void clearStr (char* str) {
   int len = strlen(str);
   for (int c = 0; c < len; c++) {
      str[c] = 0;
   }
}


// -----------------------------------
//Function to add a char to a string and check its length
// -----------------------------------
void addChar (char ch, char* str) {
   char *tagMsg  = "<TRUNCATED_TAG>";
   char *dataMsg = "-TRUNCATED_DATA-";

   // Check the max size of the string to make sure it doesn't grow too
   // big.  If string is beyond MAX_STRING_LEN assume it is unimportant
   // and replace it with a warning message.
   if (strlen(str) > MAX_STRING_LEN - 2) {
      if (tagFlag) {
         clearStr(tagStr);
         strcpy(tagStr,tagMsg);
      }
      if (dataFlag) {
         clearStr(dataStr);
         strcpy(dataStr,dataMsg);
      }

      // Clear the temp buffer and flags to stop current processing
      clearStr(tmpStr);
      tagFlag = false;
      dataFlag = false;

   } else {
      // Add char to string
      str[strlen(str)] = ch;
   }
}


// -----------------------------------
// Function to check the current tag for a specific string
// -----------------------------------
boolean matchTag (char* searchTag) {
   if ( strcmp(tagStr, searchTag) == 0 ) {
      return true;
   } else {
      return false;
   }
}


// -----------------------------------
// Do the network reading stuff
// Debug Mode 5
// -----------------------------------
bool netread(int fselector){

  String _gatewayIP = "";

  println("---------------------------------", 0);

  
  //----------------------------------------------------------
  // make the request to the server 
  if (client.connect(host_ip, 49000)) {

      println("SUCCESS: Connection to the Server established", 0);
      print("Start sending Post Request...", 0);

      _gatewayIP = String(host_ip[0]) + ":" + String(host_ip[1]) + ":" + String(host_ip[2]) + ":" + String(host_ip[3]);
      println("Router ip as string for XML Post request: " + _gatewayIP , 0);

      _gatewayIP = "192:168:178:1";

      client.println("POST /igdupnp/control/WANCommonIFC1 HTTP/1.1\nHOST: " + _gatewayIP + ":49000");
    
    if(fselector == 1){
      client.println("SOAPACTION: \"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1#GetTotalBytesSent\"");
      client.println("Content-Type: text/xml; charset=\"utf-8\"");
      client.println("Content-Length: 287");
      client.print(F("<?xml version=\"1.0\" encodin=\"utf-8\"?>\n<s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" "));
      client.print(F("xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\">\n<s:Body>\n<u:GetTotalBytesSent xmlns:u=\"urn:schemas-upnp-org:service:"));
      client.println(F("WANCommonInterfaceConfig:1\" />\n</s:Body>\n</s:Envelope>"));
    }


    if(fselector == 2){
      client.println("SOAPACTION: \"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1#GetTotalBytesReceived\"\nContent-Type: text/xml; charset=\"utf-8\"\nContent-Length: 289");
      client.print(F("<?xml version=\"1.0\" encodin=\"utf-8\"?>\n<s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" "));
      client.print(F("xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\">\n<s:Body>\n<u:GetTotalBytesReceived xmlns:u=\"urn:schemas-upnp-org:service:"));
      client.println(F("WANCommonInterfaceConfig:1\" />\n</s:Body>\n</s:Envelope>"));
    }


    println("Post request sent", 0);
    

  } 
  else {
    // no connection to the server
    println("FAILED: No Connection to the Server", 0);
    return false;
  }

  //---------------------------------------------------------- 
  //wait for the server to answer
  while (!client.available()){
    println("Waiting for server...", 0);
    ;
  }
  
  //----------------------------------------------------------
  //get data
  while (client.available()){

    println("Start getting Post...", 0);
    serialEvent();
    println("Post Recived", 0);

  }



  //----------------------------------------------------------
  // Make a clean Cut with the Client disconnect and Init..
  client.flush();

  // INFO
  println("", 0);
  print("Disconnecting...", 0);        
  

  client.stop();
  
  println("done.", 0);        
  
  client.flush();

  return true;
    
}


// -----------------------------------
// Get the gateway IP and set the gloabal variable
// -----------------------------------
bool setRouterIp(){

  // Some Variables we need
  Process _p;// Process Object
  int _readValueInt = 0;
  char _readValueChar;
  String _gatewayIp = "";

  int _ipSegment = 0;
  bool _retVal = false;
    
  // This command line runs the WifiStatus script, (/usr/bin/pretty-wifi-info.lua), then
  // sends the result to the grep command to look for a line containing the word
  // "Signal:"  the result is passed to this sketch:
  _p.runShellCommand("route -n | grep '^0\\.0\\.\\0\\.0[ \\t]\\+[1-9][0-9]*\\.[1-9][0-9]*\\.[1-9][0-9]*\\.[1-9][0-9]*[ \\t]\\+0\\.0\\.0\\.0[ \\t]\\+[^ \\t]*G[^ \\t]*[ \\t]' | awk '{print $2}'");
  _p.runShellCommand("route -n | grep 'UG[ \\t]' | awk '{print $2}'");

  // do nothing until the process finishes, so you get the whole output:
  while (_p.running());

  // Read command output. runShellCommand() should have passed the IP of the Gateway "xxx.xxx.xxx.xxx":
  while (_p.available()) {

    _readValueInt = _p.read();
    _readValueChar = char(_readValueInt);

    if(_readValueInt == _readValueChar){
      if (_readValueInt == 46) { // Check for the "."
         host_ip[_ipSegment] = (byte)_gatewayIp.toInt();// Convert the ip segment to byte
         _ipSegment++;// Increment the ip segment
         _gatewayIp = "";// We have recived data so reset the streing for the next segment
         _retVal = true;// We have recived some data so cross the fingers that the data is correct. and set the return value to true
      }
      else{
        _gatewayIp += _readValueChar; //Get the Data From the Terminal (The IP Adress of the Gateway (Our Fr!zbox))        
      }
    }
  }

  // Add the last Segment
  host_ip[_ipSegment] = (byte)_gatewayIp.toInt();

  println("---------------------------------", 0);
  println("Router ip found: " + String(host_ip[0]) + ":" + String(host_ip[1]) + ":" + String(host_ip[2]) + ":" + String(host_ip[3]) , 0);


  return _retVal;
  
}



// -----------------------------------
// Debug functions
// -----------------------------------
// Init the debug interface
void DebugOutput(byte _outputSelect) {

  switch (_outputSelect) {

    // Serial connection on the USB
    case 0:
      Serial.begin(9600);
      Serial.println("Serial connection started.");
      break;

    // Console connection for the debug seasion via network on the arduino yun
    case 1:
      Bridge.begin();
      Console.begin();
      while(!Console);  
      Console.println("Console connection started.");
      break;

    // Defaulkt fallback  
    default:
      // Do nothing. no correct selection was made
      break;
  }
}

// print line
void println(String _outputData , byte _debugLevel){

  if(!(  (( bitRead(debugLevel, 0) == 1 ) && ( _debugLevel == 0 ))
      || (( bitRead(debugLevel, 1) == 1 ) && ( _debugLevel == 1 ))
      || (( bitRead(debugLevel, 2) == 1 ) && ( _debugLevel == 2 ))
      || (( bitRead(debugLevel, 3) == 1 ) && ( _debugLevel == 3 ))
      || (( bitRead(debugLevel, 4) == 1 ) && ( _debugLevel == 4 ))
      || (( bitRead(debugLevel, 5) == 1 ) && ( _debugLevel == 5 ))
      || (( bitRead(debugLevel, 6) == 1 ) && ( _debugLevel == 6 ))
      || (( bitRead(debugLevel, 7) == 1 ) && ( _debugLevel == 7 )) )){
    return;
  }
  
  // put your main code here, to run repeatedly:
  if(DEBUG_OUTPUT == 0){
    Serial.println(_outputData);  
  }
  else{
    Console.println(_outputData);
  }
}

// print
void print(String _outputData , byte _debugLevel){

  if(!(  (( bitRead(debugLevel, 0) == 1 ) && ( _debugLevel == 0 ))
      || (( bitRead(debugLevel, 1) == 1 ) && ( _debugLevel == 1 ))
      || (( bitRead(debugLevel, 2) == 1 ) && ( _debugLevel == 2 ))
      || (( bitRead(debugLevel, 3) == 1 ) && ( _debugLevel == 3 ))
      || (( bitRead(debugLevel, 4) == 1 ) && ( _debugLevel == 4 ))
      || (( bitRead(debugLevel, 5) == 1 ) && ( _debugLevel == 5 ))
      || (( bitRead(debugLevel, 6) == 1 ) && ( _debugLevel == 6 ))
      || (( bitRead(debugLevel, 7) == 1 ) && ( _debugLevel == 7 )) )){
    return;
  }

  // put your main code here, to run repeatedly:
  if(DEBUG_OUTPUT == 0){
    Serial.print(_outputData);  
  }
  else{
    Console.print(_outputData);
  }
}
 
