/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
*********/

// Load libraries
#include "BluetoothSerial.h"

// Check if Bluetooth configs are enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` and enable it
#endif

// Bluetooth Serial object
BluetoothSerial SerialBT;

// GPIO where LED is connected to
const int ledPin = 25;

// GPIO where the KY-028 is connected to
const int analogPin = 32;  // Analog pin for temperature readings
const int digitalPin = 33; // Optional digital pin for threshold checking

// Handle received and sent messages
String message = "";
char incomingChar;
String temperatureString = "";

// Timer: auxiliary variables
unsigned long previousMillis = 0;    // Stores last time temperature was published
const long interval = 10000;         // interval at which to publish sensor readings

int ledState = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(digitalPin, INPUT); // Set digital pin as input for threshold signal
  Serial.begin(115200);
  // Bluetooth device name
  SerialBT.begin("ESP32");
  Serial.println("The device started, now you can pair it with Bluetooth!");
}

void loop() {
  unsigned long currentMillis = millis();
  // Send temperature readings
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Read analog value from KY-028
    int analogValue = analogRead(analogPin);
    
    // Convert analog value to temperature (calibration required for accuracy)
    float voltage = analogValue * (3.3 / 4095.0); // Convert ADC value to voltage
    float temperature = (voltage - 0.5) * 100.0;  // Example conversion formula
    
    // Read digital signal (optional, if you need threshold monitoring)
    int digitalValue = digitalRead(digitalPin);
    
    // Create a string with temperature information
    temperatureString = String(temperature) + "C  " + String(temperature * 9.0 / 5.0 + 32.0) + "F";
    SerialBT.println(temperatureString);
    
    // Log optional digital signal
    SerialBT.println("Threshold Signal: " + String(digitalValue ? "HIGH" : "LOW"));
  }

  // Read received messages (LED control command)
  if (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n') {
      message += String(incomingChar);
    } else {
      message = "";
    }
    if (incomingChar == '1') {  
    }
    Serial.write(incomingChar);
  }

  // Check received message and control output accordingly
  if (message == "led_on") {
    ledState = 100;
    analogWrite(ledPin, ledState * 255 / 100);
  } else if (message == "led_off") {
    ledState = 0;
    analogWrite(ledPin, ledState * 255 / 100);
  } else if (message == "+5") {
    ledState += 5;
    if (ledState > 100) { ledState == 100; }
    analogWrite(ledPin, ledState * 255 / 100);
  } else if (message == "-5") {
    ledState -= 5;
    if (ledState < 0) { ledState == 0; }
    analogWrite(ledPin, ledState * 255 / 100);
  } else if (message == "+20") {
    ledState += 20;
    if (ledState > 100) { ledState == 100; }
    analogWrite(ledPin, ledState * 255 / 100);
  } else if (message == "-20") {
    ledState -= 20;
    if (ledState < 0) { ledState == 0; }
    analogWrite(ledPin, ledState * 255 / 100);
  }
  
  delay(20);
}
