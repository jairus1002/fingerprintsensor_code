
//google api key   ==AIzaSyCK79DYIs-lQHQftTl5JXrhwxm-mKwd0Q0

#include <WiFi.h>
#include <HTTPClient.h>

// Function to send HTTP POST request to Google Geolocation API
void sendLocationRequest() {
    HTTPClient http;
    http.begin("https://www.googleapis.com/geolocation/v1/geolocate?key=AIzaSyCK79DYIs-lQHQftTl5JXrhwxm-mKwd0Q0");
    http.addHeader("Content-Type", "application/json");

   String payload = "{\"wifiAccessPoints\": ["
                     "{\"macAddress\": \"00:11:22:33:44:55\", \"signalStrength\": -50},"
                     "{\"macAddress\": \"AA:BB:CC:DD:EE:FF\", \"signalStrength\": -60}"
                     "]}";
    
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
    } else {
        Serial.print("Error on sending POST request: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

void setup() {
    Serial.begin(115200);
    WiFi.begin("Galaxy A13", "collo2020");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void loop() {
    sendLocationRequest();
    delay(60000); // Send location request every 1 minute
}