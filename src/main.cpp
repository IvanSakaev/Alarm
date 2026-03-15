#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <GyverButton.h>
#include <GyverEncoder.h>
#include <GTimer.h>
#include <bigNumbers.h>
#include <GyverPower.h>
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
  return true;
}

#include <mode0.h>
#include <mode1.h>
#include <symbols.h>
#include <melod.h>

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

  //* init EEPROM
  ee_load0();
  ee_load1();

  //* init RTC
  if (!rtc.begin())
  {
    lcd.setCursor(0, 0);
    lcd.print("RTC not found");
    while (true)
      delay(1000);
  }
  uint8_t seconds;
  if (rtc.lostPower())
  {
    lcd.setCursor(0, 0);
    lcd.print("RTC battery was flat");
    lcd.setCursor(0, 1);
    lcd.print("Setting time when");
    lcd.setCursor(0, 2);
    lcd.print("was turned off!");
    rtc.adjust(now.toDateTime());
    delay(5000);
    lcd.clear();
    seconds = 5;
  }
  else
  {
    DateTime eeprom_time = now.toDateTime();
    seconds = now.fromDateTime(rtc.now());
    DateTime rtc_time = now.toDateTime();
    // check if there was an alarm while the clock was turned off
    if (rtc_time.dayOfTheWeek() == eeprom_time.dayOfTheWeek())
    {
      for (uint8_t i = 0; i < 4; i++)
      {
        if (alar[i].bringingEnabled)
        {
          DateTime alarm_time = DateTime(2000, 1, 2, alar[i].time.hours, alar[i].time.minutes, 0);
          if (alarm_time > eeprom_time && alarm_time <= rtc_time)
          {
            wokeUpper();
            melod.start(100, GTMode::Timeout);
            alar[i].isBringingNow = true;
            break;
          }
        }
      }
    }
  }

  //* timers
  time.start((61 - seconds) * 1000UL, GTMode::Timeout);
  wokeUpper();

  //* buttons and encoder
  butt_settings.setTimeout(1000);
  butt_settings.setDebounce(120);
  butt_mode.setDebounce(120);
  sensor.setClickTimeout(1000);
  sensor.setDebounce(50);
  sensor.setType(HIGH_PULL);
  sensor.setDirection(NORM_CLOSE);

  //* power
  power.hardwareDisable(PWR_SPI | PWR_ADC);
}

void loop()
{
  static uint8_t mode = 0;
  static uint16_t current_note = 0;
  static bool time_changed = true;
  bool mode_changed = false;

  butt_settings.tick();
  butt_mode.tick();
  sensor.tick();
  enc.tick();

  if (time)
  {
    DateTime now_rtc = rtc.now();
    if (now_rtc.minute() != now.minutes || now_rtc.hour() != now.hours || now_rtc.dayOfTheWeek() != now.weekday)
    {
      now.fromDateTime(now_rtc);
      time_changed = true;
      ee_save0();

      for (uint8_t i = 0; i < 4; i++)
      {
        if (alar[i].isBringing(now.minutes, now.hours, now.weekday))
        {
          if (!zaran_off && !alar[i].isBringingNow)
          {
            wokeUpper();
            melod.start(100, GTMode::Timeout);
            alar[i].isBringingNow = true;
            break;
          }
        }
        else
        {
          if (alar[i].isBringingNow)
          {
            alar[i].isBringingNow = false;
            melod.stop();
          }
        }
      }
    }
    uint8_t seconds = now_rtc.second();
    time.start((61 - seconds) * 1000UL, GTMode::Timeout);
  }

  if (butt_mode.isClick())
  {
    if (wokeUpper())
    {
      mode = (mode + 1) % 2;
      mode_changed = true;
    }
  }

  if (melod)
  {
    tone(10, pgm_read_dword(&freqs[current_note]), pgm_read_float(&lens[current_note]));
    melod.start(pgm_read_float(&pas[current_note]), GTMode::Timeout);
    current_note = (current_note + 1) % 269;
  }

  if (sensor.isClick())
  {
    wokeUpper();
    for (uint8_t i = 0; i < 4; i++)
      alar[i].isBringingNow = false;
    current_note = 0;
    melod.stop();
  }

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

  if (mode == 0)
  {
    mode0(now, mode_changed, time_changed);
  }
  else if (mode == 1)
  {
    mode1(mode_changed, time_changed);
  }

  time_changed = false;
}
