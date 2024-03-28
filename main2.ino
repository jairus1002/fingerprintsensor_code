#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2); // Set the LCD address and dimensions

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

uint8_t id;

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for serial monitor to open
  delay(100);
  
  // Initialize the LCD
  lcd.begin();  
  lcd.backlight();
  lcd.print("Fingerprint");
  lcd.setCursor(0,1);
  lcd.print("Enrollment");  
  delay(2000);

  // Set the data rate for the sensor serial port
  Serial2.begin(57600);

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

void loop() {
  lcd.print("Ready to enroll");
  lcd.setCursor(0, 1);
  lcd.print("a fingerprint!");
  delay(2000);

  lcd.clear();
  lcd.print("Enter ID # (1-127):");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
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

  while (!getFingerprintEnroll());
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
