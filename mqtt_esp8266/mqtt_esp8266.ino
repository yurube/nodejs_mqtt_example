#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "*****-2";
const char* password = "*****";
const char* mqtt_server = "broker.shiftr.io";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  char c_payload[length];
  memcpy(c_payload, payload, length);
  c_payload[length] = '\0';

  String s_topic = String(topic);
  String s_payload = String(c_payload);
  const size_t bufferSize = JSON_OBJECT_SIZE(2) + 60;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject &root = jsonBuffer.parseObject(s_payload);
  bool status = root["output_status"];
  // Switch on the LED if an 1 was received as first character
  if (status == true) {
    digitalWrite(BUILTIN_LED, LOW);
    client.publish("get/iot/lamp/", "true");

  } else {
    digitalWrite(BUILTIN_LED, HIGH);
    client.publish("get/iot/lamp/", "false");
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), "haydutmqtt", "c123456789")) {
      Serial.println("connected");

      client.subscribe("set/iot/lamp/");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 1000) {
    //    lastMsg = now;
    //    ++value;
    //    snprintf (msg, 75, "hello world #%ld", value);
    //    Serial.print("Publish message: ");
    //    Serial.println(msg);
    //    client.publish("outTopic", msg);
  }
}
