#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

//LCD & RTC Instances.
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

// I2C protocol
#define SLAVE_ADDR 0x08


// Display Menu
const uint8_t numOptions = 5;
const char* menu[numOptions] = {
  "Radar",
  "Logs",
  "Microphone",
  "Humidity",
  "Temperature"
};

// Menu things
uint8_t menuLevel = 0;
uint8_t lastMenuLevel = 10;
uint8_t currentOption = 0;
uint8_t lastCurrentOption = 10;

// -- TIME AND DATE VARIABLES
uint8_t hour;
uint8_t minutes;
uint8_t seconds;
uint8_t day;
uint8_t month;
uint16_t year;

// setup
void setup()
{
  Serial.begin(9600);
  Wire.begin();

  // -- LCD Initialization
  lcd.init();
  lcd.backlight();

  // -- RTC INITIALIZATION --
  if(rtc.begin() == false)
  {
    lcd.print(F("Error RTC"));
    while(1);
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop()
{
  Wire.requestFrom(SLAVE_ADDR, 2);
  if(Wire.available())
  {
    menuLevel = Wire.read();
    currentOption = Wire.read();
  }

  if(menuLevel != lastMenuLevel || currentOption != lastCurrentOption)
  {
    lcd.clear();
    lastMenuLevel = menuLevel;
    lastCurrentOption = currentOption;
  }

  // Time and date declarations
  DateTime now = rtc.now();
  hour    = now.hour();
  minutes = now.minute();
  seconds = now.second();
  year    = now.year();
  month   = now.month();
  day     = now.day();

  // Dis play management
  switch(menuLevel)
  {
    case 0:
    {
      lcd.setCursor(0, 0);
      lcd.print(F("Time: "));
      if (hour < 10) lcd.print(F("0"));
      lcd.print(hour);
      lcd.print(F(":"));
      if (minutes < 10) lcd.print(F("0"));
      lcd.print(minutes);
      lcd.print(F(":"));
      if (seconds < 10) lcd.print(F("0"));
      lcd.print(seconds);
      lcd.print(F("   "));

      // DISPLAY DATE
      lcd.setCursor(0, 1);
      lcd.print(F("Date: "));
      if (day < 10) lcd.print(F("0"));
      lcd.print(day);
      lcd.print(F("/"));
      if (month < 10) lcd.print(F("0"));
      lcd.print(month);
      lcd.print(F("/"));
      lcd.print(year);
      lcd.print(F(" "));
      break;
    }
    case 1:
    {
      int8_t upper = currentOption;
      int8_t lower = (currentOption + 1)%numOptions;
      lcd.setCursor(0, 0);
      lcd.print(F("> "));
      lcd.print(menu[upper]);
      lcd.setCursor(0, 1);
      lcd.print(F("  "));
      lcd.print(menu[lower]);
      break;
    }
  }
}
