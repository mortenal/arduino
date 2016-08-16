#ifndef dhtrf_h
#define dhtrf_h

#include <Arduino.h>

void d_encode(uint8_t buf[], uint8_t id, double temperature, double humidity);

double  d_temperature(uint8_t* dhtrf);
double  d_humidity(uint8_t* dhtrf);
uint8_t d_id  (uint8_t* dhtrf);

#endif