//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Arduino Code testing
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int relayPin = LED_BUILTIN;
unsigned long miliseconds = 10000;
unsigned long timerSec = 0;
const byte rxAddr[6] = "00001";
bool explosion = false;

#define D1 5 // CE to pin3 NRF
#define D2 4 // CSN to pin4 NRF
#define D5 14 // SCK to pin5 NRF
#define D6 12 // MISO to pin7 NRF
#define D7 13 // MOSI to pin6 NRF

RF24 radio(D1, D2); //CE-CSN
typedef struct{
  bool explosion;
} Data;

typedef struct{
  bool response = true;
} Response;

Data data;
Response response;

void setup()
{
  Serial.begin(115200);

  Serial.println("Hello");
  Serial.println(radio.isChipConnected());

  pinMode(relayPin, OUTPUT);
  radio.begin();
  radio.setChannel(90);
  radio.setRetries(15, 15);
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1, rxAddr);
  radio.startListening();
  radio.writeAckPayload(1, &response, sizeof(response));

  Serial.println(radio.isChipConnected());

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  receivedata();
  timer();
  relay();
  payload();
}
void receivedata(){
  if (radio.available())
    radio.read(&data, sizeof(data));
}
void timer(){
  if(data.explosion)
    explosion = true;
  if (explosion)
    timerSec = timerSec + 1;
  else timerSec = 0;
  if(timerSec >= miliseconds)
    explosion = false;
}
void relay(){
  if(explosion)
    digitalWrite(relayPin, LOW);
  else
    digitalWrite(relayPin, HIGH);
}
void payload(){
  radio.writeAckPayload(1, &response, sizeof(response));
}
