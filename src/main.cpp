#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

/*  Pin defines  */
#define BTN_FIRE 9
#define BTN_SUBTRACT 10
#define BTN_ADD 11
#define SEMISTOR 12

/*  Serial defines (0 = disable | 1 = enable)  */
#define DEBUG 0
#if DEBUG 
  #define SERIAL_SPEED 9600
#endif
/*  EEPROM Adresses  */
#define REPEAT_ADDRESS 0
#define DELAY_ADDRESS 1

/*  LCD defines  */
#define BOOT_DELAY_MS 2000
#define LCD_TITLE "CBAPKA"
#define LCD_SOURCE "cv.fstudio.dev"

/*  repeat count  */
unsigned char repeatCount = 5;

/*  delay time in ms for repeat  */
unsigned short int delayTimeInMs = 50;

/*  Button pressed status  */
bool isAddBtnPressed = false;
bool isSubtractBtnPressed = false;
bool isFireBtnPressed = false;

bool updateDisplay = true;

/*   Method init   */
void initSystem();
void initLCD();
void loadSettings();

void draw(unsigned char pointer);
void drawPointer(unsigned char pointer);

void onAddPressed(unsigned char pointer);
void onSubtractPressed(unsigned char pointer);
unsigned char onAddAndSubtractHolded(unsigned char pointer);
void onFirePressed();
void saveSettings(unsigned char pointer);

/*  Setup LCD pins  */
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

void setup()
{
  initSystem();
}

void initSystem()
{
  pinMode(BTN_SUBTRACT, INPUT);
  pinMode(BTN_ADD, INPUT);
  pinMode(BTN_FIRE, INPUT);
  pinMode(SEMISTOR, OUTPUT);

  initLCD();

  loadSettings();
}

void initLCD()
{
  lcd.begin(12, 2);

  lcd.setCursor(5, 0);
  lcd.print(LCD_TITLE);
  lcd.setCursor(1, 1);
  lcd.print(LCD_SOURCE);
  delay(BOOT_DELAY_MS);
}

void loadSettings(){
  if (EEPROM.read(REPEAT_ADDRESS) > 0 && EEPROM.read(DELAY_ADDRESS) > 0)
  {
    repeatCount = EEPROM.read(REPEAT_ADDRESS);
    delayTimeInMs = EEPROM.read(DELAY_ADDRESS);
  } else {
    lcd.setCursor(1, 0);
    lcd.print("Error to read");
    lcd.setCursor(1, 1);
    lcd.print("DATA NOT FOUND");
  }
}

void loop()
{
  static unsigned char pointer = 0;
  draw(pointer);
  onAddPressed(pointer);
  onSubtractPressed(pointer);
  pointer = onAddAndSubtractHolded(pointer);
  onFirePressed();
}

void draw(unsigned char pointer)
{
  if (updateDisplay)
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Repeat ");
    lcd.setCursor(10, 0);
    lcd.print(repeatCount);
    lcd.setCursor(1, 1);
    lcd.print("Delay ");
    lcd.setCursor(10, 1);
    lcd.print(delayTimeInMs);

    drawPointer(pointer);
    saveSettings(pointer);

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

void onAddPressed(unsigned char pointer)
{
  if (digitalRead(BTN_ADD) && !isAddBtnPressed)
  {
    switch (pointer)
    {
    case 0:
      if (repeatCount < 255) repeatCount++;
      break;
    case 1:
      if (delayTimeInMs < 9999) delayTimeInMs++;
      break;
    }
    isAddBtnPressed = true;
    updateDisplay = true;
  }
  else if (!digitalRead(BTN_ADD) && isAddBtnPressed)
  {
    isAddBtnPressed = false;
  }
}

void onSubtractPressed(unsigned char pointer)
{
  if (digitalRead(BTN_SUBTRACT) && !isSubtractBtnPressed)
  { 
    switch (pointer)
    {
    case 0:
      if (repeatCount > 1) repeatCount--;
      break;
    case 1:
      if (delayTimeInMs > 1) delayTimeInMs--;
      break;
    }
    isSubtractBtnPressed = true;
    updateDisplay = true;
  }
  else if (!digitalRead(BTN_SUBTRACT) && isSubtractBtnPressed)
  {
    isSubtractBtnPressed = false;
  }
}

unsigned char onAddAndSubtractHolded(unsigned char pointer){
  if (isAddBtnPressed && isSubtractBtnPressed)
  {
    return (pointer == 0) ? 1 : 0;
  }
  return pointer;
}

void onFirePressed()
{
  if (digitalRead(BTN_FIRE) && !isFireBtnPressed)
  {
    for (byte i = 1; i <= repeatCount; i++)
    {
      digitalWrite(SEMISTOR, HIGH);
      delay(delayTimeInMs);
      digitalWrite(SEMISTOR, LOW);
      delay(delayTimeInMs);
    }
    isFireBtnPressed = true;
    updateDisplay = true;
  }
  else if (!digitalRead(BTN_FIRE) && isFireBtnPressed)
  {
    isFireBtnPressed = false;
  }
}

void saveSettings(unsigned char pointer) {
  switch (pointer)
  {
  case 0:
    EEPROM.update(REPEAT_ADDRESS, repeatCount);
    break;
  case 1:
    EEPROM.update(DELAY_ADDRESS, delayTimeInMs);
    break;
  }
}