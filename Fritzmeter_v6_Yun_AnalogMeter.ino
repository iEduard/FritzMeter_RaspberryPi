/* Fritzmeter with Arduino Yun
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
 * Router: FRITZ!Box 6340 Cable (kbw)
 * Firmware: FRITZ!OS 06.04
 *
 */

// -----------------------------------
// Library's
// -----------------------------------
#include <YunClient.h>
#include <Bridge.h>
#include <Process.h>
//#include <DebugOutput.h>
//#include <Console.h>

// -----------------------------------
// Constants
// -----------------------------------
#define MAX_STRING_LEN  35
#define SWITCHING_TIME 3000
//#define DEBUG_LEVEL 2   // 5 = netread() // 6 = serialevent() // 2 = 
#define UPLOAD_ANALOG_PIN 9
#define DOWNLOAD_ANALOG_PIN 10

// -----------------------------------
// Global Variables
// -----------------------------------

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };// MAC address for controller.
byte host_ip[] = {  192, 168, 0, 1 };// IP Adresse des Routers
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

// Debug Output Object
//DebugOutput debugOutput;


// -----------------------------------
// Setup of the Project
// -----------------------------------
void setup(){


  // -------------------------------------
  // Start the Bridge between the Ardoino and the Linux Processor
  Bridge.begin();

  // -------------------------------------
  // start the Debug library:
  // debugOutput.begin(DEBUG_CONSOLE,1);    // DEBUG_SERIAL DEBUG_CONSOLE
  

  // -------------------------------------
  // Define the two Analog Outputs
  pinMode(UPLOAD_ANALOG_PIN, OUTPUT); 
  pinMode(DOWNLOAD_ANALOG_PIN, OUTPUT);

  // -------------------------------------
  // Set the Client TimeOut
  client.setTimeout(200);


  while (GatewayIP==""){
    GatewayIP = getGateway();
    //debugOutput.println("No Gateway recived...", 200);
    delay(3000);
  }

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
  if (((lastCycleTime + SWITCHING_TIME) < millis()) ||  ((lastCycleTime) > millis())){

    
    // Call the xml Processor to obtain currently used bandwidth
    txValid = netread(1); // Get the send Bytes  
    rxValid = netread(2); // Get the recived Bytes 

    // Get the Current Time and calculate the Time Difference from the last measurement
    unsigned long timeSpan = millis() - lastCycleTime;    

    // Info with the Current TX/RX Return from the Fr!tzBox

    //INFO over Serial Send Speed
    //debugOutput.println("----------------------------------------------", 5);
    //debugOutput.println("Unfiltered Data", 5);
    //debugOutput.print("Fr!tzBox Total TX bits/s:", 5);
    //debugOutput.println((String)nsbl, 5);
  
    //INFO over Serial Receive Speed
    //debugOutput.print("Fr!tzBox Total RX bits/s:", 5);
    //debugOutput.println((String)nrbl, 5);

    //debugOutput.println("----------------------------------------------", 5);
    //debugOutput.println("Aktueller Tick", 5);
    //debugOutput.print("millis()", 5);
    //debugOutput.println((String)millis(), 5);

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

      //debugOutput.print("tagStr: ", 220);
      //debugOutput.println(tagStr, 220);
      //debugOutput.print("dataStr: ", 220);
      //debugOutput.println(dataStr, 220);

      // Find specific tags and print data

      if (matchTag("<NewTotalBytesSent>")) {
        //debugOutput.print("TX: ", 220);
        //debugOutput.print(dataStr, 220);
        nsbl = strtoul(dataStr,NULL,0);
        nsbl=nsbl*8; //convert bytes to bits
      }
    
      if (matchTag("<NewTotalBytesReceived>")) {
        //debugOutput.print("RX: ", 220);
        //debugOutput.print(dataStr, 220);
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

  //----------------------------------------------------------
  // make the request to the server 
  if (client.connect(host_ip, 49000)) {


      //debugOutput.println("---------------------------------", 230);
      //debugOutput.println("SUCCESS: Connection to the Server established", 230);        
      //debugOutput.print("Start sending Post Request...", 230);        
      

      client.println("POST /igdupnp/control/WANCommonIFC1 HTTP/1.1\nHOST: " + GatewayIP + ":49000");
    
    if(fselector == 1){
      client.println("SOAPACTION: \"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1#GetTotalBytesSent\"\nContent-Type: text/xml; charset=\"utf-8\"\nContent-Length: 285\n");
      client.print(F("<?xml version=\"1.0\" encodin=\"utf-8\"?>\n<s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" "));
      client.print(F("xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\">\n<s:Body>\n<u:GetTotalBytesSent xmlns:u=\"urn:schemas-upnp-org:service:"));
      client.println(F("WANCommonInterfaceConfig:1\" />\n</s:Body>\n</s:Envelope>\n"));
    }


    if(fselector == 2){
      client.println("SOAPACTION: \"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1#GetTotalBytesReceived\"\nContent-Type: text/xml; charset=\"utf-8\"\nContent-Length: 289\n");
      client.print(F("<?xml version=\"1.0\" encodin=\"utf-8\"?>\n<s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" "));
      client.print(F("xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\">\n<s:Body>\n<u:GetTotalBytesReceived xmlns:u=\"urn:schemas-upnp-org:service:"));
      client.println(F("WANCommonInterfaceConfig:1\" />\n</s:Body>\n</s:Envelope>\n"));
    }


    //debugOutput.println("Post request sent", 230);
    

  } 
  else {
    // no connection to the server
    //debugOutput.println("FAILED: No Connection to the Server", 230);        
    return false;
  }

  //---------------------------------------------------------- 
  //wait for the server to answer
  while (!client.available()){
    //debugOutput.println("Waiting for server...", 230);
    ;
  }
  
  //----------------------------------------------------------
  //get data
  while (client.available()){

    //debugOutput.println("Start getting Post...", 230);
    serialEvent();
    //debugOutput.println("Post Recived", 230);

  }



  //----------------------------------------------------------
  // Make a clean Cut with the Client disconnect and Init..
  client.flush();

  // INFO
  //debugOutput.println("", 230);
  //debugOutput.print("Disconnecting...", 230);        
  

  client.stop();
  
  //debugOutput.println("done.", 230);        
  
  client.flush();

  return true;
    
}


// -----------------------------------
// Get the Gateway IP
// -----------------------------------
String getGateway(){

  // Some Variables we need
  Process p;// Process Object
  int readValueInt = 0;
  char readValueChar;
  String gatewayIp = "";
  
  // This command line runs the WifiStatus script, (/usr/bin/pretty-wifi-info.lua), then
  // sends the result to the grep command to look for a line containing the word
  // "Signal:"  the result is passed to this sketch:
  p.runShellCommand("route -n | grep '^0\\.0\\.\\0\\.0[ \\t]\\+[1-9][0-9]*\\.[1-9][0-9]*\\.[1-9][0-9]*\\.[1-9][0-9]*[ \\t]\\+0\\.0\\.0\\.0[ \\t]\\+[^ \\t]*G[^ \\t]*[ \\t]' | awk '{print $2}'");
  p.runShellCommand("route -n | grep 'UG[ \\t]' | awk '{print $2}'");

  // do nothing until the process finishes, so you get the whole output:
  while (p.running());

  // Read command output. runShellCommand() should have passed the IP of the Gateway "xxx.xxx.xxx.xxx":
  while (p.available()) {

    readValueInt = p.read();
    readValueChar = char(readValueInt);

    if(readValueInt == readValueChar){
        gatewayIp += readValueChar; //Get the Data From the Terminal (The IP Adress of the Gateway (Our Fr!zbox))
    }
  }

  // Return the Gateway IP Adress
  return gatewayIp;
}

 
