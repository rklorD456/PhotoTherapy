#include <avr/io.h>
#include <avr/interrupt.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SWITCH_PIN_UP 5 // Pin for the button to move the arrow up
#define SWITCH_PIN_SELECT 6 // Pin for the button to select the time
#define switchPin1  2 // Pin connected to the first switch
#define switchPin2  3 // Pin connected to the second switch
#define GroupLed1  7   // First LED pin
#define GroupLed2   8    // Second LED pin
#define GroupLed3   9    // third LED pin
#define buzzer 10 // Buzzer connected to Arduino pin 9

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int timeOptions[] = {5, 15, 25, 30};
const int arrowWidth = 5; // Width of the arrow
const int arrowHeight = 3; // Height of the arrow
int selectedTimeIndex = 0; // Index of the currently selected time option
int chosenTime=0; // Global variable for the chosen time
volatile bool switchPressed1 = false; // State of the first switch
volatile bool switchPressed2 = false; // State of the second switch
int showCountdown = 0; // Initially set to 0
int welcome = 1;
volatile bool isTimeTherapyActive = true;
int debounceDelay = 100; // Debounce delay in milliseconds
int mode = 0;

void setup() {
  Serial.begin(9600);
  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE); 

  // Set up button and LED and buzzer pins
  pinMode(SWITCH_PIN_UP, INPUT);
  pinMode(SWITCH_PIN_SELECT, INPUT);
  pinMode(GroupLed1, OUTPUT);
  pinMode(GroupLed2, OUTPUT);
  pinMode(buzzer, OUTPUT); 

  // Attach interrupts for switch buttons
  pinMode(switchPin1, INPUT);
  pinMode(switchPin2, INPUT);
}

void loop() 
{
  if (welcome == 1) {
    initiate();
    waitForTime(100);
    Serial.println("welcom");
  }
  else if(welcome == 0)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("  System mode ");
    display.setCursor(0,12);
    display.print("Mode 1 []");
    display.setCursor(0,24);
    display.print("Mode 2 []");
    display.setCursor(0,36);
    display.print("Mode 3 []");
    display.setCursor(0,48);
    display.print("Mode 4 []");
    displayModeSelection(mode);
    display.display();
    // Mode selection
    if (digitalRead(switchPin1) == LOW) {
      mode = mode + 1;
      if (mode > 3) mode = 0;
      displayModeSelection(mode);
    }
    // Start therapy process
    if (digitalRead(switchPin2) == LOW) {
      isTimeTherapyActive = true;
      InitTimeTherapy();
      waitForTime(500);
    }
  }
}

void initiate()
{
  peep(1);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("  Welcome ");
  display.display();
  display.setCursor(0,30);
  for(int i =20; i>0; i--)
  {
    display.print(".");
    waitForTime(100);
    display.display();
  }
  while(true)
  {
    Serial.println("waiting press switch 2 😊");
    if(digitalRead(switchPin2) == LOW)
    {
      Serial.println("pressed switch 2");
      welcome = 0; 
      break;
    }
  }
}

void InitTimeTherapy()
{  
  display.clearDisplay();
  display.setCursor(0,0);
  displayTimeSelection(selectedTimeIndex);
  
  while(isTimeTherapyActive)
  {
    if (digitalRead(SWITCH_PIN_UP) == LOW && debounce(SWITCH_PIN_UP))
    {
      selectedTimeIndex++;
      if (selectedTimeIndex > 3) {
        selectedTimeIndex = 0; // Reset to the first option
      }
      displayTimeSelection(selectedTimeIndex);
      waitForTime(250);
    }

    if (digitalRead(SWITCH_PIN_SELECT) == LOW && debounce(SWITCH_PIN_SELECT)) {
      chosenTime = timeOptions[selectedTimeIndex]; // Update the global variable
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0,0);
      display.print(" Selected Time ");
      display.setCursor(0,40);
      display.print(chosenTime);
      display.println(" mins");
      display.display();
      showCountdown = 1; // Set to 1 to initiate the countdown
      while (digitalRead(SWITCH_PIN_SELECT) == LOW){} // Wait for the button to be released
      waitForTime(500);
    }

    if (showCountdown == 1) {
      CountDown(chosenTime, mode);
    }
  }
}

void CountDown(int x, int y) {
  if (mode == 0) {
    digitalWrite(GroupLed1, HIGH);
    digitalWrite(GroupLed2, LOW);
    digitalWrite(GroupLed3, LOW);
  } else if (mode == 1) {
    digitalWrite(GroupLed1, LOW);
    digitalWrite(GroupLed2, HIGH);
    digitalWrite(GroupLed3, LOW);
  } else if (mode == 2) {
    digitalWrite(GroupLed1, HIGH);
    digitalWrite(GroupLed2, HIGH);
    digitalWrite(GroupLed3, LOW);
  } else if (mode == 3) {
    digitalWrite(GroupLed1, LOW);
    digitalWrite(GroupLed2, LOW);
    digitalWrite(GroupLed3, HIGH);
  }

  for (int j = x-1; j >= 0; j--) {
    for (int i = 59; i >= 0; i--) {
      if (isTimeTherapyActive && digitalRead(SWITCH_PIN_SELECT) == LOW) {
        resetSystem();
        return;
      }
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.print("   Time  ");
      display.setCursor(0, 20);
      display.print("  ");
      display.print(j);
      display.print(" : ");
      if (i < 10) display.print("0");
      display.print(i);
      display.setCursor(0, 40);
      if (y == 0 ||y == 1|| y == 2|| y == 3) {
        display.print("mode: ");
        display.print(y);
        display.print(" ON");
        display.display();
      }
      display.display();
      waitForTime(1000); 
    }
  }
  FinishTherapy();
}


void displayModeSelection(int x) {
  display.clearDisplay();
  display.setTextSize(1);  
  display.setCursor(0, 0);
  display.println("Select Mode:");
  for (int i = 0; i < 4; i++) {
    display.setCursor(0, (i + 1) * 12);
    display.print("Mode ");
    display.print(i + 1);
    display.print(" [");
    if (i == x) {
      display.print("*");
    } else {
      display.print(" ");
    }
    display.print("]");
  }
  display.display();
  waitForTime(250);
}

void displayTimeSelection(int x) {
  display.clearDisplay();
  display.setTextSize(1);  
  display.setCursor(0, 0);
  display.println("Select Time:");
  for (int i = 0; i < 4; i++) {
    display.setCursor(0, (i + 1) * 12);
    display.print(timeOptions[i]);
    display.print(" mins");

    // Draw arrow at the end of each time selection row
    int arrowX = 100; // X-coordinate of the arrow
    int arrowY = (i + 1) * 12 + 2;
    if (i == x) { // Draw arrow only for the selected time option
      display.drawLine(arrowX, arrowY, arrowX + arrowWidth, arrowY + arrowHeight / 2, SSD1306_WHITE);
      display.drawLine(arrowX, arrowY + arrowHeight, arrowX + arrowWidth, arrowY + arrowHeight / 2, SSD1306_WHITE);
    }
  }
  display.display();
}

void FinishTherapy()
{
  display.clearDisplay();
  display.setTextSize(2);  
  display.setCursor(0, 0);
  display.println("Finished");
  digitalWrite(GroupLed1, LOW);
  digitalWrite(GroupLed2, LOW);
  digitalWrite(GroupLed3, LOW);
  display.setCursor(0, 30);
  tone(buzzer, 1000);
  for(int i = 10; i > 0; i--) {
    display.print('.');
    waitForTime(100);
    display.display();
    if(i==2){noTone(buzzer);}
  } 
  display.display();
  welcome = 1;
  isTimeTherapyActive = false;
  peep(1);
  showCountdown = 0;
  mode = 0;
  display.clearDisplay();
  waitForTime(1000);
}

bool debounce(int pin) {
  static unsigned long lastDebounceTime = 0;
  static int lastButtonState = HIGH; // Store the last button state
  unsigned long currentMillis = millis();

  if (currentMillis - lastDebounceTime >= debounceDelay) {
    int buttonState = digitalRead(pin); // Read the current button state
    if (buttonState != lastButtonState) {
      lastDebounceTime = currentMillis;
      lastButtonState = buttonState; // Update the last button state
      return true;
    }
  }
  return false;
}

void resetSystem() {
  // Perform any necessary cleanup/reset actions here
  display.clearDisplay();
  tone(buzzer, 494);
  display.setTextSize(3);
  display.setCursor(0,0);
  display.print("Restarting...");
  display.display();
  isTimeTherapyActive = false;
  switchPressed1 = false;
  switchPressed2 = false;
  showCountdown = 0;
  digitalWrite(GroupLed1, LOW);
  digitalWrite(GroupLed2, LOW);
  digitalWrite(GroupLed3, LOW);
  welcome = 1;
  chosenTime = 0;
  mode = 0;
  waitForTime(1000);
  noTone(buzzer);
}


void waitForTime(unsigned long duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {}
}

void peep(int peepCount) 
{
  for (int i = 0; i < peepCount; i++) 
  {
    tone(buzzer, 1000);
    waitForTime(500);         
    noTone(buzzer);     
    waitForTime(500);         
  }
}