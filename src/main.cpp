#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <EncButton.h>

/*  Arduino pins  */
#define BTN_FIRE 21
#define SEMISTOR 8

/*  EEPROM adresses  */
#define WRITE_ADDRESS 0
#define SETTINGS_ADDRESS 1

/*  LCD defines  */
#define BOOT_DELAY_MS 3000
#define LCD_TITLE "Spot Welding"
#define LCD_AUTHOR "Syorito Hatsuki"

unsigned char pointer = 0;

/*  Button pressed status  */
bool isFireBtnPressed = false;
bool updateDisplay = true;

/*   Method init   */
void initSystem();
void initLCD();
void loadSettings();

void draw(unsigned char pointer);
void drawPointer(unsigned char pointer);

void onFirePressed();

/*  Init libraries  */
LiquidCrystal lcd(5, 6, 9, 10, 11, 12);
EncButton<EB_TICK, 3, 2, 4> enc(INPUT);

struct
{
  /*  Repeat count  */
  unsigned char repeatCount = 5;
  /*  Delay time in ms for repeat  */
  unsigned short int delayTimeInMs = 50;
} settings;

void setup()
{
  initSystem();
}

void initSystem()
{
  pinMode(BTN_FIRE, INPUT);
  pinMode(SEMISTOR, OUTPUT);

  initLCD();

  loadSettings();
}

void initLCD()
{
  lcd.begin(16, 2);

  lcd.setCursor(2, 0);
  lcd.print(LCD_TITLE);
  lcd.setCursor(1, 1);
  lcd.print(LCD_AUTHOR);
  delay(BOOT_DELAY_MS);
}

void loadSettings()
{
  if (EEPROM.read(WRITE_ADDRESS) != 'w')
  {
    EEPROM.put(SETTINGS_ADDRESS, settings);
    EEPROM.put(WRITE_ADDRESS, 'w');
  } else EEPROM.get(SETTINGS_ADDRESS, settings);
}

void loop()
{
  enc.tick();

  draw(pointer);
  onFirePressed();

  if (enc.click())
  {
    pointer = (pointer == 0) ? 1 : 0;
    updateDisplay = true;
  }

  if (enc.right())
  {
    switch (pointer)
    {
    case 0:
      if (settings.repeatCount < 255)
        settings.repeatCount++;
      break;
    case 1:
      if (settings.delayTimeInMs < 999)
        settings.delayTimeInMs++;
      break;
    }
    updateDisplay = true;
  }

  if (enc.left())
  {
    switch (pointer)
    {
    case 0:
      if (settings.repeatCount > 1)
        settings.repeatCount--;
      break;
    case 1:
      if (settings.delayTimeInMs > 1)
        settings.delayTimeInMs--;
      break;
    }
    updateDisplay = true;
  }

  if (enc.rightH())
  {
    switch (pointer)
    {
    case 0:
      if (settings.repeatCount <= 245)
        settings.repeatCount += 10;
      break;
    case 1:
      if (settings.delayTimeInMs <= 989)
        settings.delayTimeInMs += 10;
      break;
    }
    updateDisplay = true;
  }

  if (enc.leftH())
  {
    switch (pointer)
    {
    case 0:
      if (settings.repeatCount > 10)
        settings.repeatCount -= 10;
      break;
    case 1:
      if (settings.delayTimeInMs > 10)
        settings.delayTimeInMs -= 10;
      break;
    }
    updateDisplay = true;
  }
}

void draw(unsigned char pointer)
{
  if (updateDisplay)
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Repeat ");
    lcd.setCursor(10, 0);
    lcd.print(settings.repeatCount);
    lcd.setCursor(1, 1);
    lcd.print("Delay ");
    lcd.setCursor(10, 1);
    lcd.print(settings.delayTimeInMs);

    drawPointer(pointer);
    
    EEPROM.put(SETTINGS_ADDRESS, settings);

    updateDisplay = false;
  }
}

void drawPointer(unsigned char pointer)
{
  lcd.setCursor(9, pointer);
  lcd.print("[");
  lcd.setCursor(14, pointer);
  lcd.print("]");
}

void onFirePressed()
{
  if (digitalRead(BTN_FIRE) && !isFireBtnPressed)
  {
    for (byte i = 1; i <= settings.repeatCount; i++)
    {
      digitalWrite(SEMISTOR, HIGH);
      delay(settings.delayTimeInMs);
      digitalWrite(SEMISTOR, LOW);
      delay(settings.delayTimeInMs);
    }
    isFireBtnPressed = true;
    updateDisplay = true;
  }
  else if (!digitalRead(BTN_FIRE) && isFireBtnPressed) isFireBtnPressed = false;
}