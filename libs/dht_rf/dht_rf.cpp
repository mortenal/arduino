#include <dht_rf.h>

void d_encode(uint8_t buf[], uint8_t id, double temperature, double humidity) {
  int hum10 = (int) (humidity * 10);
  int tem10 = (int) (temperature * 10);

  // 10-bits
  buf[0] = hum10 & 255;
  buf[1] = (hum10 & 768) >> 8;
  
  // 11-bits
  if (tem10 < 0) {
    buf[3] = 1;
    tem10 = tem10 * -1;
  } else {
    buf[3] = 0;
  }

  buf[1] = buf[1] | ((tem10 & 3) << 2);
  buf[2] = (tem10 & 1020) >> 2;

  // 3-bits
  buf[3] = buf[3] | ((id & 7) << 1);
}

double d_temperature(uint8_t* dhtrf) {
  int p1 = (dhtrf[1] & 12) >> 2;
  int p2 = (dhtrf[2] << 2);

  double absValue = ((double)(p1 | p2)) / 10;

  if (dhtrf[3] & 1 != 0) {
    return -absValue;
  } else {
    return absValue;
  }
}

double d_humidity(uint8_t* dhtrf) {
  return ((double)(((dhtrf[1] & 3) << 8) | dhtrf[0])) / 10;
}

uint8_t d_id(uint8_t* dhtrf) {
  return (dhtrf[3] & 14) >> 1;
}