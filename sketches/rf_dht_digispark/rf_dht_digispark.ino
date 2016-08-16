#include <avr/sleep.h>
#include <avr/wdt.h>
#include <dht_rf.h>
#include <dht.h>
#include <VirtualWire.h>

#define RF_TX_PIN   2
#define LED_PIN     3
#define DHT_PIN     4
#define INTERVAL   30

dht DHT;

volatile boolean f_wdt = INTERVAL + 1;

void setup()
{
  vw_set_tx_pin(RF_TX_PIN);
  vw_setup(2000);
  pinMode(LED_PIN, OUTPUT);

  alternate(LED_PIN, HIGH, LOW, 5, 400);
}

void system_sleep() {
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA &= ~(1<<ADEN); //Disable ADC, saves ~230uA
  setup_watchdog(6);
  sleep_mode();
  ADCSRA |= (1<<ADEN); //Enable ADC
  sleep_disable();
}

void loop()
{
  if (f_wdt > INTERVAL) {
    f_wdt = 0;
    
    uint8_t buf[4];
    int chk = DHT.read(DHT_PIN);
    
    switch (chk)
    {
      case DHTLIB_OK:
        d_encode(buf, 1, DHT.temperature, DHT.humidity);
        vw_send(buf, 4);
        vw_wait_tx();
        alternate(LED_PIN, HIGH, LOW, 1, 200);
        break;
      case DHTLIB_ERROR_CHECKSUM: 
      case DHTLIB_ERROR_TIMEOUT: 
      default: 
        alternate(LED_PIN, HIGH, LOW, 10, 100);
        break;
    }
    
    system_sleep();
  }
}

void alternate(int pin, int v1, int v2, int count, int tDelay) {
  for (int i = 0; i < count; i++) {
      digitalWrite(pin, v1);
      delay(tDelay / 2);
      digitalWrite(pin, v2);
      delay(tDelay / 2);
  }
}

//Sets the watchdog timer to wake us up, but not reset
//0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
//6=1sec, 7=2sec, 8=4sec, 9=8sec
//From: http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/
void setup_watchdog(int timerPrescaler) {
 
  if (timerPrescaler > 9 ) timerPrescaler = 9; //Limit incoming amount to legal settings
 
  byte bb = timerPrescaler & 7; 
  if (timerPrescaler > 7) bb |= (1<5); //Set the special 5th bit if necessary
 
  //This order of commands is important and cannot be combined
  MCUSR &= ~(1<<WDRF); //Clear the watch dog reset
  WDTCR |= (1<<WDCE) | (1<<WDE); //Set WD_change enable, set WD enable
  WDTCR = bb; //Set new watchdog timeout value
  WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int
}
 
//This runs each time the watch dog wakes us up from sleep
ISR(WDT_vect) {
  f_wdt++;
}
