#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "kevo";
const char* password = "123456789";
const char* googleApiUrl = "https://www.googleapis.com/geolocation/v1/geolocate?key=AIzaSyCK79DYIs-lQHQftTl5JXrhwxm-mKwd0Q0"; // Replace with your Google API key
const char* serverUrl = "http://192.168.137.1/PROJECT_1/gps.php"; // Replace with your server URL

void setup() {
  Serial.begin(115200);
  connectWifi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }
  
  double latitude;
  double longitude;
  
  if (sendLocationRequest(latitude, longitude)) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Create JSON object to hold latitude and longitude
    DynamicJsonDocument jsonDocument(200);
    jsonDocument["latitude"] = latitude;
    jsonDocument["longitude"] = longitude;
  
    // Serialize JSON object to string
    String postData;
    serializeJson(jsonDocument, postData);
  
    // Send HTTP POST request with JSON data
    int httpCode = http.POST(postData);

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.print("HTTP Code: ");
      Serial.println(httpCode);
      Serial.print("Response: ");
      Serial.println(payload);
    } else {
      Serial.println("Error in HTTP request");
    }
  
    http.end();
  }
  
  delay(60000); // Send location request every 1 minute
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

bool sendLocationRequest(double& latitude, double& longitude) {
  HTTPClient http;
  http.begin(googleApiUrl);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"wifiAccessPoints\": [{\"macAddress\": \"00:11:22:33:44:55\", \"signalStrength\": -50},{\"macAddress\": \"AA:BB:CC:DD:EE:FF\", \"signalStrength\": -60}]}";
  
  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    String response = http.getString();
    DynamicJsonDocument jsonDocument(1024);
    deserializeJson(jsonDocument, response);
    latitude = jsonDocument["location"]["lat"];
    longitude = jsonDocument["location"]["lng"];
    Serial.println("Latitude: " + String(latitude, 6));
    Serial.println("Longitude: " + String(longitude, 6));
    return true;
  } else {
    Serial.print("Error on sending POST request: ");
    Serial.println(httpResponseCode);
    return false;
  }

  http.end();
}
