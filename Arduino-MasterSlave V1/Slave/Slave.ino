#include <Wire.h>

#define SLAVE_ADDR 0x08
#define BACK_BUTTON 6
#define RED_LED 5
#define GREEN_LED 4
#define JOY_X A0
#define JOY_Y A1
#define SW_PIN 3
#define DEADZONE 250

// Menu
volatile uint8_t menuLevel = 0;
volatile int8_t currentOption = 0;
const uint8_t numOptions = 5;

// -- Debounce Control
const uint32_t debounceDelay = 200;
uint32_t lastDebounceTime    = 0;

// Joystick
int16_t joyX;
int16_t joyY;
int8_t joyDirY = 0;    // -1, 0, +1
int8_t lastJoyDirY = 0;
int8_t joyDirX = 0;    // -1, 0, +1
int8_t lastJoyDirX = 0;

void setup()
{
  Serial.begin(9600);
  // -- Buttons
  pinMode(SW_PIN, INPUT_PULLUP);
  pinMode(BACK_BUTTON, INPUT_PULLUP);

  // -- LEDS
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);

  // -- I2C Communication
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(sendEvent);
}

void loop()
{
   // Enter Button
  if(digitalRead(SW_PIN) == LOW && (millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis();
    menuLevel = 1;
  }
  // Joystick settings and centering.
  joyX = analogRead(JOY_X) - 512;
  joyY = (analogRead(JOY_Y) - 512)*(-1);

  // joystick centering
  // This is for horizontal displacement.
  if(abs(joyX) < DEADZONE) joyX = 0;
  if(joyX > 0)      joyDirX = 1;
  else if(joyX < 0) joyDirX = -1;
  else              joyDirX = 0;
  // This is for verticaldisplacemtn
  if(abs(joyY) < DEADZONE) joyY = 0;
  if(joyY > 0)      joyDirY = 1;
  else if(joyY < 0) joyDirY = -1;
  else              joyDirY = 0;

  // Menu navigation
  // Down
  if(joyDirY == -1 && lastJoyDirY == 0)
  {
    currentOption++;
    if(currentOption >= numOptions) currentOption = 0; 
  }
  //UP
  if(joyDirY == 1 && lastJoyDirY == 0)
  {
    currentOption--;
    if(currentOption < 0) currentOption = numOptions - 1; 
  }

  // Back buttn
  if(digitalRead(BACK_BUTTON) == LOW && (millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis();
    menuLevel = 0;
  }

  lastJoyDirY = joyDirY;
  lastJoyDirX = joyDirX;
}

void sendEvent() 
{
  Wire.write(menuLevel);
  Wire.write(currentOption);
}