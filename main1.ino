#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

String url = "http://192.168.137.1/PROJECT_1/test_data.php";

const char* ssid = "kevo";
const char* password = "1234567890";

int temperature = 30;
int humidity = 40;

void setup() {
  Serial.begin(115200);
  connectWifi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }
  
  DynamicJsonDocument jsonDocument(200); // Adjust the size according to your JSON data
  
  jsonDocument["temperature"] = temperature;
  jsonDocument["humidity"] = humidity;
  
  String postData;
  serializeJson(jsonDocument, postData);
  
  HTTPClient http;
  http.begin(url);

  http.addHeader("Content-Type", "application/json"); // Sending JSON data
  
  int httpCode = http.POST(postData);

  if (httpCode > 0) {
    String payload = http.getString();
    
    Serial.print("URL: ");
    Serial.println(url);
    Serial.print("Data: ");
    Serial.println(postData);
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);
    Serial.print("Response: ");
    Serial.println(payload);
  } else {
    Serial.println("Error in HTTP request");
  }
}

void connectWifi() {
  Serial.println("Connecting to WiFi...");
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
