#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BMP085.h> // Include the BMP180 library

#define KEYPAD_PIN A0
#define BUZZER_PIN 12
#define LED_PIN 13
#define LM35_PIN A1 // LM35 analog input pin
#define PULSE_SENSOR_PIN A2 

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address, number of columns, number of rows
Adafruit_BMP085 bmp;

const int ROWS = 4; // Number of rows in the keypad
const int COLS = 4; // Number of columns in the keypad

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; // Connect row pins of keypad to these Arduino pins
byte colPins[COLS] = {5, 4, 3, 2}; // Connect column pins of keypad to these Arduino pins

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Wire.begin();
  lcd.begin(16, 2);
  lcd.print("Welcome! System On");
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  lcd.clear();
  lcd.print("1. Temperature");
  lcd.setCursor(0, 1);
  lcd.print("2. BP");
  lcd.setCursor(9, 0);
  lcd.print("3. Pulse");
  lcd.setCursor(9, 1);
  lcd.print("4. BMI");

  // Initialize BMP180 sensor (pressure sensor)
  
  if (!bmp.begin()) {
    lcd.clear();
    lcd.print("BMP180 not found!");
    while (1);
  }

  // Turn on the LED and buzzer initially
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
}

void loop() {
  char key = getKeyPressed();

  if (key == '1') {
    displayTemperature();
  }
  else if (key == '2') {
    displayPressure();
  }
  else if (key == '3') {
    displayHeartRate();
  }
  else if (key == '4') {
    calculateBMI();
  }
}
char getKeyPressed() {
  char key = keypad.getKey();
  return key;
}

void displayTemperature() {
  // Turn on the LED and buzzer when taking input
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000);

  int lm35Reading = analogRead(LM35_PIN);
  float temperatureC = (lm35Reading / 1024.0) * 500.0; // Convert analog reading to temperature in Celsius

  lcd.clear();
  lcd.print("Temperature: ");
  lcd.print(temperatureC);
  lcd.print(" C");

  // Turn off the LED and buzzer after input is taken
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
}

// ... Rest of your code remains unchanged

void displayPressure() {
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000);

  float pressure = bmp.readPressure() / 100.0; // Pressure in hPa

  lcd.clear();
  lcd.print("Pressure: ");
  lcd.print(pressure);
  lcd.print(" hPa");

  // Turn off the LED and buzzer after input is taken
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
  // Existing code for displaying pressure from BMP180 sensor
}
void displayHeartRate() {
  // Turn on the LED and buzzer when taking input
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000);

  int pulseRate = readPulseRate();

  if (pulseRate != -1) {
    lcd.clear();
    lcd.print("Pulse Rate: ");
    lcd.print(pulseRate);
    lcd.print(" bpm");
  }
  else {
    lcd.clear();
    lcd.print("Connect Pulse Sensor");
    lcd.setCursor(0, 1);
    lcd.print("to Body");
  }

  // Turn off the LED and buzzer after input is taken
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
}

void calculateBMI() {
  // Turn on the LED and buzzer when taking input
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000);

  lcd.clear();
  lcd.print("Enter Weight (kg):");
  float weight = readFloatKeypad();

  lcd.clear();
  lcd.print("Enter Height (cm):");
  float height = readFloatKeypad();

  lcd.clear();
  lcd.print("Enter Age:");
  int age = readIntKeypad();

  float heightInMeter = height / 100.0;
  float bmi = weight / (heightInMeter * heightInMeter);

  lcd.clear();
  lcd.print("BMI: ");
  lcd.print(bmi);

  // Turn off the LED and buzzer after input is taken
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
}

/*float readTemperature() {
  Wire.beginTransmission(TEMPERATURE_SENSOR_ADDRESS);
  Wire.write(0); // Select temperature register
  Wire.endTransmission();

  Wire.requestFrom(TEMPERATURE_SENSOR_ADDRESS, 2); // Read two bytes
  byte msb = Wire.read();
  byte lsb = Wire.read();

  if (msb == 0xFF && lsb == 0xFF) {
    return -1.0; // Invalid temperature value
  }

  int rawTemperature = (msb << 8) | lsb;
  float temperature = rawTemperature * 0.0625; // Conversion factor for the sensor

  return temperature;
}*/

float readTemperature() {
  int lm35Reading = analogRead(LM35_PIN);
  // Convert analog reading to temperature in Celsius
  float temperatureC = (lm35Reading / 1024.0) * 500.0; 

  return temperatureC;
}


int readPulseRate() {
  int pulseRate = analogRead(PULSE_SENSOR_PIN);

  if (pulseRate > 0 && pulseRate < 1024) {
    return pulseRate;
  }

  return -1; // Invalid pulse rate
}

float readFloatKeypad() {
  char inputBuffer[16];
  int i = 0;

  while (true) {
    char key = getKeyPressed();

    if (key >= '0' && key <= '9') {
      inputBuffer[i++] = key;
      lcd.write(key);
    }
    else if (key == '#') {
      inputBuffer[i] = '\0';
      return atof(inputBuffer);
    }
  }
}

int readIntKeypad() {
  char inputBuffer[16];
  int i = 0;

  while (true) {
    char key = getKeyPressed();

    if (key >= '0' && key <= '9') {
      inputBuffer[i++] = key;
      lcd.write(key);
    }
    else if (key == '#') {
      inputBuffer[i] = '\0';
      return atoi(inputBuffer);
    }
  }
}
// ... Rest of your code remains unchanged
