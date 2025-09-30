/**
  ******************************************************************************
  * @file           : DHT_Sensors.h
  * @brief          : Header for DHT_Sensors.cpp file.
  *                   This file declares the necessary functions for the DHT 
  *                   sensors initialization and reading.    
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DHT_SENSORS_H
#define DHT_SENSORS_H
// Includes ------------------------------------------------------------------
#include <DHT.h>
#include "My_Error_Handler.h"
#include "WSerial.h"

// Sensor Pins
#define DHT_LEFT_PIN PA_10
#define DHT_MIDDLE_PIN PB_2
#define DHT_RIGHT_PIN PB_1
#define DHT22_TYPE DHT22
#define DHT11_TYPE DHT11

extern HardwareSerial Serial;

// Sensor Objects
extern DHT dht_left;
extern DHT dht_middle;
extern DHT dht_right;

// Functions
void setupSensors();
void readDHTSensors(float &t_left, float &h_left, float &t_middle, float &h_middle, float &t_right, float &h_right);

#endif // DHT_SENSORS_H