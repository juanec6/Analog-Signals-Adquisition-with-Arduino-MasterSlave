#include <Wire.h>
#include <Servo.h>

#define SLAVE_ADDR 0x08
#define SERVOPIN 2
#define BACK_BUTTON 6
#define RED_LED 5
#define GREEN_LED 4
#define JOY_X A0
#define JOY_Y A1
#define SW_PIN 3
#define DEADZONE 350
#define POTENTIOMETER A3
#define SWITCH1 9
#define ECHO 7
#define TRIG 8

//servo
Servo Servo1;

// Menu
volatile uint8_t menuLevel = 0;
volatile int8_t currentOption = 0;
volatile uint8_t servoPos = 0;
volatile uint32_t distance;
const uint8_t numOptions = 5;

// -- Debounce Control
const uint32_t debounceDelay = 500;
uint32_t lastDebounceTime    = 0;

// Joystick
int16_t joyX;
int16_t joyY;
int8_t joyDirY = 0;    // -1, 0, +1
int8_t lastJoyDirY = 0;
int8_t joyDirX = 0;    // -1, 0, +1
int8_t lastJoyDirX = 0;

// Potentiometer and servo and radar
uint16_t valPot;
int8_t servoDir = 1;
unsigned long duration;



// -- Servo sweep motion
void servoMotion()
{ 
  static unsigned long lastMove = 0;
  if(millis() - lastMove >= 50)
  {
    lastMove = millis();
    servoPos += servoDir;
    if(servoPos >= 180) servoDir = -1;
    if(servoPos <= 0) servoDir = 1;
    Servo1.write(servoPos);
  }
}

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

   // -- Servo
  Servo1.attach(SERVOPIN);
  pinMode(SWITCH1, INPUT_PULLUP);
  
  // HC SR04
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // -- I2C Communication
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(sendEvent);
}

void loop()
{
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

  // switch between servo sweeper or manual manage with servo
  if(digitalRead(SWITCH1) == LOW)
  {
    servoMotion();
  } 
  else 
  {
    // manual servo movement:
    valPot = analogRead(POTENTIOMETER);
    Serial.println(valPot);
    servoPos = map(valPot, 30, 1010, 0, 180);
    servoPos = constrain(servoPos, 0, 180);
    Servo1.write(servoPos);
  }

  // Menu navigation
  // Enter Button
  if(digitalRead(SW_PIN) == LOW && (millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis();
    if(menuLevel < 2) menuLevel++;
  }
  // Down
  if(joyDirY == -1 && lastJoyDirY == 0 && menuLevel == 1)
  {
    currentOption++;
    if(currentOption >= numOptions) currentOption = 0; 
  }
  //UP
  if(joyDirY == 1 && lastJoyDirY == 0 && menuLevel == 1)
  {
    currentOption--;
    if(currentOption < 0) currentOption = numOptions - 1; 
  }

  // Back buttn
  if(digitalRead(BACK_BUTTON) == LOW && (millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis();
    if(menuLevel > 0) menuLevel--;
    if(menuLevel == 0) currentOption = 0;
  }
  lastJoyDirY = joyDirY;
  lastJoyDirX = joyDirX;

  // ULTRASONIC THINGS
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  duration = pulseIn(ECHO, HIGH, 28000);
  distance = (duration * 0.0343)/2;
}

void sendEvent() 
{
  Wire.write(menuLevel);
  Wire.write(currentOption);
  Wire.write(servoPos);
  Wire.write(distance);
}
