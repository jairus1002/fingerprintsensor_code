

////192.168. 1.184 that works in the gateway 192.168. 1.1. Important: you need to use an available IP address in your local network and the corresponding gateway.
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <AsyncTCP.h>  // Include AsyncTCP library
//#include <AsyncWebServer.h>  // Include AsyncWebServer library

const char* ssid = "kevo"; // Replace with your WiFi SSID
const char* password = "123456789"; // Replace with your WiFi password

//AsyncWebServer server(80);  // Create AsyncWebServer instance

LiquidCrystal_I2C lcd(0x3F, 16, 2); // Set the LCD address and dimensions

// Initialize Serial2 using HardwareSerial
HardwareSerial mySerial(2); // Use Serial2
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

// Function prototypes
void enroll();
void verify();
uint8_t readnumber(void);
bool fingerprintExists(uint8_t id);
uint8_t getFingerprintEnroll();

void handlePostRequest(WiFiClient& client, String request) {
  // Check if it's a POST request
  if (request.indexOf("POST") != -1) {
    // Check if it's the correct URL
    if (request.indexOf("/enroll") != -1) {
      enroll();
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><body><h1>Enrollment process initialized</h1></body></html>");
    } else {
      client.println("HTTP/1.1 404 Not Found");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><body><h1>Not Found</h1></body></html>");
    }
  } else {
    // Respond to other HTTP methods
    client.println("HTTP/1.1 405 Method Not Allowed");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<html><body><h1>Method Not Allowed</h1></body></html>");
  }
}
void setup() {
  lcd.begin();
  Serial.begin(9600);
  lcd.backlight();

  // Connect to WiFi
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize AsyncWebServer
  //server.on("/", HTTP_POST, handlePostRequest);   // Define POST request handler
  //server.begin();

  lcd.clear();
  lcd.print("Fingerprint");
  lcd.setCursor(0, 1);
  lcd.print("Enrollment/Verify");
  delay(2000);

  // Set the data rate for the sensor serial port
  mySerial.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    lcd.clear();
    lcd.print("Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("sensor found!");
  } else {
    lcd.clear();
    lcd.print("Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("sensor not found!");
    while (1) { delay(1); }
  }

  delay(500); // Add delay after sensor found message

  lcd.setCursor(0, 1);
  lcd.print("Reading sensor");
  lcd.setCursor(0, 1);
  lcd.print("parameters");

  // Read sensor parameters
  finger.getParameters();
  lcd.clear();
  lcd.print("Capacity: "); lcd.print(finger.capacity);
  lcd.setCursor(0, 1);
  lcd.print("Security level: "); lcd.print(finger.security_level);
  delay(2000);
  lcd.clear();
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_OK) {
    // Image taken successfully, convert it to a template
    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) {
      lcd.clear();
      lcd.print("Image conversion failed!");
      return p;
    }
  } else if (p != FINGERPRINT_NOFINGER) {
    // Handle other cases like communication errors or imaging failures
    lcd.clear();
    lcd.print("Unknown error");
    return p;
  }
  return p;
}

/*
 WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("New client");
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Handle the HTTP POST request
  handlePostRequest(client, request);

  // Close the connection
  delay(1);
  client.stop()
*/void verify() {
  Serial.println("Entering verify() function"); // Debugging statement
  lcd.clear();
  lcd.print("Place finger...");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");

  uint8_t p = FINGERPRINT_NOFINGER; // Initialize p to FINGERPRINT_NOFINGER

  // Keep looping until a fingerprint is detected
  while (p != FINGERPRINT_OK) {
    p = getFingerprintID();
    if (p == FINGERPRINT_OK) {
      // Fingerprint detected, continue with verification
      p = finger.fingerSearch();
      if (p == FINGERPRINT_OK) {
        lcd.clear();
        lcd.print("Print matched!");
        lcd.setCursor(0, 1);
        lcd.print("ID: ");
        lcd.print(finger.fingerID);
        lcd.setCursor(8, 1);
        lcd.print("Confidence: ");
        lcd.print(finger.confidence);
        delay(2000);
      } else if (p == FINGERPRINT_NOTFOUND) {
        lcd.clear();
        lcd.print("No match found!");
        delay(2000);
      } else {
        lcd.clear();
        lcd.print("Unknown error");
        delay(2000);
      }
    } else if (p == FINGERPRINT_NOFINGER) {
      lcd.clear();
      lcd.print("No finger detected");
      delay(500); // Add a small delay before checking again
    } else {
      lcd.clear();
      lcd.print("Unknown error");
      delay(2000);
    }
  }
}


 void loop() {


  lcd.clear();
  lcd.print("Press 'e' to enroll");
  lcd.setCursor(0, 1);
  lcd.print("'v' to verify");

  // Wait until there is data available in the serial buffer
  while (!Serial.available()) {
    delay(100); // Add a small delay to reduce CPU load
  }

  char choice = Serial.read(); // Read the character from the serial buffer

  // Clear the serial buffer by reading any remaining characters
  while (Serial.available()) {
    Serial.read();
  }

  // Debugging message to check the input character
  Serial.print("Choice: ");
  Serial.println(choice);

  switch (choice) {
    case 'v':
      verify();
      break;
    case 'e':
      enroll();
      break;
    default:
      lcd.clear();
      lcd.print("Invalid choice!");
      delay(2000);
      break;
  }
}


void enroll() {
  lcd.clear();
  lcd.print("Ready to enroll");
  lcd.setCursor(0, 1);
  lcd.print("a fingerprint!");
  delay(2000);

  lcd.clear();
  lcd.print("Enter ID # (1-127):");
  id = readnumber();
  if (id == 0) { // ID #0 not allowed, try again!
    return;
  }

  if (fingerprintExists(id)) {
    lcd.clear();
    lcd.print("Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("exists!");
    delay(2000);
    return;
  }

  lcd.clear();
  lcd.print("Enrolling ID #");
  lcd.print(id);
  delay(2000);

  uint8_t p = getFingerprintEnroll();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("enrolled and stored!");
    delay(2000);
  } else {
    lcd.clear();
    lcd.print("Error enrolling");
    lcd.setCursor(0, 1);
    lcd.print("fingerprint!");
    delay(2000);
  }
}


uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

bool fingerprintExists(uint8_t id) {
  return finger.getImage() == FINGERPRINT_OK;
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  lcd.clear();
  lcd.print("Place finger...");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    delay(50); // Add delay for stability
  }

  // Image taken
  lcd.clear();
  lcd.print("Image taken!");
  delay(2000);

  // Convert image to template
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Error converting");
    lcd.setCursor(0, 1);
    lcd.print("image!");
    delay(2000);
    return p;
  }

  // Remove finger
  lcd.clear();
  lcd.print("Remove finger...");
  delay(2000);

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
    delay(50); // Add delay for stability
  }

  // Ask for same finger again
  lcd.clear();
  lcd.print("Place same finger");
  lcd.setCursor(0, 1);
  lcd.print("again...");
  delay(2000);

  // Wait for finger
  p = -1;
  lcd.clear();
  lcd.print("Waiting...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    delay(50); // Add delay for stability
  }

  // Image taken
  lcd.clear();
  lcd.print("Image taken!");
  delay(2000);

  // Convert image to template
  // Convert image to template
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Error converting");
    lcd.setCursor(0, 1);
    lcd.print("image!");
    delay(2000);
    return p;
  }

  // Create fingerprint model
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("enrolled!");
    delay(2000);
  } else {
    lcd.clear();
    lcd.print("Error creating");
    lcd.setCursor(0, 1);
    lcd.print("model!");
    delay(2000);
    return p;
  }

  // Store fingerprint model
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("stored!");
    delay(2000);
  } else {
    lcd.clear();
    lcd.print("Error storing");
    lcd.setCursor(0, 1);
    lcd.print("fingerprint!");
    delay(2000);
    return p;
  }


  
  return true;
}
