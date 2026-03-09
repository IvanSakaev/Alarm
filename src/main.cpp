#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <GyverButton.h>
#include <GyverEncoder.h>
#include <GyverTimer.h>
#include <bigNumbers.h>
#include <GyverPower.h>
#include <EEPROM.h>

#define DELETEL 0
#define TONE_PIN 10

LiquidCrystal_I2C lcd(0x27, 20, 4);
bigNumbers<LiquidCrystal_I2C> bigNumbersLcd(&lcd);
GButton butt_mode(2);
GButton butt_settings(3);
GButton sensor(12);
GTimer time(MS);
GTimer enc_show(MS);
GTimer sleeper(MS);
GTimer melod(MS);
GTimer off(MS);
Encoder enc(6, 7, ENC_NO_BUTTON, TYPE2);
bool isBacklight = true;

static signed char days = 0;
static signed char hours = 0;
static signed char minets = 0;
bool zaran_off = false;

bool wokeUpper()
{
  if (!isBacklight)
  {
    lcd.backlight();
    isBacklight = true;
    sleeper.setInterval(30000 >> DELETEL);
    return false;
  }
  sleeper.setInterval(30000 >> DELETEL);
  return true;
}

void ee_save0();
void ee_save1();

#include <mode1.h>
#include <mode0.h>
#include <symbols.h>
#include <melod.h>

void ee_save0()
{
  EEPROM.put(0, days);
  EEPROM.put(1, hours);
  EEPROM.put(2, minets);
}

void ee_save1()
{
  for (char i = 0; i < 4; i++)
  {
    EEPROM.put(3 + (10 * i), alar[(int)i].hs);
    EEPROM.put(4 + (10 * i), alar[(int)i].ms);
    EEPROM.put(5 + (10 * i), alar[(int)i].isBring);
    for (int j = 0; j < 7; j++)
    {
      EEPROM[6 + (10 * i) + j] = alar[(int)i].ds[j];
    }
  }
}

void setup()
{
  //* lcd
  lcd.backlight();
  lcd.init();
  lcd.backlight();
  lcd.createChar(3, doubledot);
  lcd.createChar(4, uperrow);
  lcd.createChar(5, lefterrow);
  bigNumbersLcd.intNumbers();

  //* buttons and encoder
  butt_settings.setTimeout(1000 >> DELETEL);
  butt_mode.setTickMode(AUTO);
  butt_mode.setDebounce(20 >> DELETEL);
  butt_settings.setTickMode(AUTO);
  butt_settings.setDebounce(20 >> DELETEL);
  sensor.setDebounce(50 >> DELETEL);
  sensor.setTimeout(800 >> DELETEL);
  sensor.setClickTimeout(1000 >> DELETEL);
  sensor.setType(HIGH_PULL);
  sensor.setDirection(NORM_CLOSE);
  enc.setTickMode(AUTO);

  //* timers
  enc_show.setTimeout(250 >> DELETEL);
  enc_show.reset();
  time.setInterval(60000 >> DELETEL);
  wokeUpper();

  //* power
  // power.hardwareDisable(PWR_SPI | PWR_ADC);
  // power.setSystemPrescaler(PRESCALER_2);

  //* EEPROM
  EEPROM.get(0, days);
  EEPROM.get(1, hours);
  EEPROM.get(2, minets);
  days %= 7;
  hours %= 24;
  minets %= 60;

  for (char i = 0; i < 4; i++)
  {
    EEPROM.get(3 + (10 * i), alar[(int)i].hs);
    EEPROM.get(4 + (10 * i), alar[(int)i].ms);
    alar[(int)i].isBring = (bool)EEPROM[5 + (10 * i)];
    for (int j = 0; j < 7; j++)
    {
      alar[(int)i].ds[j] = (bool)EEPROM[6 + (10 * i) + j];
    }
    alar[(int)i].hs %= 24;
    alar[(int)i].ms %= 60;
    alar[(int)i].get();
  }
}

void loop()
{
  static bool perex = true;
  static signed char mode = 0;
  static unsigned int j = 0;
  static char theAlarm = 255;

  sensor.tick();

  if (time.isReady())
  {
    minets += 1;
    if (minets >= 60)
    {
      minets = 0;
      hours += 1;
      if (hours >= 24)
      {
        hours = 0;
        days = (days + 1) % 7;
      }
    }
    ee_save0();
  }

  if (butt_mode.isClick())
  {
    if (wokeUpper())
    {
      mode = (mode + 1) % 2;
      perex = true;
    }
  }

  if (mode == 0)
  {
    mode = mode0(minets, hours, days, perex);
  }
  else if (mode == 1)
  {
    mode = mode1(perex);
  }
  perex = false;

  for (char i = 0; i < 4; i++)
  {
    if (alar[(int)i].isBringing(minets, hours, days))
    {
      if (!zaran_off)
      {
        if (alar[(int)i].bringin_on)
        {
          wokeUpper();
          melod.setTimeout(100 << DELETEL);
          alar[(int)i].bringin_on = false;
          theAlarm = i;
          break;
        }
      }
    }
    else
    {
      alar[(int)i].bringin_on = true;
      if (i == theAlarm)
      {
        j = 0;
        melod.stop();
        theAlarm = 255;
      }
    }
  }

  if (melod.isReady())
  {
    tone(10, pgm_read_dword(&freqs[j]), pgm_read_float(&lens[j]));
    melod.setTimeout(pgm_read_float(&pas[j]));
    j = (j + 1) % 269;
  }

  if (sensor.isClick())
  {
    wokeUpper();
    alar[(int)theAlarm].bringin_on = false;
    if (theAlarm != 255)
    {
      j = 0;
      melod.stop();
      theAlarm = 255;
    }
  }

  if (sensor.isTriple())
  {
    zaran_off = true;
    off.setTimeout((unsigned long)1000 * 60 * 60 * 2);
    perex = true;
  }

  if (off.isReady())
  {
    zaran_off = false;
    perex = true;
  }

  if (sleeper.isReady())
  {
    lcd.noBacklight();
    isBacklight = false;
  }
}
