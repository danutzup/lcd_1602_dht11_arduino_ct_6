/*
danzup arduino atmega2560 4 LM50 sensor , 1 relay , mqtt , ethernet 
*/
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include "DHT.h"
// Update these with values suitable for your hardware/network.
byte mac[]    = {  0xCE, 0xED, 0xBA, 0xAE, 0x7E, 0x56 };
IPAddress ip(192, 168, 1, 100);
//mqtt server IP
IPAddress server(192, 168, 1, 111);
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// DHT Sensor
const int DHTPin = 22;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);
//LM50 sesors connect to Analog pin
int sensor1Pin = A11;
int sensor2Pin = A12;
int sensor3Pin = A13;
int sensor4Pin = A14;
//int sensor5Pin = A10;  
//int sensor6Pin = A9;
const int relay2 = 35;
long now = millis();
long lastMeasure = 0;
int lcd_show = 0;
long lastTime = 0;
// Receive with start- and end-markers combined with parsing
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing
float temperature1E = 0.0;
float temperature1F = 0.0;
boolean newData = false;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }

//  if(topic == "camera_tehnica/relay2"){
      Serial.print("Changing Relay2 to ");
      if(messageTemp == "on"){
        digitalWrite(relay2, HIGH);
        Serial.print("Releu set On");
      }
      else if(messageTemp == "off"){
        digitalWrite(relay2, LOW);
        Serial.print("Releu set Off");
      }
  //}
  Serial.println();
}

EthernetClient ctClient;
PubSubClient client(ctClient);
//long lastReconnectAttempt = 0;

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ctClient")) {
      Serial.println("connected");  
      client.subscribe("camera_tehnica/relay2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, LOW);
  Ethernet.begin(mac, ip);
  client.setServer(server, 1883);
  client.setCallback(callback);
  dht.begin();
  delay(1500);
  //lastReconnectAttempt = 0;
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Danzup Control");
  lcd.setCursor(0, 1);
  lcd.print("version 0.6");
  delay(2000);
  lcd.clear();
}

//=====serial receiving data=======
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial1.available() > 0 && newData == false) {
        rc = Serial1.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//=======parse serial data=====
void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    temperature1E  = atof(strtokIndx);     // convert this part to a float
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    temperature1F  = atof(strtokIndx);     // convert this part to a float    

}
//============

void loop()
{
   recvWithStartEndMarkers();
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        parseData();
        ////showParsedData();
        newData = false;
    }
    static char sensor5Temp[6];
    dtostrf(temperature1E, 4, 1, sensor5Temp);    
    static char sensor6Temp[6];
    dtostrf(temperature1F, 4, 1, sensor6Temp); 
 //--------lm 50  senzor 1------------------
 //getting the voltage reading from the temperature sensor
 int reading1 = analogRead(sensor1Pin);  
 // converting that reading to voltage, for 3.3v arduino use 3.3
 float voltage1 = reading1 * 5.0;
 voltage1 /= 1024.0; 
 // print out the voltage
 //Serial.print(voltage); Serial.println(" volts");
 // now print out the temperature
 float temperature1A = (voltage1 - 0.5) * 100 ;  
 //converting from 10 mv per degree whit 500 mV offset
 //to degrees ((voltage - 500mV) times 100)
 static char sensor1Temp[6];
 dtostrf(temperature1A, 4, 1, sensor1Temp);

 //--------lm 50  senzor 2------------------
 //getting the voltage reading from the temperature sensor
 int reading2 = analogRead(sensor2Pin);  
 // converting that reading to voltage, for 3.3v arduino use 3.3
 float voltage2 = reading2 * 5.0;
 voltage2 /= 1024.0; 
 // print out the voltage
 //Serial.print(voltage); Serial.println(" volts");
 // now print out the temperature
 float temperature1B = (voltage2 - 0.5) * 100 ;  
 //converting from 10 mv per degree whit 500 mV offset
 //to degrees ((voltage - 500mV) times 100)
 static char sensor2Temp[6];
 dtostrf(temperature1B, 4, 1, sensor2Temp);
 
 //--------lm 50  senzor 3------------------
 //getting the voltage reading from the temperature sensor
 int reading3 = analogRead(sensor3Pin);  
 // converting that reading to voltage, for 3.3v arduino use 3.3
 float voltage3 = reading3 * 5.0;
 voltage3 /= 1024.0; 
 // print out the voltage
 //Serial.print(voltage); Serial.println(" volts");
 // now print out the temperature
 float temperature1C = (voltage3 - 0.5) * 100 ;  
 //converting from 10 mv per degree whit 500 mV offset
 //to degrees ((voltage - 500mV) times 100)
 static char sensor3Temp[6];
 dtostrf(temperature1C, 4, 1, sensor3Temp);
 
 //--------lm 50  senzor 4------------------
 //getting the voltage reading from the temperature sensor
 int reading4 = analogRead(sensor4Pin);  
 // converting that reading to voltage, for 3.3v arduino use 3.3
 float voltage4 = reading4 * 5.0;
 voltage4 /= 1024.0; 
 // print out the voltage
 //Serial.print(voltage); Serial.println(" volts");
 // now print out the temperature
 float temperature1D = (voltage4 - 0.5) * 100 ;  
 //converting from 10 mv per degree whit 500 mV offset
 //to degrees ((voltage - 500mV) times 100)
 static char sensor4Temp[6];
 dtostrf(temperature1D, 4, 1, sensor4Temp);
 //int temp1 = temperature1D;
/*
//--------lm 50  senzor 5------------------
 //getting the voltage reading from the temperature sensor
 int reading5 = analogRead(sensor5Pin);  
 // converting that reading to voltage, for 3.3v arduino use 3.3
 float voltage5 = reading5 * 5.0;
 voltage5 /= 1024.0; 
 // print out the voltage
 //Serial.print(voltage); Serial.println(" volts");
 // now print out the temperature
 float temperature1E = (voltage5 - 0.5) * 100 ;  
 //converting from 10 mv per degree whit 500 mV offset
 //to degrees ((voltage - 500mV) times 100)
 static char sensor5Temp[6];
 dtostrf(temperature1E, 4, 1, sensor5Temp);
 
//--------lm 50  senzor 6------------------
 //getting the voltage reading from the temperature sensor
 int reading6 = analogRead(sensor6Pin);  
 // converting that reading to voltage, for 3.3v arduino use 3.3
 float voltage6 = reading6 * 5.0;
 voltage6 /= 1024.0; 
 // print out the voltage
 //Serial.print(voltage); Serial.println(" volts");
 // now print out the temperature
 float temperature1F = (voltage6 - 0.5) * 100 ;  
 //converting from 10 mv per degree whit 500 mV offset
 //to degrees ((voltage - 500mV) times 100)
 static char sensor6Temp[6];
 dtostrf(temperature1F, 4, 1, sensor6Temp);
 */
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ctClient");

  now = millis();
  // Publishes new temperature and humidity every 50 seconds
  if (now - lastMeasure > 50000) {
    lastMeasure = now;
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float f = dht.readTemperature(true);
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t))  {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    // Computes temperature values in Celsius
    static char temperatureTemp[7];
    dtostrf(t, 4, 1, temperatureTemp);   
    static char humidityTemp[7];
    dtostrf(h, 4, 1, humidityTemp);
    // Publishes Temperature and Humidity values
    client.publish("camera_tehnica/temperature_c", temperatureTemp);
    client.publish("camera_tehnica/humidity", humidityTemp);
    client.publish("camera_tehnica/temperature1", sensor1Temp);  
    client.publish("camera_tehnica/temperature2", sensor2Temp); 
    client.publish("camera_tehnica/temperature3", sensor3Temp);
    client.publish("camera_tehnica/temperature4", sensor4Temp);
    client.publish("camera_tehnica/temperature5", sensor5Temp);    //boiler 
    client.publish("camera_tehnica/temperature6", sensor6Temp);    //panou solar
  }

if(millis() - lastTime >= 10000) {
  lastTime = millis();
  //lcd_show = !lcd_show;
  lcd_show = lcd_show + 1;
   if (lcd_show > 2) {
    lcd_show = 0;
   }
  lcd.clear();
}

if (lcd_show == 1){  
// lcd.clear();
int temp = dht.readTemperature();
//temp = temp - 1;
int humi = dht.readHumidity();
lcd.setCursor(0, 0);
lcd.print("Temp    Humi");
lcd.setCursor(0, 1);
lcd.print(temp);
lcd.setCursor(8, 1);
lcd.print(humi);
}

if (lcd_show == 0){
//  lcd.clear();
lcd.setCursor(0, 0);
lcd.print("1=");
lcd.setCursor(2, 0);
lcd.print(sensor1Temp);
lcd.setCursor(0, 1);
lcd.print("2=");
lcd.setCursor(2, 1);
lcd.print(sensor2Temp);
lcd.setCursor(8, 0);
lcd.print("3=");
lcd.setCursor(10, 0);
lcd.print(sensor3Temp);
lcd.setCursor(8, 1);
lcd.print("4=");
lcd.setCursor(10, 1);
lcd.print(sensor4Temp);
}

if (lcd_show == 2){
//  lcd.clear();
lcd.setCursor(0, 0);
lcd.print("5=");
lcd.setCursor(2, 0);
lcd.print(sensor5Temp);
lcd.setCursor(0, 1);
lcd.print("6=");
lcd.setCursor(2, 1);
lcd.print(sensor6Temp);

}

}  
