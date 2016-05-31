/*
 * Copyright (c) 2015 Chris Howell
 *
 * This file is part of Open EVSE.
 * Open EVSE is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * Open EVSE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Open EVSE; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
 
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "FS.h"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);

//Default SSID and PASSWORD for AP Access Point Mode
const char* ssid = "OpenEVSE";
const char* password = "openevse";
const char* www_username = "admin";
const char* www_password = "openevse";
String st;

String esid = "";
String epass = "";  
String apikey = "";
String node = "";

String connected_network = "";
String last_datastr = "";
String status_string = "";
String ipaddress = "";

//SERVER strings and interfers for OpenEVSE Energy Monotoring
const char* host = "data.openevse.com";
const int httpsPort = 443;
const char* e_url = "/emoncms/input/post.json?node=";
const char* inputID_AMP   = "OpenEVSE_AMP:";  //OpenEVSE Current Sensor
const char* inputID_VOLT   = "OpenEVSE_VOLT:"; //Not currently in used
const char* inputID_TEMP1   = "OpenEVSE_TEMP1:"; //Sensor DS3232 Ambient
const char* inputID_TEMP2   = "OpenEVSE_TEMP2:"; //Sensor MCP9808 Ambient
const char* inputID_TEMP3   = "OpenEVSE_TEMP3:"; //Sensor TMP007 Infared
const char* inputID_PILOT   = "OpenEVSE_PILOT:"; //OpenEVSE Pilot Setting

int amp = 0; //OpenEVSE Current Sensor
int volt = 0; //Not currently in used
int temp1 = 0; //Sensor DS3232 Ambient
int temp2 = 0; //Sensor MCP9808 Ambient
int temp3 = 0; //Sensor TMP007 Infared
int pilot = 0; //OpenEVSE Pilot Setting

//Defaults OpenEVSE Settings
int rgb_lcd = 1;
int serial_dbg = 0;
int auto_service = 1;
int service = 1;
int current_l1 = 0;
int current_l2 = 0;
int current_l1min = 6;
int current_l2min = 6;
int current_l1max = 20;
int current_l2max = 80;
int current_scale = 0;
int current_offset = 0;

//Default OpenEVSE Safety Configuration
String diode_ck = "1";
String gfci_test = "1";
String ground_ck = "1";
String stuck_relay = "1";
String vent_ck = "1";
String temp_ck = "1";
String auto_start = "1";

String firmware = "1.00.100";
String protocol = "2.0.2b";

//Default OpenEVSE Fault Counters
int gfci_count = 0;
int nognd_count = 0;
int stuck_count = 0;

//OpenEVSE RAPI Flags
String flags = "";
int flags1 = 0;
int flags2 = 0;
int flags3 = 0;

//OpenEVSE Session options
int kwh_limit = 0;
int time_limit = 0;

//OpenEVSE Usage Statistics
unsigned long wattsec = 0;
unsigned long watthour_total = 0;

// Wifi mode
// 0 - STA (Client)
// 1 - AP with STA retry
// 2 - AP only
// 3 - AP + STA
int wifi_mode = 0; 
int buttonState = 0;
int clientTimeout = 0;
int i = 0;
unsigned long Timer;
unsigned long packets_sent = 0;
unsigned long packets_success = 0;
unsigned long comm_sent = 0;
unsigned long comm_success = 0;

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}
// -------------------------------------------------------------------
// Start Access Point, starts on 192.168.4.1
// Access point is used for wifi network selection
// -------------------------------------------------------------------
void startAP() {
  //Serial.print("Starting Access Point");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  //Serial.print("Scan: ");
  int n = WiFi.scanNetworks();
  //Serial.print(n);
  //Serial.println(" networks found");
  st = "";
  for (int i = 0; i < n; ++i){
    st += "\""+WiFi.SSID(i)+"\"";
    if (i<n-1) st += ",";
  }
  delay(100);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  char tmpStr[40];
  Serial.println("$FP 0 0 SSID...OpenEVSE.");
  delay(100);
  Serial.println("$FP 0 1 PASS...openevse.");
  delay(5000);
  Serial.println("$FP 0 0 IP_Address......");
  delay(100);
  sprintf(tmpStr,"%d.%d.%d.%d",myIP[0],myIP[1],myIP[2],myIP[3]);
  //Serial.print("Access Point IP Address: ");
  Serial.print("$FP 0 1 ");
  Serial.println(tmpStr);
  ipaddress = tmpStr;
}

// -------------------------------------------------------------------
// Start Client, attempt to connect to Wifi network
// -------------------------------------------------------------------
void startClient() {
  //Serial.print("Connecting as Wifi Client to ");
  //Serial.print(esid.c_str());
  //Serial.print(" epass:");
  //Serial.println(epass.c_str());
  WiFi.begin(esid.c_str(), epass.c_str());
  
  delay(50);
  
  int t = 0;
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED){
    
    delay(500);
    t++;
    if (t >= 20){
      //Serial.println(" ");
      //Serial.println("Trying Again...");
      delay(2000);
      WiFi.disconnect();
      WiFi.begin(esid.c_str(), epass.c_str());
      t = 0;
      attempt++;
      if (attempt >= 5){
        startAP();
        // AP mode with SSID in EEPROM, connection will retry in 5 minutes
        wifi_mode = 1;
        break;
      }
    }
  }
  
  if (wifi_mode == 0 || wifi_mode == 3){
    IPAddress myAddress = WiFi.localIP();
    char tmpStr[40];
    sprintf(tmpStr,"%d.%d.%d.%d",myAddress[0],myAddress[1],myAddress[2],myAddress[3]);
    //Serial.print("Connected, IP Address: ");
    Serial.println("$FP 0 0 Client-IP.......");
    delay(100);
    Serial.print("$FP 0 1 ");
    Serial.println(tmpStr);
    
    // Copy the connected network and ipaddress to global strings for use in status request
    connected_network = esid;
    ipaddress = tmpStr;
  }
}

void ResetEEPROM(){
  //Serial.println("Erasing EEPROM");
  for (int i = 0; i < 512; ++i) { 
    EEPROM.write(i, 0);
    //Serial.print("#"); 
  }
  EEPROM.commit();   
}

// -------------------------------------------------------------------
// Load SPIFFS Home page
// url: /
// -------------------------------------------------------------------
void handleHome() {
  String s;
  SPIFFS.begin(); // mount the fs
  File f = SPIFFS.open("/home.html", "r");
  if (f) {
    String s = f.readString();
    server.send(200, "text/html", s);
    f.close();
  }
}

// -------------------------------------------------------------------
// Handle turning Access point off
// url: /apoff
// -------------------------------------------------------------------
void handleAPOff() {
  server.send(200, "text/html", "Turning Access Point Off");
  Serial.println("Turning Access Point Off");
  delay(2000);
  WiFi.mode(WIFI_STA); 
}

// -------------------------------------------------------------------
// Save selected network to EEPROM and attempt connection
// url: /savenetwork
// -------------------------------------------------------------------
void handleSaveNetwork() {
  String s;
  String qsid = server.arg("ssid");
  String qpass = server.arg("pass");      
  esid = qsid;
  epass = qpass;
  
  qpass.replace("%21", "!");
//  qpass.replace("%22", '"');
  qpass.replace("%23", "#");
  qpass.replace("%24", "$");
  qpass.replace("%25", "%");
  qpass.replace("%26", "&");
  qpass.replace("%27", "'");
  qpass.replace("%28", "(");
  qpass.replace("%29", ")");
  qpass.replace("%2A", "*");
  qpass.replace("%2B", "+");
  qpass.replace("%2C", ",");
  qpass.replace("%2D", "-");
  qpass.replace("%2E", ".");
  qpass.replace("%2F", "/");
  qpass.replace("%3A", ":");
  qpass.replace("%3B", ";");
  qpass.replace("%3C", "<");
  qpass.replace("%3D", "=");
  qpass.replace("%3E", ">");
  qpass.replace("%3F", "?");
  qpass.replace("%40", "@");
  qpass.replace("%5B", "[");
  qpass.replace("%5C", "'\'");
  qpass.replace("%5D", "]");
  qpass.replace("%5E", "^");
  qpass.replace("%5F", "_");
  qpass.replace("%60", "`");
  qpass.replace("%7B", "{");
  qpass.replace("%7C", "|");
  qpass.replace("%7D", "}");
  qpass.replace("%7E", "~");
  qpass.replace('+', ' ');

  qsid.replace("%21", "!");
//  qsid.replace("%22", '"');
  qsid.replace("%23", "#");
  qsid.replace("%24", "$");
  qsid.replace("%25", "%");
  qsid.replace("%26", "&");
  qsid.replace("%27", "'");
  qsid.replace("%28", "(");
  qsid.replace("%29", ")");
  qsid.replace("%2A", "*");
  qsid.replace("%2B", "+");
  qsid.replace("%2C", ",");
  qsid.replace("%2D", "-");
  qsid.replace("%2E", ".");
  qsid.replace("%2F", "/");
  qsid.replace("%3A", ":");
  qsid.replace("%3B", ";");
  qsid.replace("%3C", "<");
  qsid.replace("%3D", "=");
  qsid.replace("%3E", ">");
  qsid.replace("%3F", "?");
  qsid.replace("%40", "@");
  qsid.replace("%5B", "[");
  qsid.replace("%5C", "'\'");
  qsid.replace("%5D", "]");
  qsid.replace("%5E", "^");
  qsid.replace("%5F", "_");
  qsid.replace("%60", "`");
  qsid.replace("%7B", "{");
  qsid.replace("%7C", "|");
  qsid.replace("%7D", "}");
  qsid.replace("%7E", "~");
  qsid.replace('+', ' ');
  
  if (qsid != 0){
    for (int i = 0; i < 32; i++){
      if (i<qsid.length()) {
        EEPROM.write(i+0, qsid[i]);
      } else {
        EEPROM.write(i+0, 0);
      }
    }
    
    for (int i = 0; i < 32; i++){
      if (i<qpass.length()) {
        EEPROM.write(i+32, qpass[i]);
      } else {
        EEPROM.write(i+32, 0);
      }
    }
    
    
    EEPROM.commit();
    server.send(200, "text/html", "Saved");
    delay(2000);
    
    // Startup in STA + AP mode
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssid, password);
    wifi_mode = 3;
    startClient();
  }
}

// -------------------------------------------------------------------
// Save apikey
// url: /saveapikey
// -------------------------------------------------------------------
void handleSaveApikey() {
  apikey = server.arg("apikey");
  node = server.arg("node");
  if (apikey!=0) {
    EEPROM.write(129, node[i]);
    for (int i = 0; i < 32; i++){
      if (i<apikey.length()) {
        EEPROM.write(i+96, apikey[i]);
      } else {
        EEPROM.write(i+96, 0);
        EEPROM.write(129, 0);
      }      
    }
    EEPROM.commit();
    server.send(200, "text/html", "Saved");
  }
}

// -------------------------------------------------------------------
// Wifi scan /scan not currently used
// url: /scan
// -------------------------------------------------------------------
void handleScan() {
  //Serial.println("WIFI Scan");
  int n = WiFi.scanNetworks();
  //Serial.print(n);
  //Serial.println(" networks found");
  st = "";
  for (int i = 0; i < n; ++i){
    st += "\""+WiFi.SSID(i)+"\"";
    if (i<n-1) st += ",";
  }
  server.send(200, "text/plain","["+st+"]");
}

// -------------------------------------------------------------------
// url: /lastvalues
// Last values on atmega serial
// -------------------------------------------------------------------
void handleLastValues() {
  server.send(200, "text/html", last_datastr);
}

// -------------------------------------------------------------------
// url: /status
// returns wifi status
// -------------------------------------------------------------------
void handleStatus() {

  String s = "{";
  if (wifi_mode==0) {
    s += "\"mode\":\"STA\",";
  } else if (wifi_mode==1 || wifi_mode==2) {
    s += "\"mode\":\"AP\",";
  } else if (wifi_mode==3) {
    s += "\"mode\":\"STA+AP\",";
  }
  s += "\"networks\":["+st+"],";
  s += "\"ssid\":\""+esid+"\",";
  s += "\"pass\":\""+epass+"\",";
  s += "\"apikey\":\""+apikey+"\",";
  s += "\"node\":\""+node+"\",";
  s += "\"ipaddress\":\""+ipaddress+"\",";
  s += "\"comm_sent\":\""+String(comm_sent)+"\",";
  s += "\"comm_success\":\""+String(comm_success)+"\",";
  s += "\"packets_sent\":\""+String(packets_sent)+"\",";
  s += "\"packets_success\":\""+String(packets_success)+"\"";
  s += "}";
  server.send(200, "text/html", s);
}

void handleConfig() {

  String s = "{";
  s += "\"firmware\":\""+String(firmware)+"\",";
  s += "\"protocol\":\""+String(protocol)+"\",";
  s += "\"diodet\":\""+String(diode_ck)+"\",";
  s += "\"gfcit\":\""+String(gfci_test)+"\",";
  s += "\"groundt\":\""+String(ground_ck)+"\",";
  s += "\"relayt\":\""+String(stuck_relay)+"\",";
  s += "\"ventt\":\""+String(vent_ck)+"\",";
  s += "\"tempt\":\""+String(temp_ck)+"\"";
  s += "}";
  server.send(200, "text/html", s);
}

// -------------------------------------------------------------------
// Reset config and reboot
// url: /reset
// -------------------------------------------------------------------
void handleRst() {
  ResetEEPROM();
  EEPROM.commit();
  server.send(200, "text/html", "Reset");
  WiFi.disconnect();
  delay(1000);
  ESP.reset();
}

void handleRapi() {
  String s;
  s = "<html><font size='20'><font color=006666>Open</font><b>EVSE</b></font><p><b>Open Source Hardware</b><p>Send RAPI Command<p>Common Commands:<p>Set Current - $SC XX<p>Set Service Level - $SL 1 - $SL 2 - $SL A<p>Get Real-time Current - $GG<p>Get Temperatures - $GP<p>";
        s += "<p>";
        s += "<form method='get' action='r'><label><b><i>RAPI Command:</b></i></label><input name='rapi' length=32><p><input type='submit'></form>";
        s += "</html>\r\n\r\n";
  server.send(200, "text/html", s);
}

void handleRapiR() {
  String s;
  String rapiString;
  String rapi = server.arg("rapi");
  rapi.replace("%24", "$");
  rapi.replace("+", " "); 
  Serial.flush();
  Serial.println(rapi);
  delay(100);
       while(Serial.available()) {
         rapiString = Serial.readStringUntil('\r');
       }    
   s = "<html><font size='20'><font color=006666>Open</font><b>EVSE</b></font><p><b>Open Source Hardware</b><p>RAPI Command Sent<p>Common Commands:<p>Set Current - $SC XX<p>Set Service Level - $SL 1 - $SL 2 - $SL A<p>Get Real-time Current - $GG<p>Get Temperatures - $GP<p>";
   s += "<p>";
   s += "<form method='get' action='r'><label><b><i>RAPI Command:</b></i></label><input name='rapi' length=32><p><input type='submit'></form>";
   s += rapi;
   s += "<p>>";
   s += rapiString;
   s += "<p></html>\r\n\r\n";
   server.send(200, "text/html", s);
}

void handleRapiRead() {
  delay(100);
  Serial.flush(); 
  Serial.println("$GV*C1");
  comm_sent++;
  delay(100);
  while(Serial.available()) {
      String rapiString = Serial.readStringUntil('\r');
      if (rapiString.startsWith("$OK") ) {
        comm_success++;
        int firstRapiCmd = rapiString.indexOf(' ');
        String qrapi; 
        qrapi = rapiString.substring(rapiString.indexOf(' ', firstRapiCmd));
        firmware = qrapi;
        String qrapi1;
        qrapi1 = rapiString.substring(rapiString.indexOf(' ', firstRapiCmd + 1 ));
        protocol = qrapi1;
      }
    }
}

void setup() {
	delay(2000);
	Serial.begin(115200);
  EEPROM.begin(512);
  pinMode(0, INPUT);
  //char tmpStr[40];
  
 
  for (int i = 0; i < 32; ++i){
    char c = char(EEPROM.read(i));
    if (c!=0) esid += c;
  }
  for (int i = 32; i < 96; ++i){
    char c = char(EEPROM.read(i));
    if (c!=0) epass += c;
  }
  for (int i = 96; i < 128; ++i){
    char c = char(EEPROM.read(i));
    if (c!=0) apikey += c;
  }
  node += char(EEPROM.read(129));
     
  WiFi.disconnect();
  // 1) If no network configured start up access point
  if (esid == 0)
  {
    startAP();
    wifi_mode = 2; // AP mode with no SSID in EEPROM    
  } 
  // 2) else try and connect to the configured network
  else
  {
    WiFi.mode(WIFI_STA);
    wifi_mode = 0;
    startClient();
  }
  
 ArduinoOTA.begin();
  server.on("/", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    handleHome();
  });
  server.on("/r", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    handleRapiR();
  });
  server.on("/reset", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    handleRst();
  });
  server.on("/status", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    handleStatus();
  });
  server.on("/config", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    handleConfig();
  });
  server.on("/rapi", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    handleRapi();
  });
  server.on("/savenetwork", handleSaveNetwork);
  server.on("/saveapikey", handleSaveApikey);
  server.on("/lastvalues",handleLastValues);
  server.on("/scan", handleScan);
  server.on("/apoff",handleAPOff);
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });
  
	server.begin();
	//Serial.println("HTTP server started");
  delay(100);
  Timer = millis();

  handleRapiRead();
}

void loop() {
ArduinoOTA.handle();
server.handleClient();
  
int erase = 0;  
buttonState = digitalRead(0);
while (buttonState == LOW) {
  buttonState = digitalRead(0);
  erase++;
  if (erase >= 5000) {
    ResetEEPROM();
    int erase = 0;
    WiFi.disconnect();
    Serial.print("Finished...");
    delay(2000);
    ESP.reset(); 
  } 
}
// Remain in AP mode for 5 Minutes before resetting
if (wifi_mode == 1){
   if ((millis() - Timer) >= 300000){
     ESP.reset();
   }
}   
 
if (wifi_mode == 0 || wifi_mode == 3 && apikey != 0){
   if ((millis() - Timer) >= 30000){
     Timer = millis();
     Serial.flush();
     Serial.println("$GE*B0");
     comm_sent++;
     delay(100);
       while(Serial.available()) {
         String rapiString = Serial.readStringUntil('\r');
         if ( rapiString.startsWith("$OK ") ) {
           comm_success++;
           String qrapi; 
           qrapi = rapiString.substring(rapiString.indexOf(' '));
           pilot = qrapi.toInt();
           last_datastr = "Pilot:";
           last_datastr += pilot;
           String qrapi1;
           qrapi1 = rapiString.substring(rapiString.lastIndexOf(' '));
           flags = qrapi1;
           flags1 = flags[1];
           flags2 = flags[2];
           flags3 = flags[3];
           //Decode Flags
           if (flags1 >= 4){
             temp_ck = "0";
             flags1 = flags1 - 4;
           }
           if (flags1 >= 2){
             gfci_test = "0";
             flags1 = flags1 - 2;
           }
           if (flags1 == 1){
             rgb_lcd = 0;
           }
           if (flags2 >= 8){
             serial_dbg = 1;
             flags2 = flags2 - 8;
           }
           if (flags2 >= 4){
             auto_start = "0";
             flags2 = flags2 - 4;
           }
           if (flags2 >= 2){
             auto_service = 0;
             flags2 = flags2 - 2;
           }
           if (flags2 == 1){
             stuck_relay = "0";
           }
           if (flags3 >= 8){
             ground_ck = "0";
             flags3 = flags3 - 8;
           }
           if (flags3 >= 4){
             vent_ck = "0";
             flags3 = flags3 - 4;
           }
           if (flags3 >= 2){
             diode_ck = "0";
             flags3 = flags3 - 2;
           }
           if (flags3 == 1){
             service = 2;
           }
         }
       }  
  
     delay(100);
     Serial.flush();
     Serial.println("$GG*B2");
     comm_sent++;
     delay(100);
     while(Serial.available()) {
       String rapiString = Serial.readStringUntil('\r');
       if ( rapiString.startsWith("$OK") ) {
         comm_success++;
         String qrapi; 
         qrapi = rapiString.substring(rapiString.indexOf(' '));
         amp = qrapi.toInt();
         String qrapi1;
         qrapi1 = rapiString.substring(rapiString.lastIndexOf(' '));
         volt = qrapi1.toInt();
         last_datastr += ",CT1:";
         last_datastr += amp;
         
       }
    }  
    
    
    
    delay(100);
    Serial.flush(); 
    Serial.println("$GP*BB");
    comm_sent++;
    delay(100);
    while(Serial.available()) {
      String rapiString = Serial.readStringUntil('\r');
      if (rapiString.startsWith("$OK") ) {
        comm_success++;
        String qrapi; 
        qrapi = rapiString.substring(rapiString.indexOf(' '));
        temp1 = qrapi.toInt();
        last_datastr += ",T1:";
        last_datastr += (temp1/10);
        String qrapi1;
        int firstRapiCmd = rapiString.indexOf(' ');
        qrapi1 = rapiString.substring(rapiString.indexOf(' ', firstRapiCmd + 1 ));
        temp2 = qrapi1.toInt();
        last_datastr += ",T2:";
        last_datastr += (temp2/10);
        String qrapi2;
        qrapi2 = rapiString.substring(rapiString.lastIndexOf(' '));
        temp3 = qrapi2.toInt();
        last_datastr += ",T3:";
        last_datastr += (temp3/10);
    
    
      }
    } 
 
// Use WiFiClient class to create TCP connections
    WiFiClientSecure client;
    if (!client.connect(host, httpsPort)) {
      return;
    }
  
// We now create a URL for OpenEVSE RAPI data upload request
    String url = e_url;
    String url_amp = inputID_AMP;
    url_amp += amp;
    url_amp += ",";
    String url_volt = inputID_VOLT;
    url_volt += volt;
    url_volt += ",";
    String url_temp1 = inputID_TEMP1;
    url_temp1 += temp1;
    url_temp1 += ",";
    String url_temp2 = inputID_TEMP2;
    url_temp2 += temp2;
    url_temp2 += ","; 
    String url_temp3 = inputID_TEMP3;
    url_temp3 += temp3;
    url_temp3 += ","; 
    String url_pilot = inputID_PILOT;
    url_pilot += pilot;
    url += node;
    url += "&json={";
    url += url_amp;
    if (volt <= 0) {
      url += url_volt;
    }
    if (temp1 != 0) {
      url += url_temp1;
    }
    if (temp2 != 0) {
      url += url_temp2;
    }
    if (temp3 != 0) {
      url += url_temp3;
    }
    url += url_pilot;
    url += "}&devicekey=";
    url += apikey.c_str();
    
// This will send the request to the server
    packets_sent++;
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
    delay(10);
    String line = client.readString();
      if (line.indexOf("ok") >= 0){
          packets_success++;
        }
    
    //Serial.println(host);
    //Serial.println(url);
    
  }
}

}