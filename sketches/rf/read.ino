#include <VirtualWire.h>

int rxPin = 0;
int pin = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Listening");
  
  vw_set_rx_pin(0);
  vw_rx_start();
  vw_setup(2000);
}

void loop()
{
  byte message[VW_MAX_MESSAGE_LEN];
  byte messageLength = VW_MAX_MESSAGE_LEN;
  char messageC[VW_MAX_MESSAGE_LEN];
  
  if (vw_get_message(message, &messageLength))
  {
    for (int i = 0; i < messageLength; i++)
    {
      messageC[i] = (char) message[i];
    }

        // Read like this on receiver
    int id;
    int temp;
    int light;
    
    sscanf(messageC, "ID:%i,TMP:%i,LIGHT:%i", &id, &temp, &light);

    Serial.write("id: "); Serial.println(id);
    Serial.write("tmp: "); Serial.println(temp);
    Serial.write("light: "); Serial.println(light);
  }
}
