/*********
  Steve Fisch May, 2020
  Code taken and modified from three sources, all by Rui Santos of:
  
https://randomnerdterdtutorials.com
https://rntlab.com/question/esp32-mqtt-subscribe-to-multiple-topics/
https://randomnerdtutorials.com/esp8266-nodemcu-mqtt-publish-ds18b20-arduino/
https://randomnerdtutorials.com/esp32-esp8266-i2c-lcd-arduino-ide/
 
  
 Using MQTT readings from By 3KU_Delta in 
 
 https://www.instructables.com/id/Crocodile-Solar-Pool-Sensor/
*********/

#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <LiquidCrystal_I2C.h>

// Change the credentials below, so your ESP8266 connects to your router
#define WIFI_SSID "myssid"
#define WIFI_PASSWORD "my_password"

// Change the MQTT_HOST variable to your Raspberry Pi IP address,
// so it connects to your Mosquitto MQTT broker

#define MQTT_HOST IPAddress(192, 168, 1, 88)
#define MQTT_PORT 1883

// Create objects to handle MQTT client
AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
Ticker wifiReconnectTimer;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

// Set the LCD number of columns and rows
const int lcdColumns = 16;
const int lcdRows = 2;
float PoolTemp;
String temperatureString = ""; 
// dtostrf(PoolTemp, 3, 1, PoolTemp1); 

// Set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Thermometer icon
byte thermometerIcon[8] = {
  B00100,
  B01010,
  B01010,
  B01010,
  B01010,
  B10001,
  B11111,
  B01110
};

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
 connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

// Add more topics that want your ESP8266 to be subscribed to
//~STEVE FROM CROC

void onMqttConnect(bool sessionPresent){
  delay(10000);
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub =  mqttClient.subscribe("home/pool/solarcroc/tempc", 0);
  uint16_t packetIdSub2 = mqttClient.subscribe("home/pool/solarcroc/lastupdate", 0);
  uint16_t packetIdSub1 = mqttClient.subscribe("home/pool/solarcroc/battv", 0);
  Serial.print("Subscribing at QoS 0, packetId: ");
  Serial.println(packetIdSub);
}
  void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

// You can modify this function to handle what happens when you receive a certain message in a specific topic
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  String messageTemp;
  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
      }
      
 if (strcmp(topic, "home/pool/solarcroc/tempc") == 0) {
    lcd.clear();
//    lcd.setCursor(1, 0);
    lcd.write(0);
    lcd.print("");
//    lcd.setCursor(0, 1);
    lcd.print(messageTemp);
  }
  

  Serial.println("Publish received.");
  Serial.print("  message: ");
  Serial.println(messageTemp);
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
  
if (strcmp(topic, "home/pool/solarcroc/lastupdate") == 0) {
     lcd.setCursor(6,0);
     lcd.print("Updt:");
     lcd.print(messageTemp);
  
}

Serial.println("Publish received.");
  Serial.print("  message: ");
  Serial.println(messageTemp);
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
  
if (strcmp(topic, "home/pool/solarcroc/battv") == 0) {
     lcd.setCursor (0, 1);
     lcd.print("   Volt:");
     lcd.print(messageTemp);
}
 Serial.println("Publish received.");
  Serial.print("  message: ");
  Serial.println(messageTemp);
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
 }

void setup() {
  Serial.begin(115200);
 // Initialize LCD
  lcd.init();
  // Turn on LCD backlight
  lcd.backlight();
  // Create thermometer icon
  lcd.createChar(0, thermometerIcon);
    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
 wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
//  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  connectToWifi();
}

//NO BUTTON !!!!!
void loop() 
{ }
