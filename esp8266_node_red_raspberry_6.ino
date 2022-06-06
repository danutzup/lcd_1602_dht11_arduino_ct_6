/*****
 *  danzup esp8266  switch mqtt raspberry pi CT
 *  
 *  
*****/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// Change the credentials below, so your ESP8266 connects to your router
//const char* ssid = "AnaSiDoru";
const char* ssid = "CameraTehnica";
const char* password = "danzupwir";
// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "192.168.1.111";
// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient ESP8266Client6;
PubSubClient client(ESP8266Client6);
//  - GPIO 5 = D1 on ESP-12E NodeMCU board
//  - GPIO 4 = D2 on ESP-12E NodeMCU board
const int releu = 5;         // pin relay connected
int switchPIN = 4;           // pin switch to control from wall pannel
int switchStatus;            // status switch
int relayStatus = 0;
long lastMsg = 0;
char msg[50];
// Initial state of relay
void intialState(){
      digitalWrite(releu, LOW);
      snprintf (msg, 75, "Off");
      client.publish("control-ct/releu", msg);
}
// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}
// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp; 
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
 
  // If a message is received on the topic control-ct/releu , you check if the message is either on or off. Turns the relay GPIO according to the message
  if(topic=="control-ct/releu"){
      Serial.print("Changing releu to ");
      if(messageTemp == "on"){
        digitalWrite(releu, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "off"){
        digitalWrite(releu, LOW);
        Serial.print("Off");
      }
  }
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
 
    if (client.connect("ESP8266Client6")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics 
      client.subscribe("control-ct/releu");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {
  pinMode(releu, OUTPUT);  
  pinMode(switchPIN, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  intialState();
}

// For this project, you don't need to change anything in the loop function. Basically it ensures that you ESP is connected to your broker
void loop() {


  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop()) {
    client.connect("ESP8266Client6");
  }

} 
