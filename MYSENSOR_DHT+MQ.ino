#include <SPI.h>
#include <MySensor.h>  
#include <DHT.h> 
#include <math.h>
#include <Wire.h>
#define MY_LEDS_BLINKING_FEATURE
#define CHILD_ID_MQ 3
#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1
#define HUMIDITY_SENSOR_DIGITAL_PIN 8
unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

const int MQ_Pin = A3;

MySensor gw;
DHT dht;
float lastTemp;
float lastHum;
float hum_floa;
float last_mq_reading;
//boolean metric = true; 
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgMQ(CHILD_ID_MQ, V_LEVEL);


void setup()  
{ 
  gw.begin(NULL,41,false);
  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN); 

  // Send the Sketch Version Information to the Gateway
  gw.sendSketchInfo("Humidity", "1.0");
  gw.sendSketchInfo("Air quality", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  gw.present(CHILD_ID_HUM, S_HUM);
  gw.present(CHILD_ID_TEMP, S_TEMP);
  gw.present(CHILD_ID_MQ, S_AIR_QUALITY);
  
//  metric = gw.getConfig().isMetric;
}

void loop()      
{  
  delay(dht.getMinimumSamplingPeriod());

  float temperature = dht.getTemperature();
  if (isnan(temperature)) {
      Serial.println("Failed reading temperature from DHT");
  } else if (temperature != lastTemp) {
    lastTemp = temperature;
//    if (!metric) {
//      temperature = dht.toFahrenheit(temperature);
//    }
    gw.send(msgTemp.set(temperature, 1));
    Serial.print("T: ");
    Serial.println(temperature);
  }
//  float hum_floa = dht.getHumidity();
//  float humidity = round (hum_floa);
  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
      Serial.println("Failed reading humidity from DHT");
  } else if (humidity != lastHum) {
      lastHum = humidity;
      gw.send(msgHum.set(humidity, 1));
      Serial.print("H: ");
      Serial.println(humidity);


float mq_reading = analogRead(MQ_Pin);
  if (isnan(mq_reading)) {
      Serial.println("Failed mq_reading");
  } else if (mq_reading != last_mq_reading) {
    last_mq_reading = mq_reading;
    
    gw.send(msgMQ.set(mq_reading, 1));
    Serial.print("MQ: ");
    Serial.println(mq_reading);
  }
  }



  gw.sleep(SLEEP_TIME); //sleep a bit
}

