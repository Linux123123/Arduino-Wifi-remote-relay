//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// NodeMcu transmitter module code
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define D1 5 // CE to pin3 NRF
#define D2 4 // CSN to pin4 NRF
#define D5 14 // SCK to pin5 NRF
#define D6 12 // MISO to pin7 NRF
#define D7 13 // MOSI to pin6 NRF

const char* ssid = "NodeBOMB";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

String isCommunicating;

IPAddress local_ip(192,168,2,1);
IPAddress gateway(192,168,2,1);
IPAddress subnet(255,255,255,0);

typedef struct{
   bool explosion = true;
} Data;

typedef struct{
  bool response;
} Response;

Data data;
Response response;

const byte rxAddr[6] = "00001";

char webpage[] PROGMEM = R"=====(
<html>
<head>
<meta charset="utf-8" />
<meta name="viewport" content="width=device-width, initial-scale=1" />
<title>BOOOOM</title>
<style>
.container {
  height: 200px;
  position: relative;
}
.center {
  margin: 0;
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
}
.diva {
  text-align: center;
}
.button {
  background-color: #FF0000;
  border: none;
  padding: 30px 64px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 30px;
  color:#ffff00
}
</style>
</head>
<body>
<div class="container">
<div class="center">
<div class="diva">
<h1> Signalo statusas: <span id="signal-state">__</span> </h1>
<button class="button" onclick="myFunction()"> BOOM </button>
</div>
</div>
</div>
</body>
<script>
function meFunction()
{
  var xhra = new XMLHttpRequest();
  var url = "/signalstate";
  xhra.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("signal-state").innerHTML = this.responseText;
    }
  };
  xhra.open("GET", url, true);
  xhra.send();
};
function myFunction()
{
  var xhr = new XMLHttpRequest();
  var url = "/explosion";
  xhr.open("GET", url, false);
  xhr.send();
};
var meVar = setInterval(meFunction, 650);
</script>
</html>
)=====";

RF24 radio(D1, D2); //CE-CSN
ESP8266WebServer server(80);

void setup()
{
   WiFi.softAP(ssid, password, 8);
   WiFi.softAPConfig(local_ip, gateway, subnet);
   delay(100);
   radio.begin();
   radio.setChannel(90);
   radio.setRetries(15, 15);
   radio.enableAckPayload();
   radio.setDataRate(RF24_250KBPS);
   radio.setPALevel(RF24_PA_MAX);
   radio.openWritingPipe(rxAddr);
   radio.stopListening();
   server.on("/", handle_OnConnect);
   server.on("/explosion", handle_explosion);
   server.on("/signalstate", getSignalState);
   server.onNotFound(handle_NotFound);
   server.begin();
}
void loop()
{
   server.handleClient();
   senddata();
   receivedata();
   proccesdata();
}
void getSignalState(){
  server.send(200,"text/plain", isCommunicating);
}
void handle_OnConnect() {
  server.send_P(200, "text/html", webpage); 
}
void handle_explosion() {
  data.explosion = true;
  server.send(200, "text/plain", "BOOOOOM"); 
}
void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
void senddata(){
  radio.write(&data, sizeof(data));
}
void receivedata(){
  if (radio.isAckPayloadAvailable()) 
    radio.read(&response, sizeof(response));
  else response.response = false;
}
void proccesdata(){
  if (response.response)
    isCommunicating = "Yra signalas";
  else isCommunicating = "Nera signalo";
  data.explosion = false;
}
