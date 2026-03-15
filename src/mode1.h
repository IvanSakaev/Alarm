#ifndef MODE1_H
#define MODE1_H

#include "alarm.h"
#include <EEPROM.h>

void display1(bool update_display, int8_t selected_alarm);
void display1settings(bool update_display, bool time_changed, int8_t selected_alarm, Alarmer setting_alarm, int8_t setting_mode);

bool massiv[7] = {true, true, true, true,
                  true, true, true};
Alarmer alar[(signed char)4] = {
    Alarmer(0, 0, massiv),
    Alarmer(0, 0, massiv),
    Alarmer(0, 0, massiv),
    Alarmer(0, 0, massiv),
};

struct PersistAlarm
{
  int8_t hours;
  int8_t minutes;
  bool bringingEnabled;
  bool days[7];
};

void ee_load1()
{
  for (char i = 0; i < 4; i++)
  {
    PersistAlarm p;
    EEPROM.get(16 + (10 * i), p);
    alar[(int)i].time.hours = p.hours;
    alar[(int)i].time.minutes = p.minutes;
    alar[(int)i].bringingEnabled = p.bringingEnabled;
    for (int j = 0; j < 7; j++)
    {
      alar[(int)i].days[j] = p.days[j];
    }
    alar[(int)i].time.normalize(false);
  }
}

void ee_save1()
{
  // Write only changed bytes to EEPROM to reduce wear.
  for (char i = 0; i < 4; i++)
  {
    uint16_t base = 16 + (10 * i);
    EEPROM.update(base + 0, (uint8_t)alar[(int)i].time.hours);
    EEPROM.update(base + 1, (uint8_t)alar[(int)i].time.minutes);
    EEPROM.update(base + 2, (uint8_t)alar[(int)i].bringingEnabled);
    for (int j = 0; j < 7; j++)
    {
      EEPROM.update(base + 3 + j, (uint8_t)alar[(int)i].days[j]);
    }
  }
}

DateTime getNearestAlarm(MyTime now)
{
  DateTime now_time = now.toDateTime();
  DateTime nearest_alarm = DateTime(0, 0, 0);
  for (uint8_t i = 0; i < 4; i++)
  {
    if (alar[i].bringingEnabled)
    {
      DateTime alarm_time = alar[i].time.toDateTime();
      uint8_t day_idx = 0;
      while (alarm_time < now_time || !(alar[i].getDay(alarm_time.dayOfTheWeek()) || alar[i].isOnce()))
      {
        alarm_time = alarm_time + TimeSpan(1, 0, 0, 0); // Add one day
        day_idx++;
        if (day_idx >= 7)
          break;
      }
      if (day_idx >= 7)
        continue;
      if (nearest_alarm == DateTime(0, 0, 0) || alarm_time < nearest_alarm)
        nearest_alarm = alarm_time;
    }
  }
  return nearest_alarm;
}

void mode1(bool mode_changed, bool time_changed)
{
  static bool settings = false;
  static int8_t selected_alarm = 0;
  static Alarmer setting_alarm = Alarmer(0, 0, massiv);
  static uint8_t setting_mode = 0;
  bool update_display = false;

  if (mode_changed)
  {
    settings = false;
    update_display = true;
  }

  if (butt_settings.isClick())
  {
    if (wokeUpper())
    {
      if (settings)
      {
        setting_mode = (setting_mode + 1) % 9;
      }
      else
      {
        alar[selected_alarm].bringingEnabled = !alar[selected_alarm].bringingEnabled;
        ee_save1();
      }
    }
  }

  if (butt_settings.isHolded())
  {
    if (wokeUpper())
    {
      if (!settings)
      {
        settings = true;
        setting_mode = 0;
        setting_alarm = alar[selected_alarm];
        update_display = true;
      }
      else
      {
        settings = false;
        alar[selected_alarm] = setting_alarm;
        ee_save1();
        update_display = true;
      }
    }
  }

  if (!settings)
  {
    enc_show.stop();
    if (enc.isLeft())
    {
      selected_alarm--;
    }
    if (enc.isRight())
    {
      selected_alarm++;
    }
    if (selected_alarm < 0)
    {
      selected_alarm = 0;
    }
    if (selected_alarm >= 4)
    {
      selected_alarm = 3;
    }

    //* display
    display1(update_display, selected_alarm);
  }
  else
  {
    if (enc.isLeft())
    {
      if (setting_mode == 0)
      {
        setting_alarm.time.hours--;
      }
      else if (setting_mode == 1)
      {
        setting_alarm.time.minutes--;
      }
      else if (setting_mode >= 2)
      {
        uint8_t selected_day = setting_mode - 2;
        setting_alarm.days[selected_day] = !setting_alarm.days[selected_day];
      }
      if (!enc_show.running())
        enc_show.start(50, GTMode::Timeout);
    }
    else if (enc.isRight())
    {
      if (setting_mode == 0)
      {
        setting_alarm.time.hours++;
      }
      else if (setting_mode == 1)
      {
        setting_alarm.time.minutes++;
      }
      else if (setting_mode >= 2)
      {
        uint8_t selected_day = setting_mode - 2;
        setting_alarm.days[selected_day] = !setting_alarm.days[selected_day];
      }
      if (!enc_show.running())
        enc_show.start(50, GTMode::Timeout);
    }
    setting_alarm.time.normalize(false);

    display1settings(update_display, enc_show.tick(), selected_alarm, setting_alarm, setting_mode);
  }
}

void display1(bool update_display, int8_t selected_alarm)
{
  static bool prev_enabled[4] = {false, false, false, false};
  static int8_t prev_selected_alarm = -1;
  if (update_display)
  {
    lcd.clear();
    for (uint8_t i = 0; i < 4; i++)
    {
      lcd.setCursor(0, i);
      alar[i].get();
      lcd.print(alar[i].text);
    }
  }
  for (uint8_t i = 0; i < 4; i++)
  {
    if (update_display || (alar[i].bringingEnabled != prev_enabled[i]))
    {
      lcd.setCursor(17, i);
      lcd.print(alar[i].bringingEnabled ? "+" : "-");
      prev_enabled[i] = alar[i].bringingEnabled;
    }
    if (update_display || (prev_selected_alarm != selected_alarm))
    {
      if (selected_alarm == i)
      {
        lcd.setCursor(19, i);
        lcd.write(5);
      }
      else if (!update_display && (prev_selected_alarm == i))
      {
        lcd.setCursor(19, i);
        lcd.print(" ");
      }
    }
  }
  prev_selected_alarm = selected_alarm;
}

void display1settings(bool update_display, bool time_changed, int8_t selected_alarm, Alarmer setting_alarm, int8_t setting_mode)
{
  static int8_t prev_setting_mode = -1;

  if (update_display)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ALARM ");
    lcd.print(selected_alarm + 1);
    lcd.print(" SETTINGS");
    lcd.setCursor(8, 3);
    lcd.print("MTWTFSS");

    prev_setting_mode = -1;
  }

  if (update_display || time_changed)
  {
    lcd.setCursor(0, 1);
    setting_alarm.get();
    lcd.print(setting_alarm.text);
  }

  if (setting_mode != prev_setting_mode)
  {
    if (setting_mode == 0)
    {
      lcd.setCursor(0, 2);
      lcd.write(4);
      lcd.write(4);
    }
    if (prev_setting_mode == 0)
    {
      lcd.setCursor(0, 2);
      lcd.print("  ");
    }
    if (setting_mode == 1)
    {
      lcd.setCursor(3, 2);
      lcd.write(4);
      lcd.write(4);
    }
    if (prev_setting_mode == 1)
    {
      lcd.setCursor(3, 2);
      lcd.print("  ");
    }
    if (setting_mode >= 2)
    {
      uint8_t selected_day = setting_mode - 2;
      lcd.setCursor(8 + selected_day, 2);
      lcd.write(4);
    }
    if (prev_setting_mode >= 2)
    {
      uint8_t selected_day = prev_setting_mode - 2;
      lcd.setCursor(8 + selected_day, 2);
      lcd.print(" ");
    }
    prev_setting_mode = setting_mode;
  }
}

#endif
