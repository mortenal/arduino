#include <VirtualWire.h>

#define ID 0x01
#define TMP36 A3
#define RF 12
#define LED 13
#define LIGHT A0
#define BAUD_RATE 9600

void setup() {
  Serial.begin(BAUD_RATE);

  pinMode(LED, OUTPUT);

  vw_set_tx_pin(RF);
  vw_setup(2000);
}

void loop() {
  short tmp = getTemperature(TMP36);
  short light = getLight(LIGHT);
  char buf[1024];
  
  addId(buf, ID);
  addMetric(buf, "TMP", tmp);
  addMetric(buf, "LIGHT", light);

  sendArray(buf);
  
  delay(1000);
}

short getTemperature(short pin)
{
  float voltage = (analogRead(pin) * 5.0);
  voltage /= 1024;
  return voltage * 100.0;
}

short getLight(short pin)
{
  return analogRead(pin);
}

void toggleLed(short pin)
{
  short dr = digitalRead(pin);
  
  if (dr == LOW) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }
}

void addMetric(char *buf, char *key, int value)
{
  char buf2[128];
  snprintf(buf2, 128, ",%s:%i", key, value);
  
  strcat(buf, buf2);
}

void addId(char *buf, int id)
{
  snprintf(buf, 128, "ID:%i", id);
}

void sendArray(char *buf)
{
    Serial.println(buf);
    vw_send((uint8_t *)buf, strlen(buf));
    vw_wait_tx();
}

