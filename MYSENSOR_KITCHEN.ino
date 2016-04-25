#define SN "Kitchen Hood"
#define SV "1.0"
#include <MySensor.h> 
#include <SPI.h>
#include <DHT.h> 
#include <math.h>
#include <Wire.h>
#include <Bounce2.h>

//DEFINE CHILD_IDS
#define DIMMER_NODE_1 0
#define DIMMER_NODE_2 1
#define DIMMER_NODE_3 2
#define CHILD_ID_HUM 4
#define CHILD_ID_TEMP 5
#define CHILD_ID_MQ 6
#define CHILD_BUT1 7
#define CHILD_BUT2 8
#define CHILD_BUT3 9
#define CHILD_BUT4 10

//BUTTONS
#define BUTTON_PIN1  A0
#define BUTTON_PIN2  A1
#define BUTTON_PIN3  A2
#define BUTTON_PIN4  A3
//MQ+DHT
#define MY_LEDS_BLINKING_FEATURE
#define HUMIDITY_SENSOR_DIGITAL_PIN 8
const int MQ_Pin = A4;
//DIMMER
#define LED_PIN_1 3
#define LED_PIN_2 5
#define LED_PIN_3 6
#define FADE_DELAY 10

MySensor gw;
DHT dht;

//BUTTONS
Bounce debouncer_1 = Bounce(); 
Bounce debouncer_2 = Bounce();
Bounce debouncer_3 = Bounce();
Bounce debouncer_4 = Bounce();
int oldValue_1=-1;
int oldValue_2=-1;
int oldValue_3=-1;
int oldValue_4=-1;
bool state1;
bool state2;
bool state3;
bool state4;
MyMessage msgbut1(CHILD_BUT1,V_TRIPPED);
MyMessage msgbut2(CHILD_BUT2,V_TRIPPED);
MyMessage msgbut3(CHILD_BUT3,V_TRIPPED);
MyMessage msgbut4(CHILD_BUT4,V_TRIPPED);
//MQ+DHT
float lastTemp;
float lastHum;
float hum_floa;
float last_mq_reading;
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgMQ(CHILD_ID_MQ, V_LEVEL);
//DIMMER
byte currentLevel[3] = {0,0,0};
MyMessage dimmerMsg0(0, V_DIMMER);
MyMessage lightMsg0(0, V_LIGHT);
MyMessage dimmerMsg1(1, V_DIMMER);
MyMessage lightMsg1(1, V_LIGHT);
MyMessage dimmerMsg2(2, V_DIMMER);
MyMessage lightMsg2(2, V_LIGHT);
unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)


void setup()  
{ 
  Serial.begin(9600);
  Serial.println( SN ); 
  gw.begin( incomingMessage );
  gw.sendSketchInfo(SN, SV);
  //MQ+DHT
  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN);
  gw.present(CHILD_ID_HUM, S_HUM);
  gw.present(CHILD_ID_TEMP, S_TEMP);
  gw.present(CHILD_ID_MQ, S_AIR_QUALITY);
  //gw.wait( 50 );
  
  //DIMMER
  gw.present( DIMMER_NODE_1, S_DIMMER );
  gw.send(dimmerMsg0.set(0));
  analogWrite( LED_PIN_1, 0);
  gw.wait( 50 );
  gw.present( DIMMER_NODE_2, S_DIMMER );
  gw.send(dimmerMsg1.set(0));
  analogWrite( LED_PIN_2, 0);
  gw.wait( 50 );
  gw.present( DIMMER_NODE_3, S_DIMMER );
  gw.send(dimmerMsg2.set(0));
  analogWrite( LED_PIN_3, 0);
  gw.wait( 50 );
// gw.request( DIMMER_NODE_1, V_DIMMER );
// gw.wait( 50 );
// gw.request( DIMMER_NODE_2, V_DIMMER );
 // gw.wait( 50 );
 // gw.request( DIMMER_NODE_3, V_DIMMER );
  
  //BUTTONS
  pinMode(BUTTON_PIN1,INPUT);
  digitalWrite(BUTTON_PIN1, HIGH);
  pinMode(BUTTON_PIN2,INPUT);
  digitalWrite(BUTTON_PIN2, HIGH);
  pinMode(BUTTON_PIN3,INPUT);
  digitalWrite(BUTTON_PIN3, HIGH);
  pinMode(BUTTON_PIN4,INPUT);
  digitalWrite(BUTTON_PIN4, HIGH);
  debouncer_1.attach(BUTTON_PIN1);
  debouncer_1.interval(5);
  debouncer_2.attach(BUTTON_PIN2);
  debouncer_2.interval(5);
  debouncer_3.attach(BUTTON_PIN3);
  debouncer_3.interval(5);
  debouncer_4.attach(BUTTON_PIN4);
  debouncer_4.interval(5);
  gw.present(CHILD_BUT1, S_DOOR);
  gw.wait( 50 );
  gw.present(CHILD_BUT2, S_DOOR);
  gw.wait( 50 );
  gw.present(CHILD_BUT3, S_DOOR);
  gw.wait( 50 );
  gw.present(CHILD_BUT4, S_DOOR);
  gw.wait( 50 );
}

void loop() 
{
  gw.process();
int value_but_1 = debouncer_1.read();
int value_but_2 = debouncer_2.read();
int value_but_3 = debouncer_3.read();
int value_but_4 = debouncer_4.read(); 

//BUTTONS
  debouncer_1.update();
  if (value_but_1 != oldValue_1) 
{ 
  if ( value_but_1==0)
  {
     state1 = !state1;
     gw.send(msgbut1.set(state1));
  }
     oldValue_1 = value_but_1;
}

  debouncer_2.update();
  if (value_but_2 != oldValue_2) 
{ 
  if ( value_but_2==0)
  {
     state2 = !state2;
     gw.send(msgbut2.set(state2));
  }
     oldValue_2 = value_but_2;
}

  debouncer_3.update();
  if (value_but_3 != oldValue_3) 
{ 
  if ( value_but_3==0)
  {
     state3 = !state3;
     gw.send(msgbut3.set(state3));
  }
     oldValue_3 = value_but_3;
}

  debouncer_4.update();
  if (value_but_4 != oldValue_4) 
{ 
  if ( value_but_4==0)
  {
     state4 = !state4;
     gw.send(msgbut4.set(state4));
  }
     oldValue_4 = value_but_4;
}

}
//DHT+MQ
/* delay(dht.getMinimumSamplingPeriod());
  float temperature = dht.getTemperature();
  if (isnan(temperature)) 
  { Serial.println("Failed reading temperature from DHT"); } 
  else if (temperature != lastTemp) 
  { lastTemp = temperature;
    gw.send(msgTemp.set(temperature, 1));
  }
  float humidity = dht.getHumidity();
  if (isnan(humidity)) 
  { Serial.println("Failed reading humidity from DHT"); } 
  else if (humidity != lastHum) 
  {  lastHum = humidity;
      gw.send(msgHum.set(humidity, 1));
  }
  float mq_reading = analogRead(MQ_Pin);
  if (isnan(mq_reading)) 
  { Serial.println("Failed mq_reading"); }
  else if (mq_reading != last_mq_reading) 
  { last_mq_reading = mq_reading;
    gw.send(msgMQ.set(mq_reading, 1));
  }
*/
//DIMMER
void incomingMessage(const MyMessage &message) {
  if (message.type == V_LIGHT || message.type == V_DIMMER) {
    int requestedLevel = atoi( message.data );
    requestedLevel *= ( message.type == V_LIGHT ? 100 : 1 );
    requestedLevel = requestedLevel > 100 ? 100 : requestedLevel;
    requestedLevel = requestedLevel < 0   ? 0   : requestedLevel;
    Serial.print( "Changing level to " );
    Serial.print( requestedLevel );
    Serial.print( ", from " ); 
    Serial.println( currentLevel[message.sensor] );
    fadeToLevel( requestedLevel, message.sensor);
   
switch(message.sensor)
  {
    case 0:
      gw.send(lightMsg0.set(currentLevel[0] > 0 ? 1 : 0));
      gw.send( dimmerMsg0.set(currentLevel[0]) );
      break;
    case 1:
      gw.send(lightMsg1.set(currentLevel[1] > 0 ? 1 : 0));
      gw.send( dimmerMsg1.set(currentLevel[1]) );
      break;
    case 2:
      gw.send(lightMsg2.set(currentLevel[2] > 0 ? 1 : 0));
      gw.send( dimmerMsg2.set(currentLevel[2]) );
      break;  
  }
    
    }
}

void fadeToLevel( int toLevel, byte sensorId ) {
  int delta = ( toLevel - currentLevel[sensorId] ) < 0 ? -1 : 1;
  while ( currentLevel[sensorId] != toLevel ) {
    currentLevel[sensorId] += delta;
  switch(sensorId)
  {
    case 0:
      analogWrite( LED_PIN_1, (int)(currentLevel[sensorId] / 100. * 255) );
      break;
    case 1:
      analogWrite( LED_PIN_2, (int)(currentLevel[sensorId] / 100. * 255) );  
      break;
    case 2:
      analogWrite( LED_PIN_3, (int)(currentLevel[sensorId] / 100. * 255) );
      break;      
  }
     delay( FADE_DELAY );
  }
}
