/**
  ******************************************************************************
  * @file           : DHT_Sensors.cpp
  * @brief          : This file provides code for the DHT sensors initialization, 
  *                   and reading.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "DHT_Sensors.h"
#include "DHT.h"
#include "STOP2Mode.h"

// Sensor Objects
DHT dht_left(DHT_LEFT_PIN, DHT22_TYPE);
DHT dht_middle(DHT_MIDDLE_PIN, DHT11_TYPE);
DHT dht_right(DHT_RIGHT_PIN, DHT22_TYPE);

/**
  * @brief Setup function for DHT sensors. Initializes the DHT sensors.
  * @note This function must be called in `setup`, before reading any data.
  * @param None
  * @retval None
  */
void setupSensors() {
  pinMode(DHT_LEFT_PIN, INPUT);
  pinMode(DHT_MIDDLE_PIN, INPUT);
  pinMode(DHT_RIGHT_PIN, INPUT);

  dht_left.begin();
  dht_middle.begin();
  dht_right.begin();
}

void readDHTSensors(float &t_left, float &h_left, float &t_middle, float &h_middle, float &t_right, float &h_right) {
  t_left = dht_left.readTemperature();
  h_left = dht_left.readHumidity();
  
  t_middle = dht_middle.readTemperature();
  h_middle = dht_middle.readHumidity();
  
  t_right = dht_right.readTemperature();
  h_right = dht_right.readHumidity();

  // Check for errors
  if (isnan(t_left) || isnan(h_left)){
    Serial.println("Sensor read error on left DHT");
    if (isnan(t_left)) t_left = -1.0;
    if (isnan(h_left)) h_left = -1.0; 
    My_Error_Handler();
  } if (isnan(t_middle) || isnan(h_middle)){
    Serial.println("Sensor read error on middle DHT\r\n");
    if (isnan(t_middle)) t_middle = -1.0;
    if (isnan(h_middle)) h_middle = -1.0; 
    My_Error_Handler();
  } if (isnan(t_right) || isnan(h_right)) {
    Serial.println("Sensor read error on right DHT\r\n");
    if (isnan(t_right)) t_right = -1.0;
    if (isnan(h_right)) h_right = -1.0; 
    My_Error_Handler();
  }
}
