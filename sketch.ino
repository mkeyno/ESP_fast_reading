 #define FASTLED_FORCE_SOFTWARE_SPI
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <FS.h>
#include <Hash.h>
#include <ArduinoOTA.h>
#include "spiffs/spiffs.h" 
#include <stdlib.h>
#include <spiffs_api.h>
#include <FastLED.h>


const char configHtml[] PROGMEM = R"=====(
<html>
<head>
<meta http-equiv="Content-Language" content="en-us">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Flash R/W Test</title>
<script>
 function createXHR() 
{ 
try   {return new XMLHttpRequest();} 
catch (e) {
try {return new ActiveXObject("Microsoft.XMLHTTP");} 
catch (e)  {return new ActiveXObject("Msxml2.XMLHTTP");}}
}

function show_list_file() 
{
var _AJX=createXHR();    
_AJX.onreadystatechange = function(){   
if(_AJX.readyState == 4)
{  
var div = document.getElementById('showfiles');
div.innerHTML = _AJX.responseText; 
var _listBox=document.getElementById('listfile');
_listBox.selectedIndex=_listBox.options.length-1;                         
}
}
var command  = "?code=DIR"; 
_AJX.open("GET","/process" + command, true);
_AJX.send(null); 
}

function removefile()
{
var path=document.getElementById("_path").value;
var file=document.getElementById("_file").value;

var _AJX=createXHR();    
_AJX.onreadystatechange = function(){   
if(_AJX.readyState == 4)
{  
var recieved =_AJX.responseText; show_list_file();          
}
}
var command  = "?code=RMF&patch=" + path+ '&file='+file; 
_AJX.open("GET","/process" + command, true);
_AJX.send(null);  
}

function updatePath()
{
var path=document.getElementById("_path").value;
var _AJX=createXHR();    
_AJX.onreadystatechange = function()
{   
if(_AJX.readyState == 4){}
}
var command  = "?code=UPA&patch=" + path; 
_AJX.open("GET","/process" + command, true);
_AJX.send(null);  
}
function SHOW()
{
var val ;
var _text=document.getElementById("Button2").value;
 if(_text=="Show On") { document.getElementById("Button2").value="Show Off"; val="1"; 
            document.getElementById("Button2").style.backgroundColor ="#FFFF00";
            }
 else           { document.getElementById("Button2").value="Show On";  val="0";
            document.getElementById("Button2").style.backgroundColor ="#F0F0F0";
            } 
var _AJX=createXHR(); 
 
_AJX.onreadystatechange = function()
{   
if(_AJX.readyState == 4){}
}
var command  = "?code=SHO&val=" + val; 
_AJX.open("GET","/process" + command, true);
_AJX.send(null);  
}
function parse_line()
{
var e = document.getElementById("listfile");
var line = e.options[e.selectedIndex].text;
var myArray=line.split("/");
var num=myArray.length; 
if(num>2){ 
 document.getElementById("_path").value='/'+myArray[1]; 
 document.getElementById("_file").value='/'+myArray[2]; 
 }
else     {
 document.getElementById("_path").value='/'; 
 document.getElementById("_file").value=myArray[1];
 }
}
</script>
</head>
<body>
<input type="button" id="Bu51on1" onclick="upload_file();return false;" name="" value="Upload file" style="position:absolute;left:261px;top:49px;width:73px;height:25px;z-index:0;">
<script>

function upload_file()
{
var file=document.getElementById("FileUpload1");
var path=document.getElementById("path");

if(file.files.length === 0)  return;   
var formData = new FormData();
 ///formData.append(name, value, filename);
    formData.append("data", file.files[0], path.value); 
var _AJX = new XMLHttpRequest(); 
_AJX.onreadystatechange = function()
{
 if (_AJX.readyState == 4){ 
             if(_AJX.status != 200)    alert("ERROR["+_AJX.status+"]: "+_AJX.responseText); 
             //else                        alert("upload success");
             show_list_file();
             }
}; 
_AJX.open("POST", "/update");
_AJX.send(formData); 
}
</script>
<input type="file" id="FileUpload1" style="position:absolute;left:175px;top:49px;width:73px;height:23px;line-height:23px;z-index:1;" name="FileUpload1" multiple ="false" onchange="check_validity();return false;"> 
<script>
function check_validity()
 {
 var file=document.getElementById("FileUpload1");
 var path=document.getElementById("path");
  if(file.files.length === 0)     return; 
  var filename = file.files[0].name; 
  var ext = /(?:\.([^.]+))?$/.exec(filename)[1]; 
  var name = /(.*)\.[^.]+$/.exec(filename)[1]; 
  if(typeof name !== undefined){ filename = name; } 
  if(typeof ext !== undefined){ 
       if(ext === "html") ext = "htm"; 
  else if(ext === "jpeg") ext = "jpg"; 
  filename = filename + "." + ext; }
  if(path.value === "/" || path.value.lastIndexOf("/") === 0)  { path.value = "/"+filename; } 
  else { path.value = path.value.substring(0, path.value.lastIndexOf("/")+1)+filename; }
 }  
</script>
<input type="text" id="path" style="position:absolute;left:4px;top:49px;width:155px;height:23px;line-height:23px;z-index:2;" name="Editbox1" value="">
<!-- show list -->
<div id="showfiles" style="position:absolute;left:140px;top:108px;width:110px;height:71px;z-index:3">
</div>
<input type="button" id="Button3" onclick="show_list_file();return false;" name="" value="show file List" style="position:absolute;left:14px;top:108px;width:80px;height:25px;z-index:4;">
<input type="text" id="_file" style="position:absolute;left:240px;top:3px;width:92px;height:23px;line-height:23px;z-index:5;" name="Editbox1" value="" text-align="right">
<input type="text" id="_path" style="position:absolute;left:54px;top:3px;width:105px;height:23px;line-height:23px;z-index:6;" name="Editbox1" value="" text-align="right">
<div id="wb_Text3" style="position:absolute;left:4px;top:8px;width:50px;height:18px;z-index:7;text-align:left;">
<span style="color:#000000;font-family:Arial;font-size:16px;">patch</span></div>
<div id="wb_Text4" style="position:absolute;left:196px;top:8px;width:30px;height:18px;z-index:8;text-align:left;">
<span style="color:#000000;font-family:Arial;font-size:16px;">file</span></div>
<input type="button" id="Button1" onclick="removefile();return false;" name="" value="Remove file" style="position:absolute;left:14px;top:146px;width:80px;height:25px;z-index:9;">
<input type="button" id="Button7" onclick="updatePath();return false;" name="" value="Update Path" style="position:absolute;left:14px;top:187px;width:80px;height:25px;z-index:10;">
<input type="button" id="Button2" onclick="SHOW();return false;" name="" value="Show On" style="position:absolute;left:392px;top:27px;width:96px;height:25px;z-index:11;">
</body>
</html>
)=====";  
#define NUM_LEDS  144
#define DATA_PIN    3 // (8,15),( 4,2) (6,12) (7,13) 
                      // (2,4)   (1,5) (5,14) (0,16)
#define CLOCK_PIN  4

#define Image_Line  200
#define SPI_FLASH_SEC_SIZE 4096

CRGB leds[NUM_LEDS];

extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;
extern "C" uint32_t _SPIFFS_page;
extern "C" uint32_t _SPIFFS_block;

uint32_t free_Space_location_start;
uint32_t start_address_of_imagefile[10];
static uint32_t current_raw_location;
  uint8_t * BUFFER;
  uint8_t  * LED_BUFFER;
byte number_of_file;     
     
ESP8266WebServer server(80);
const byte DNS_PORT = 53;
DNSServer dnsServer;

File fsUploadFile;
String PATH="/";
String MyIP="";
String myHostname = "ESPflash"; //add 1-14
String IMAGE_Names[10]; 
bool SHOW=false;
const char *SSID = "keyno";
const char *SSIDPASS = "09127163464";

////////////////////////////////////////////////////////////////////////
String formatBytes(size_t bytes){
       if (bytes < 1024)                 return String(bytes)+"B"; 
  else if(bytes < (1024 * 1024))         return String(bytes/1024.0)+"KB";
  else if(bytes < (1024 * 1024 * 1024))  return String(bytes/1024.0/1024.0)+"MB";
  else                                   return String(bytes/1024.0/1024.0/1024.0)+"GB";
}
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
void DIR(void){
  uint32_t total=0;
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
              String fileName = dir.fileName();
              size_t fileSize = dir.fileSize();
                    total+=fileSize;
              Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
            }
    Serial.print("\t\t\tTotal:");Serial.println(total);
  
}
void removefile(String path, String filename){
  if(!SPIFFS.exists(path+filename))  return server.send(404, "text/plain", "FileNotFound");  
  SPIFFS.remove(path+filename);             server.send(200, "text/plain", "removed");
}
void show_listfile(void){  
   String records="<select name=\"Combobox1\" width=\"200\" size=\"10\" id=\"listfile\"  onChange=\"parse_line();\"   >";
   byte i=0;
   Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
              String fileName = dir.fileName();
              size_t fileSize = dir.fileSize();
              records+="<option value=\"" + String(i)+"\">" +fileName +"</option>";
              i++;
            }
  records+="</select>";
 server.send(200, "text/plain", records);   
}
void process() {
  Serial.print("[HTML]"); Serial.println(server.arg(0));    
     if(server.arg(0)=="RMF") removefile(server.arg(1),server.arg(2)) ; 
else if(server.arg(0)=="UPA") PATH=server.arg(1);
else if(server.arg(0)=="SHO")  if(server.arg(1)=="1") SHOW=true; else SHOW=false;
else if(server.arg(0)=="DIR") show_listfile() ;
  }
bool handleFileRead(String path){///>>>>>>>>> Authentification before sending page
   Serial.print(" Request page : " + path); 
  if(path.endsWith("/")) path += "index.html";  
  String contentType = getContentType(path);   
  String pathWithGz = path + ".gz"; 
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
                             if(SPIFFS.exists(pathWithGz))  path += ".gz";                           
                            File file = SPIFFS.open(path, "r");
                            size_t sent = server.streamFile(file, contentType);///>>>>>>>>>
                            file.close();
                            Serial.println("\t[SENT]");
                            return true;
                            }
  Serial.println("\t[Not Found]");
  return false;
}
void handleFileUpload(){ // "/update"
  if(server.uri() != "/update" ) return;
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
                      String filename = upload.filename;                            
                       if(!filename.startsWith("/")) filename = "/"+filename; 
                       if(PATH.length()>3) filename=PATH+filename;
                       
                      if (SPIFFS.exists(filename) )    SPIFFS.rename(filename,(filename+".BAK"));
                      Serial.print("handleFileUpload Name: "); Serial.println(filename);
                      fsUploadFile = SPIFFS.open(filename, "w");
                      filename = String();
                      } 
  else if(upload.status == UPLOAD_FILE_WRITE){
                        //Serial.print("handleFileUpload Data: "); Serial.println(upload.currentSize);
                        if(fsUploadFile)      fsUploadFile.write(upload.buf, upload.currentSize);
                        } 
  else if(upload.status == UPLOAD_FILE_END){
                      if(fsUploadFile)      fsUploadFile.close();
                      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
                      }
}
void handleRoot() {
 server.send ( 200, "text/html", configHtml );
}
void handleOther(){
 if(!handleFileRead(server.uri()))
  {
    //Serial.println("Request other files");     
    String  message = "File Not Found\n\n";
        message += "URI: ";
        message += server.uri();
        message += "\nMethod: ";
        message += (server.method() == HTTP_GET)?"GET":"POST";
        message += "\nArguments: ";
        message += server.args();
        message += "\n";
    for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message); 
  }  
}
void print_chip_info(void){
Serial.print("Slected FlashChipSize: ");Serial.println(formatBytes(ESP.getFlashChipSize()));
 Serial.print("FlashChipRealSize: ");Serial.println(formatBytes(ESP.getFlashChipRealSize()));
Serial.print("SketchSize:");Serial.println(formatBytes(ESP.getSketchSize()));
Serial.print("FreeSketchSpace:");Serial.println(formatBytes(ESP.getFreeSketchSpace()));
Serial.print("FreeHeap:");Serial.println(formatBytes(ESP.getFreeHeap()));

Serial.print("\nChipId: ");Serial.println(ESP.getChipId());
Serial.print("CpuFreqMHz: ");Serial.println(ESP.getCpuFreqMHz());  
Serial.print("SdkVersion: ");Serial.println(ESP.getSdkVersion());
Serial.print("FlashChipSpeed: ");Serial.println(ESP.getFlashChipSpeed());
Serial.print("FlashChipMode: ");Serial.println(ESP.getFlashChipMode()); 
}
////////////////////////////////////////////////////////////////////////
byte list_images_filename(String _path){
    Dir dir = SPIFFS.openDir(_path);
    int j=0;
    while (dir.next()) IMAGE_Names[j++] = dir.fileName().substring(1);    
    for(int i=0;i<j;i++) Serial.printf("Image File=%s\n",IMAGE_Names[i].c_str());
  return j;    
}
void copy_file_from_SPIFF_to_Raw_Flash(String path, byte file_num){
uint32_t startTime = millis();
 File f = SPIFFS.open(path, "r");
 if(!f) { Serial.println("Could not open file for reading"); return; } 
size_t i = f.size();
 Serial.printf("File size is %u Bytes\n", i);
 while(i > SPI_FLASH_SEC_SIZE){
							   f.read(BUFFER, SPI_FLASH_SEC_SIZE);
							   optimistic_yield(10000); 
							   ESP.flashWrite(current_raw_location,(uint32_t *)& BUFFER, SPI_FLASH_SEC_SIZE);      
							   Serial.print("."); i -= SPI_FLASH_SEC_SIZE;
							   current_raw_location+=SPI_FLASH_SEC_SIZE;
							   } 
 f.read(BUFFER, i); 
 optimistic_yield(10000); 
 ESP.flashWrite(current_raw_location,(uint32_t *)& BUFFER, i);   
 current_raw_location+=i;  
 f.close();
 Serial.printf("copy took %u ms, memory end address=%u\n",  millis() - startTime,current_raw_location); 
start_address_of_imagefile[file_num]=current_raw_location;  
}
bool ini_custom_SPIFF(void)
{
uint32_t current_SPIFF_size = (uint32_t)(&_SPIFFS_end) - (uint32_t)(&_SPIFFS_start);  
   free_Space_location_start=(ESP.getSketchSize() + FLASH_SECTOR_SIZE - 1) & (~(FLASH_SECTOR_SIZE - 1));
uint32_t avialable_room=ESP.getFreeSketchSpace();
uint32_t page   = (uint32_t) &_SPIFFS_page;
uint32_t  block  = (uint32_t) &_SPIFFS_block; 
//Serial.printf("current SPIFF start %x -> end %x in  size of %uKB  free space=%uKB\n",(uint32_t)(&_SPIFFS_start),(uint32_t)(&_SPIFFS_end),current_SPIFF_size/1024,free_Space_location_start/1024);
Serial.printf("New Start Address= %x\n",free_Space_location_start);
 if (avialable_room > current_SPIFF_size)
  {
     Serial.println("Set New SPIFFS");
     SPIFFS.end(); // free current FS implementation
    SPIFFS = FS(fs::FSImplPtr(new SPIFFSImpl(free_Space_location_start, avialable_room, page, block, 2)));
  return true;
  }
  return false;
}

void setup() 
{   
   // Serial.begin(115200);
 Serial.begin(115200, SERIAL_8N1,SERIAL_TX_ONLY);
  delay(500);  
  Serial.setDebugOutput(true);
  Serial.println(F("\n\n\n Starting....."));
delay(500);
 SPIFFS.begin();
DIR(); 
 print_chip_info();
Serial.println(F("\t\t\t Entering the Station Mode"));
WiFi.mode(WIFI_STA);                
WiFi.begin(SSID, SSIDPASS);             
unsigned long startTime = millis();
while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000)  {Serial.write('%');delay(500); } 
if(WiFi.status() != WL_CONNECTED) {  ESP.restart();}            
 Serial.print("AP IP address: ");Serial.println(WiFi.localIP());
if (!MDNS.begin(myHostname.c_str())) {
                  Serial.println(F("Error setting up MDNS responder!"));
                  while(1) {  delay(1000); }
                  }
Serial.println(F("mDNS responder started"));

  server.on("/process", process); 
  server.on("/update",      HTTP_POST, [](){ Serial.println("Upload post");    server.send(200, "text/plain", ""); }, handleFileUpload);
  server.on("/", handleRoot); 
  server.onNotFound(handleOther);  
  server.begin();  Serial.println(F("\t\t\tHTTP server started"));  
 
  const char * headerkeys[] = {"User-Agent","Cookie"} ; //here the list of headers to be recorded
  size_t hz = sizeof(headerkeys)/sizeof(char*); 
  server.collectHeaders(headerkeys, hz ); //ask server to track these headers 
 
   ini_custom_SPIFF(); 
   LED_BUFFER = (uint8_t *)malloc(3*NUM_LEDS); 
       BUFFER = (uint8_t *)malloc(SPI_FLASH_SEC_SIZE); 
current_raw_location=free_Space_location_start;
number_of_file=list_images_filename("/");
 Serial.print(F("number of image=")); Serial.println(number_of_file);
if(number_of_file)
  {
   for(byte i=0;i<number_of_file;i++) copy_file_from_SPIFF_to_Raw_Flash("/"+IMAGE_Names[i], i);  
  }
 FastLED.addLeds<APA102,DATA_PIN,CLOCK_PIN,RGB/*,DATA_RATE_MHZ(20)*/>(leds,NUM_LEDS);
   FastLED.setBrightness(255);
  FastLED.clear();
 FastLED.show();  
}

static uint32_t _index=0,OpenlastTime=0,frame_time=0,lastLineShow=0;
byte j=0,line=0;;
void loop() 
{
  server.handleClient();    
   if(SHOW)// sensor data 
  {
    if( (millis()- OpenlastTime) >10000) // animate in 10 second
    {
      
     if(j>=number_of_file)
       {
         j=0; 
         line=0; 
         frame_time=0;        
      }
       _index=start_address_of_imagefile[j];
    Serial.printf("open file number %u name=%s address=%u\n  ",j,IMAGE_Names[j].c_str(),start_address_of_imagefile[j]);
     j++; 
     OpenlastTime=millis();
    }
    // uint8_t* LED_BUFFER = new uint8_t[NUM_LEDS*3];delete [] LED_BUFFER;
    ESP.flashRead(_index,(uint32_t *) LED_BUFFER, 144*3);   
   //Serial.printf("memcpy: %08x %08x %d\r\n", (uint32_t) _index, (uint32_t) LED_BUFFER, sizeof(LED_BUFFER)); 
          memcpy(leds,LED_BUFFER, sizeof(LED_BUFFER));  // copy 3 x LED_num byte to  LED_num array  
    // for(int i = 0; i < NUM_LEDS*3; i+=3)  leds[i/3] = CRGB(LED_BUFFER[i],LED_BUFFER[i+1],LED_BUFFER[i+2]);  
   // lastLineShow=micros();
    
     FastLED.show(); 

   //  Serial.println((micros()-lastLineShow)); 
    _index+=(NUM_LEDS*3);
    line++;
    if(line>Image_Line)
    {
      Serial.printf("\nFrame took %u ms\n",  millis() - frame_time);
      line=0;
      frame_time=millis();
    }
     
  } 
optimistic_yield(1000);   
}






