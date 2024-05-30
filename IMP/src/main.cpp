#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLECharacteristic.h>

#define RED_PIN   14 // GPIO pin for the red channel
#define GREEN_PIN 27 // GPIO pin for the green channel
#define BLUE_PIN  16 // GPIO pin for the blue channel

#define LED_PIN1 17 // GPIO pin for the 1st led
#define LED_PIN2 25 // GPIO pin for the 2nd led
#define LED_PIN3 26 // GPIO pin for the 3rd led

#define SERVICE_UUID        "32c37b84-8cc4-44b6-a35a-f871c2c51fa0"
#define CHARACTERISTIC_UUID "ee6d23f3-f052-471c-b789-5a2318dc415d" 

// Define BLE server, service, characteristics, and advertising
BLEServer* pServer = NULL;
BLEService* pService = NULL;
BLECharacteristic* pCharacteristic = NULL;
BLEAdvertising* pAdvertising = NULL;

unsigned long rgbStartTime = 0;
unsigned long normalLedStartTime = 0;
unsigned long RGBinterval = 60; // Time interval for RGB LED routines (ms)
unsigned long LEDinterval = 400; // Time interval for LED routines (ms)

unsigned long effectInterval = 5000; // Time interval for switching effects (ms)
int currentRGBEffect = 0; // Initial RGB effect
int currentLEDEffect = 0; // Initial LED effect

// Function definitions
void setRGB(int red, int green, int blue);

void RGBFire();
void RGBFlicker();
void RGBBreathe();
void RGBPolice();
void RGBFade();

void LEDRaceStart();
void LEDChase();
void LEDBinary();
void LEDRise();

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    // Convert the std::string to a C string
    const char* receivedData = value.c_str();

    // Compare the received command with known commands
    if (!strcmp(receivedData, "LED_SPEED_UP")) {
      switch (LEDinterval) {
        case 400:
          LEDinterval = 200;
          break;
        
        case 600:
          LEDinterval = 400;
          break;

        case 800:
          LEDinterval = 600;
          break;

        default:
          break;
      }

    } else if (!strcmp(receivedData, "LED_SPEED_DOWN")) {
      switch (LEDinterval) {
        case 200:
          LEDinterval = 400;
          break;
        
        case 400:
          LEDinterval = 600;
          break;
        
        case 600:
          LEDinterval = 800;
          break;

        default:
          break;
      }

    } else if (!strcmp(receivedData, "RGB_SPEED_UP")) {
      switch (RGBinterval) {
        case 60:
          RGBinterval = 30;
          break;
        
        case 90:
          RGBinterval = 60;
          break;
        
        case 120:
          RGBinterval = 90;
          break;

        default:
          break;
      }

    } else if (!strcmp(receivedData, "RGB_SPEED_DOWN")) {
      switch (RGBinterval) {
        case 30:
          RGBinterval = 60;
          break;
        
        case 60:
          RGBinterval = 90;
          break;
        
        case 90:
          RGBinterval = 120;
          break;

        default:
          break;
      }
    } else if (!strcmp(receivedData, "LED_MODE_UP")) {
      if(currentLEDEffect == 3) {
        currentLEDEffect = 0;
      } else {
        currentLEDEffect++;
      }
    } else if (!strcmp(receivedData, "LED_MODE_DOWN")) {\
      if(currentLEDEffect == 0) {
        currentLEDEffect = 3;
      } else {
        currentLEDEffect--;
      }     
    } else if (!strcmp(receivedData, "RGB_MODE_UP")) {
      if(currentRGBEffect == 4) {
        currentRGBEffect = 0;
      } else {
        currentRGBEffect++;
      }  
    } else if (!strcmp(receivedData, "RGB_MODE_DOWN")) {
      if(currentRGBEffect == 0) {
        currentRGBEffect = 4;
      } else {
        currentRGBEffect--;
      } 
    } 
  }
};

class MyServerCallbacks: public BLEServerCallbacks {
  void onDisconnect(BLEServer* pServer) {
    pAdvertising->start(); // Restart advertising after disconnection
  }
};

void setup() {

  Serial.begin(115200);

  // Initialize BLE Device
  BLEDevice::init("ESP32_BT");

  // Create BLE server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service
  pService = pServer->createService(BLEUUID(SERVICE_UUID));

  // Create BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
    BLEUUID(CHARACTERISTIC_UUID),
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // Start the service
  pService->start();

  // Advertise the service
  pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  rgbStartTime = millis();
}

void loop() {

  // Update RGB LED
  if (millis() - rgbStartTime >= RGBinterval) {
    rgbStartTime = millis();
    // Apply the current effect
    switch (currentRGBEffect) {
      case 0:
        RGBBreathe();
        break;
      case 1:
        RGBFade();
        break;
      case 2:
        RGBPolice();
        break;
      case 3:
        RGBFlicker();
        break;
      case 4:
        RGBFire();
        break;
    }
  }

  // Update Normal LEDs
  if (millis() - normalLedStartTime >= LEDinterval) {
    normalLedStartTime = millis();
    // Apply the current effect
    switch (currentLEDEffect) {
      case 0:
        LEDChase();
        break;
      case 1:
        LEDRaceStart();
        break;
      case 2:
        LEDBinary();
        break;
      case 3:
        LEDRise();
        break;
    }
  }

}

void setRGB(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void RGBFire() {
  unsigned long currentMillis = millis();

  // Time interval for flickering changes
  const unsigned long flickerInterval = random(0, RGBinterval);

  static unsigned long flickerStartMillis = currentMillis;
  static int currentRed, currentGreen, currentBlue;

  // Check if it's time for a flicker change
  if (currentMillis - flickerStartMillis >= flickerInterval) {
    flickerStartMillis = currentMillis;

    // Simulate flickering brightness
    int flickerBrightness = random(128, 256);

    int flickerRed = 255;
    int flickerGreen = map(flickerBrightness, 128, 255, 0, 10); // Map the green value to interval from 0 to 13
    int flickerBlue = 0;

    // Update current color with flickering values
    currentRed = flickerRed;
    currentGreen = flickerGreen;
    currentBlue = flickerBlue;
  }

  // Set the RGB LED color
  setRGB(currentRed, currentGreen, currentBlue);
}

void RGBFlicker() {
  unsigned long currentMillis = millis();

  const unsigned long flickerInterval = 200;

  static unsigned long flickerStartMillis = currentMillis;
  static int currentRed, currentGreen, currentBlue;

  // Check if it's time for a flicker change
  if (currentMillis - flickerStartMillis >= flickerInterval) {
    flickerStartMillis = currentMillis;

    // Generate random color values
    int flickerRed = random(256);
    int flickerGreen = random(256);
    int flickerBlue = random(256);

    // Update current color with random values
    currentRed = flickerRed;
    currentGreen = flickerGreen;
    currentBlue = flickerBlue;
  }

  // Set the RGB LED color
  setRGB(currentRed, currentGreen, currentBlue);
}

void RGBBreathe() {
  unsigned long currentMillis = millis();

  // Time intervals for fade in, hold, fade out, and total cycle
  const unsigned long fadeInInterval = RGBinterval*10;   
  const unsigned long holdInterval = 200;
  const unsigned long fadeOutInterval = RGBinterval*10;
  const unsigned long cycleInterval = fadeInInterval + holdInterval + fadeOutInterval;

  static unsigned long cycleStartMillis = millis();
  static int targetRed, targetGreen, targetBlue;
  static bool fadingIn = true;
  static int cycle = 0;  // Cycle counter, without it the first cycle takes too long to start

  if(cycle == 0){
    cycleStartMillis = currentMillis;

    // Generate random color values
    targetRed = random(256);
    targetGreen = random(256);
    targetBlue = random(256);

    fadingIn = true;  // Start with fading in
    cycle++;
  } else {
    // Check if it's time to start a new cycle
    if (currentMillis - cycleStartMillis >= cycleInterval) {
      cycleStartMillis = currentMillis;

      // Generate random color values
      targetRed = random(256);
      targetGreen = random(256);
      targetBlue = random(256);

      fadingIn = true;  // Start with fading in
    }
  }

  // Calculate the percentage completion of the current phase (fade in, hold, or fade out)
  float progress = float(currentMillis - cycleStartMillis) / float(
      fadingIn ? fadeInInterval :
      (fadingIn ? fadeInInterval + holdInterval : fadeInInterval + holdInterval + fadeOutInterval)
  );

  // Ensure progress stays within [0, 1]
  progress = constrain(progress, 0.0, 1.0);

  // Calculate current color based on the progress
  int currentRed, currentGreen, currentBlue;

  if (fadingIn) {
    // Fade in phase
    currentRed = targetRed * progress;
    currentGreen = targetGreen * progress;
    currentBlue = targetBlue * progress;
  } else {
    // Fade out phase
    float fadeOutProgress = 1.0 - progress;
    currentRed = targetRed * fadeOutProgress;
    currentGreen = targetGreen * fadeOutProgress;
    currentBlue = targetBlue * fadeOutProgress;
  }

  // Set the RGB LED color
  setRGB(currentRed, currentGreen, currentBlue);

  // Check if it's time to switch from fading in to holding or fading out
  if (progress >= 1.0) {
    if (fadingIn) {
      fadingIn = false;
      cycleStartMillis = currentMillis;  // Reset the cycle start time for holding or fading out
    } else {
      fadingIn = true;
      cycleStartMillis = currentMillis;  // Reset the cycle start time for fading in
    }
  }
}

void RGBPolice() {
  unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= RGBinterval) {
    previousMillis = currentMillis;

    static int blink = 0;
    static bool isBlue = false;
    int red = 0, green = 0, blue = 0;

    switch(blink) {
      case 0:
      case 2:
        red = 255;
        break;
      case 4:
      case 6:
        blue = 255;
        break;
    }

    setRGB(red, green, blue);

    if (blink % 2 == 1) {
      red = green = blue = 0;  // Turn off for odd cases
      setRGB(red, green, blue);
    }

    blink++;

    if (blink > 7) {
      blink = 0;
    }
  }
}

void RGBFade() {

  unsigned long previousMillis = 0;
  const double fadeInterval = 1; 
  const int colorIncrement = 5;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= fadeInterval) {
    previousMillis = currentMillis;

    static int color = 0;
    int red, green, blue;

    if (color < 256) {
      // Going from green to red
      red = color;
      green = 255 - color;
      blue = 0;
    } else if (color < 512) {
      // Going from red to blue
      red = 512 - color;
      green = 0;
      blue = color - 256;
    } else if (color < 768) {
      // Going from blue to green
      red = 0;
      green = color - 513;
      blue = 768 - color;
    } else {
      // RGB led need to be set to green at this point, otherwise it blinks green
      // Reset color and start over
      red = 0;
      green = 255;
      blue = 0;
      color = 0;
    }

    setRGB(red, green, blue);
    color += colorIncrement;
  }
}

void LEDRaceStart() {
  static int raceState = 0;

  switch (raceState) {
    case 0:
      analogWrite(LED_PIN1, 255);
      analogWrite(LED_PIN2, 0);
      analogWrite(LED_PIN3, 0);
      break;
    case 1:
      analogWrite(LED_PIN1, 0);
      analogWrite(LED_PIN2, 255);
      analogWrite(LED_PIN3, 0);
      break;
    case 2:
      analogWrite(LED_PIN1, 0);
      analogWrite(LED_PIN2, 0);
      analogWrite(LED_PIN3, 255);
      break;
    case 3:
      analogWrite(LED_PIN1, 255);
      analogWrite(LED_PIN2, 255);
      analogWrite(LED_PIN3, 255);
      break;
    case 4: 
      analogWrite(LED_PIN1, 0);
      analogWrite(LED_PIN2, 0);
      analogWrite(LED_PIN3, 0);
      break;
    default:
      raceState = 0;
      break;
  }

  raceState++;
  if (raceState > 4) {
    raceState = 0;
  }
}

void LEDBinary() {
  static int number = 0;

  // Ensure number stays within the range [0, 7]
  number = (number + 1) % 8;

  // Convert the number to binary and set the LED states
  analogWrite(LED_PIN1, (number & 0b100) ? 255 : 0);
  analogWrite(LED_PIN2, (number & 0b010) ? 255 : 0);
  analogWrite(LED_PIN3, (number & 0b001) ? 255 : 0);
}

void LEDChase() {
  static int chaseState = 0;
  static bool increment = true;

  switch (chaseState) {
    case 0:
      increment = true;
      analogWrite(LED_PIN1, 255);
      analogWrite(LED_PIN2, 0);
      analogWrite(LED_PIN3, 0);
      break;
    case 1:
      analogWrite(LED_PIN1, 0);
      analogWrite(LED_PIN2, 255);
      analogWrite(LED_PIN3, 0);
      break;
    case 2:
      analogWrite(LED_PIN1, 0);
      analogWrite(LED_PIN2, 0);
      analogWrite(LED_PIN3, 255);
      increment = false;
      break;
    default:
      chaseState = 0;
      break;
  }

  if(increment) {
    chaseState++;
  } else {
    chaseState--;
  }
}

void LEDRise() {
  unsigned long previousMillis = 0;
  const double fadeInterval = 1; 
  const int increment = 23;

  static bool isInitialized = false;
  static int brightness = 0;

  if (!isInitialized) {
    // Reset everything on first call
    brightness = 0;
    previousMillis = millis();
    isInitialized = true;
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= fadeInterval) {
    previousMillis = currentMillis;

    if (brightness < 256) {
      // SLowly light up the lowest LED
      analogWrite(LED_PIN1, 0);
      analogWrite(LED_PIN2, 0);
      analogWrite(LED_PIN3, brightness);
    } else if (brightness < 512) {
      // SLowly light up the middle LED
      analogWrite(LED_PIN1, 0);
      analogWrite(LED_PIN2, brightness - 256);
    } else if (brightness < 768) {
      // SLowly light up the highest LED
      analogWrite(LED_PIN1, brightness - 512);
    } else {
      // Reset and start from the beginning
      analogWrite(LED_PIN1, 0);
      analogWrite(LED_PIN2, 0);
      analogWrite(LED_PIN3, 0);
      brightness = 0;
    }

    brightness += increment;
  }
}