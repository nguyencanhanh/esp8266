#include <ESP8266WiFi.h>
#include <PubSubClient.h>
const char* ssid = "IoTEAM Lab";
const char* password = "02438683518";
String data;
#define TOKEN "canhanh148token"  //Access token of device Display
char ThingsboardHost[] = "thingsboard.cloud";
String inputString = ""; // biến lưu dữ liệu đã nhận được
boolean stringComplete = false; // biến kiểm tra xem đã nhận đủ dữ liệu hay chưa

WiFiClient wifiClient;
PubSubClient client(wifiClient);
void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.print("connected to Wifi: ");
  Serial.println(ssid);
  client.setServer(ThingsboardHost, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  getData();
  delay(1000);
}
void getData() {
  if (Serial.available()) {
    delay(1000);  // Kiểm tra nếu có dữ liệu đang chờ đọc
    if (stringComplete) {
      // in ra dữ liệu đã nhận được
      Serial.print("Input string: ");
      Serial.println(inputString);

      // tách dữ liệu và lưu vào 2 biến
      int sta = inputString.indexOf(",");
      if (sta >= 0) {
        String Satus = inputString.substring(0, sta);
        int tem = inputString.indexOf(',', sta + 1);
        String Temperature = inputString.substring(sta + 1, tem);
        Temperature = Temperature.toFloat();
        String Humidity = inputString.substring(tem + 1);
        Humidity = Humidity.toFloat();
        String payload = "{";
        payload += "\"Status\":";
        payload += Satus;
        payload += ",";
        payload += "\"Temperature\":";
        payload += Temperature;
        payload += ",";
        payload += "\"Humidity\":";
        payload += Humidity;
        payload += "}";
        char attributes[100];
        payload.toCharArray(attributes, 100);
        client.publish("v1/devices/me/telemetry", attributes);
        Serial.println(attributes);
      } else {
        Serial.println("Invalid input string!");
      }

      // reset biến và chuỗi dữ liệu
      inputString = "";
      stringComplete = false;
    }

    // kiểm tra xem có dữ liệu mới nhận được không
    while (Serial.available()) {
      // đọc dữ liệu và lưu vào chuỗi dữ liệu
      char inChar = (char)Serial.read();
      inputString += inChar;

      // kiểm tra xem đã nhận đủ dữ liệu hay chưa
      if (inChar == '\n') {
        stringComplete = true;
      }
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if (client.connect("Esp8266", TOKEN, NULL)) {
      Serial.println("[DONE]");
    } else {
      Serial.print("[FAILED] [ rc = ");
      Serial.println(" : retrying in 5 seconds]");
      delay(500);
    }
  }
}
