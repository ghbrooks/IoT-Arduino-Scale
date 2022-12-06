#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "HX711.h" //Load Cell Amplifier
HX711 cell(D2, D3); //Amplifier is connected to these pins on the NodeMCU ESP8266 Board

long valCalibrated = 0;
long val = 0;
float count = 0;

unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;

#include "secrets-Bundant_Scale_1.h"
//#include "secrets-bun-1.h"
//#include "secrets-bun-2.h"
//#include "secrets-bun-3.h"
//#include "secrets-bun-4.h"
//#include "secrets-bun-5.h"

#define AWS_IOT_PUBLISH_TOPIC   "Bundant_Scale_1/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "Bundant_Scale_1/sub"

//#define AWS_IOT_PUBLISH_TOPIC   "Bunda-1/pub"
//#define AWS_IOT_SUBSCRIBE_TOPIC "Bunda-1/sub"

//#define AWS_IOT_PUBLISH_TOPIC   "Bunda-2/pub"
//#define AWS_IOT_SUBSCRIBE_TOPIC "Bunda-2/sub"

//#define AWS_IOT_PUBLISH_TOPIC   "Bunda-3/pub"
//#define AWS_IOT_SUBSCRIBE_TOPIC "Bunda-3/sub"

//#define AWS_IOT_PUBLISH_TOPIC   "Bunda-4/pub"
//#define AWS_IOT_SUBSCRIBE_TOPIC "Bunda-4/sub"

//#define AWS_IOT_PUBLISH_TOPIC   "Bunda-5/pub"
//#define AWS_IOT_SUBSCRIBE_TOPIC "Bunda-5/sub"

WiFiClientSecure net;

BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

PubSubClient client(net);

time_t now;
time_t nowish = 1510592825;


void NTPConnect(void)
{
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}


void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void connectAWS()
{

  delay(3000);
  WiFi.mode(WIFI_STA);

  //  set over wifi
  WiFiManager wifiManager;
  wifiManager.autoConnect("Bundant Scale");

  //  hard coded wifi
  //  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  //
  //  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  NTPConnect();

  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);

  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);


  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}


void publishMessage()
{

  val = cell.read();

  StaticJsonDocument<200> doc;
  doc["thing"] = THINGNAME;
  doc["time"] = millis();
  doc["weight"] = val;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}


void setup()
{
  Serial.begin(115200);
  connectAWS();

  
  if (isnan(val))  // Check if any reads failed and exit early (to try again).
  {
    Serial.println(F("Failed to read from weight sensor!"));
    return;
  }
  ////////// for calibrating
  //  count = count + 1;
  //  val = cell.read();
  //  //This is used for averaging/stablising val
  //  val = ((count - 1) / count) * val + (1 / count) * cell.read();
  //  // printing value in serial monitor
  ////  Serial.println( val );
  //  //used to find 2 values -- 'bunda1 untouched value' - 'averaged resting value' / '384g weight (coke can) resting' * 'coke can value'
  //  //-- without can
  //  Serial.println(val - 8481010);
  //  // or - with can
  //  Serial.println( (val - 8481010)  / 142886.0f * 384);

  ///////// for usuage
  val = cell.read();

  /////////
  Serial.print(F("weight: "));
  Serial.println(val);
  //  Serial.println(F("g"));
  delay(2000);

  now = time(nullptr);

  if (!client.connected())
  {
    connectAWS();
  }
  else
  {
    client.loop();
    if (millis() - lastMillis > 5000)
    {
      lastMillis = millis();
      publishMessage();
    }
  }
  Serial.println("I'm awake, but I'm going into deep sleep mode for 12 hours");
//  ESP.deepSleep(43200000000); // Micro Seconds
  ESP.deepSleep(10000000); // Micro Seconds
}


void loop() {
}
