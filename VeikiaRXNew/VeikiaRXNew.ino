//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Arduino Code testing
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int relayPin = 6;
unsigned long miliseconds = 50000;
unsigned long timerSec = 0;
const byte rxAddr[6] = "00001";
bool explosion = false;

RF24 radio(7, 8);
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
  pinMode(relayPin, OUTPUT);
  radio.begin();
  radio.setChannel(90);
  radio.setRetries(15, 15);
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1, rxAddr);
  radio.startListening();
  radio.writeAckPayload(1, &response, sizeof(response));
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
    digitalWrite(relayPin, HIGH);
  else
    digitalWrite(relayPin, LOW);
}
void payload(){
  radio.writeAckPayload(1, &response, sizeof(response));
}
