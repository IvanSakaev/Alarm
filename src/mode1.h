#include "alarm.h"

void ee_load1()
{
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

void ee_save1()
{
  // for (char i = 0; i < 4; i++)
  // {
  //   EEPROM.put(3 + (10 * i), alar[(int)i].hs);
  //   EEPROM.put(4 + (10 * i), alar[(int)i].ms);
  //   EEPROM[5 + (10 * i)] = (byte)alar[(int)i].isBring;
  //   for (int j = 0; j < 7; j++)
  //   {
  //     EEPROM[6 + (10 * i) + j] = (byte)alar[(int)i].ds[j];
  //   }
  // }
}

bool massiv[7] = {true, true, true, true,
                  true, true, true};
Alarmer alar[(signed char)4] = {
    Alarmer(0, 0, massiv),
    Alarmer(0, 0, massiv),
    Alarmer(0, 0, massiv),
    Alarmer(0, 0, massiv),
};

void mode1(bool mode_changed, bool time_changed)
{
  static bool settings = false;
  static signed char selected_alarm = 0;
  static signed char setting_mode = 0;
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
        update_display = true;
      }
      else
      {
        alar[selected_alarm].bringingEnabled = !alar[selected_alarm].bringingEnabled;
        ee_save1();
        update_display = true;
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
        update_display = true;
      }
      else
      {
        settings = false;
        ee_save1();
        update_display = true;
      }
    }
  }

  if (!settings)
  {
    if (enc.isLeft())
    {
      selected_alarm--;
      if (!enc_show.running())
        enc_show.start(150, GTMode::Timeout);
    }
    if (enc.isRight())
    {
      selected_alarm++;
      if (!enc_show.running())
        enc_show.start(150, GTMode::Timeout);
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
    if (update_display)
    {
      enc_show.force();
    }
    if (enc_show || update_display)
    {
      lcd.clear();
      for (char i = 0; i < 4; i++)
      {
        lcd.setCursor(0, i);
        alar[(int)i].get();
        lcd.print(alar[(int)i].text);
        if (alar[(int)i].bringingEnabled)
        {
          lcd.print("+");
        }
        else
        {
          lcd.print("-");
        }
        if (selected_alarm == i)
        {
          lcd.setCursor(19, i);
          lcd.write(5);
        }
      }
    }
  }
  else
  {
    if (enc.isLeft())
    {
      if (setting_mode == 0)
      {
        alar[selected_alarm].time.hours--;
      }
      else if (setting_mode == 1)
      {
        alar[selected_alarm].time.minutes--;
      }
      else if (setting_mode >= 2)
      {
        uint8_t selected_day = setting_mode - 2;
        alar[selected_alarm].days[selected_day] = !alar[selected_alarm].days[selected_day];
      }
      if (!enc_show.running())
        enc_show.start(150, GTMode::Timeout);
    }
    else if (enc.isRight())
    {
      if (setting_mode == 0)
      {
        alar[selected_alarm].time.hours++;
      }
      else if (setting_mode == 1)
      {
        alar[selected_alarm].time.minutes++;
      }
      else if (setting_mode >= 2)
      {
        uint8_t selected_day = setting_mode - 2;
        alar[selected_alarm].days[selected_day] = !alar[selected_alarm].days[selected_day];
      }
      if (!enc_show.running())
        enc_show.start(150, GTMode::Timeout);
    }
    alar[selected_alarm].time.normalize(false);

    if (update_display)
    {
      enc_show.force();
    }
    if (enc_show || update_display)
    {
      //* display
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ALARM ");
      lcd.print(selected_alarm + 1);
      lcd.print(" SETTINGS");
      lcd.setCursor(0, 1);
      alar[selected_alarm].get();
      lcd.print(alar[selected_alarm].text);
      lcd.setCursor(8, 3);
      lcd.print("MTWTFSS");

      if (setting_mode == 0)
      {
        lcd.setCursor(0, 2);
        lcd.write(4);
        lcd.write(4);
      }
      else if (setting_mode == 1)
      {
        lcd.setCursor(3, 2);
        lcd.write(4);
        lcd.write(4);
      }
      else if (setting_mode >= 2)
      {
        uint8_t selected_day = setting_mode - 2;
        lcd.setCursor(8 + selected_day, 2);
        lcd.write(4);
      }
    }
  }
}
