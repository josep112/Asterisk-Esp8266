#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
/*
 *  Variables WIfi
 */
#define WIFI_SSID "your_SSID"
#define WIFI_PASSWORD "your_password"
const char* host = "192.168.0.10"; // IP do Asterisk
const char* mqtt_user = "asterisk"; //user  mosquitto
const char* mqtt_pwd = "2018_pwd@@"; //password  mqtt


AsyncMqttClient mosquitto;
Ticker mqttReconnectTimer;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;
String Mac = WiFi.macAddress();
bool mqtt_inicial = false;
bool recebe_mqtt = false;

/*
 *  Wifi Connect
 */
void connectToWifi() 
{
  Serial.println("Connecting to Wi-Fi...");
  Serial.print("WIFI_SSID : ");
  Serial.println(WIFI_SSID);
  Serial.print("WIFI_Password : ");
  Serial.println(WIFI_PASSWORD);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}
//=========================================
void onWifiConnect(const WiFiEventStationModeGotIP& event)
{
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}
//=========================================
void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) 
{
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}


/*
 * Mosquitto
 */
void mqttCallback(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) 
{
  char message_buff[150];
  int i;
  for(i=0; i<len; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0'; 
  String msgString = String(message_buff);

  if (strcmp(topic,"fechadura")==0) 
  { 
     Serial.println(">>> [Mosquitto] Callback Mensagem do Topico : " + msgString);
              if(msgString == "on")
               {
                recebe_mqtt = true;
             
                }  

  }
}
    
void rx_mqtt(){
  if(recebe_mqtt){
         recebe_mqtt = false; 
         Serial.println("Triggering the Lock");
         digitalWrite(13, HIGH);
         delay(2000); // Adjusts the locking time of the lock
         digitalWrite(13, LOW);
        

  }
}

//=========================================
void connectToMqtt()
{
  Serial.println("Connecting to MQTT...");
  mosquitto.connect();
}
//=========================================
void onMqttConnect(bool sessionPresent) // MQTT Subscribe
{
  if(mosquitto.connected()){
    if (mosquitto.subscribe("fechadura", 2)) 
      {
        Serial.println(">>> [Mosquitto] Subscribe  : fechadura ");
       // Serial.println();
      } 
  }           
}     
//=========================================
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) 
{
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) 
  {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}


/*
 * Setup
 */
void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT); 
  digitalWrite(13, 0);
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  mosquitto.onConnect(onMqttConnect);
  mosquitto.onDisconnect(onMqttDisconnect);
  mosquitto.onMessage(mqttCallback);
  mosquitto.setServer(host, 1883);
  mosquitto.setCredentials(mqtt_user,mqtt_pwd);
  connectToWifi();
}


/*
 * Loop
 */
void loop() {
rx_mqtt();
}

