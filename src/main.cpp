#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <GyverButton.h>
#include <GyverEncoder.h>
#include <GTimer.h>
#include <bigNumbers.h>
#include <GyverPower.h>
#include <EEPROM.h>
#include <RTClib.h>
#include "MyTime.h"
#define TONE_PIN 10

LiquidCrystal_I2C lcd(0x27, 20, 4);
bigNumbers<LiquidCrystal_I2C> bigNumbersLcd(&lcd);
RTC_DS3231 rtc;
GButton butt_mode(2);
GButton butt_settings(3);
GButton sensor(12);
Encoder enc(7, 6, ENC_NO_BUTTON, TYPE2);
GTimer<millis> time;
GTimer<millis> sleeper;
GTimer<millis> melod;
GTimer<millis> alarm_off;
GTimer<millis> enc_show;
bool isBacklight = true;

MyTime now = MyTime(0, 0, 1);
bool zaran_off = false;

bool wokeUpper()
{
  if (!isBacklight)
  {
    lcd.backlight();
    isBacklight = true;
    sleeper.start(30000, GTMode::Timeout);
    return false;
  }
  sleeper.start(30000, GTMode::Timeout);
  sleeper.start();
  return true;
}

void ee_save0();
void ee_save1();

// #include <mode1.h>
#include <mode0.h>
#include <symbols.h>
#include <melod.h>

void ee_save0()
{
  // TODO: save time to EEPROM
}

void ee_save1()
{
  // TODO: save alarms to EEPROM
}

void setup()
{
  //* init LCD
  lcd.backlight();
  lcd.init();
  lcd.backlight();
  lcd.createChar(3, doubledot);
  lcd.createChar(4, uparrow);
  lcd.createChar(5, leftarrow);
  bigNumbersLcd.initNumbers();

  //* init RTC
  if (!rtc.begin())
  {
    lcd.setCursor(0, 0);
    lcd.print("RTC not found");
    while (true)
      delay(1000);
  }
  if (rtc.lostPower())
  {
    lcd.setCursor(0, 0);
    lcd.print("RTC battery was flat");
    lcd.setCursor(0, 1);
    lcd.print("Setting time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    delay(5000);
    lcd.clear();
  }
  uint8_t seconds = now.fromDateTime(rtc.now());

  //* timers
  time.start((61 - seconds) * 1000, GTMode::Timeout);
  sleeper.setMode(GTMode::Timeout);
  sleeper.setTime(30000);
  wokeUpper();

  //* buttons and encoder
  butt_settings.setTimeout(1000);
  butt_settings.setDebounce(20);
  butt_mode.setDebounce(20);
  sensor.setTimeout(800);
  sensor.setClickTimeout(1000);
  sensor.setDebounce(50);
  sensor.setType(HIGH_PULL);
  sensor.setDirection(NORM_CLOSE);

  //* power
  power.hardwareDisable(PWR_SPI | PWR_ADC);

  //* EEPROM
  // EEPROM.get(0, days);
  // EEPROM.get(1, hours);
  // EEPROM.get(2, minutes);
  // days %= 7;
  // hours %= 24;
  // minutes %= 60;

  // for (char i = 0; i < 4; i++)
  // {
  //   EEPROM.get(3 + (10 * i), alar[(int)i].hs);
  //   EEPROM.get(4 + (10 * i), alar[(int)i].ms);
  //   alar[(int)i].isBring = (bool)EEPROM[5 + (10 * i)];
  //   for (int j = 0; j < 7; j++)
  //   {
  //     alar[(int)i].ds[j] = (bool)EEPROM[6 + (10 * i) + j];
  //   }
  //   alar[(int)i].hs %= 24;
  //   alar[(int)i].ms %= 60;
  //   alar[(int)i].get();
  // }
}

void loop()
{
  static signed char mode = 0;
  // static unsigned int j = 0;
  // static char theAlarm = 255;
  static bool time_changed = true;

  butt_settings.tick();
  butt_mode.tick();
  sensor.tick();
  enc.tick();

  if (time)
  {
    // TODO: check RTC time and update minutes, hours and days variables
    DateTime now_rtc = rtc.now();
    if (now_rtc.minute() != now.minutes)
    {
      uint8_t seconds = now.fromDateTime(now_rtc);
      time.start((61 - seconds) * 1000, GTMode::Timeout);
      time_changed = true;
      // TODO: check if alarm is bringing and save it to EEPROM
    }
  }

  bool mode_changed = false;
  if (butt_mode.isClick())
  {
    if (wokeUpper())
    {
      mode = (mode + 1) % 2;
      mode_changed = true;
    }
  }

  if (mode == 0)
  {
    mode = mode0(now, mode_changed, time_changed);
  }
  else if (mode == 1)
  {
    // mode = mode1(time_changed);
  }
  time_changed = false;

  // for (char i = 0; i < 4; i++)
  // {
  //   if (alar[(int)i].isBringing(minutes, hours, days))
  //   {
  //     if (!zaran_off)
  //     {
  //       if (alar[(int)i].bringin_on)
  //       {
  //         wokeUpper();
  //         melod.setTimeout(100);
  //         alar[(int)i].bringin_on = false;
  //         theAlarm = i;
  //         break;
  //       }
  //     }
  //   }
  //   else
  //   {
  //     alar[(int)i].bringin_on = true;
  //     if (i == theAlarm)
  //     {
  //       j = 0;
  //       melod.stop();
  //       theAlarm = 255;
  //     }
  //   }
  // }

  // if (melod.isReady())
  // {
  //   tone(10, pgm_read_dword(&freqs[j]), pgm_read_float(&lens[j]));
  //   melod.setTimeout(pgm_read_float(&pas[j]));
  //   j = (j + 1) % 269;
  // }



  // if (sensor.isClick())
  // {
  //   wokeUpper();
  //   alar[(int)theAlarm].bringin_on = false;
  //   if (theAlarm != 255)
  //   {
  //     j = 0;
  //     melod.stop();
  //     theAlarm = 255;
  //   }
  // }

  // if (sensor.isTriple())
  // {
  //   zaran_off = true;
  //   alarm_off.start((unsigned long)1000 * 60 * 60 * 2, GTMode::Timeout);
  //   time_changed = true;
  // }

  // if (alarm_off)
  // {
  //   zaran_off = false;
  //   time_changed = true;
  // }

  if (sleeper)
  {
    lcd.noBacklight();
    isBacklight = false;
  }

  time_changed = false;
}
