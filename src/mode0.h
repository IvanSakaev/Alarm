void display0();
void display0settings(MyTime setting_now, int8_t setting_mode);

void ee_save0()
{
  // TODO: save time to EEPROM
  EEPROM.put(10, now.minutes);
  EEPROM.put(11, now.hours);
  EEPROM.put(12, now.weekday);
}

void ee_load0()
{
  // TODO: load time from EEPROM
  EEPROM.get(10, now.minutes);
  EEPROM.get(11, now.hours);
  EEPROM.get(12, now.weekday);
  now.normalize(false);
}

void mode0(MyTime &now, bool mode_changed, bool time_changed)
{
  static bool settings = false;
  static signed char setting_mode = 0;
  static MyTime setting_now;
  bool update_display = false;

  if (mode_changed)
  {
    settings = false;
    update_display = true;
  }
  if (time_changed)
  {
    update_display = true;
  }

  if (butt_settings.isClick())
  {
    if (wokeUpper() && settings)
    {
      setting_mode = (setting_mode + 1) % 3;
      update_display = true;
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
        setting_now = now;
        update_display = true;
      }
      else
      {
        settings = false;
        now = setting_now;
        rtc.adjust(now.toDateTime());
        ee_save0();
        update_display = true;
      }
    }
  }

  if (!settings)
  {
    if (update_display)
    {
      //* display
      display0();
    }
  }
  else
  {
    if (wokeUpper())
    {
      if (enc.isLeft())
      {
        if (setting_mode == 0)
        {
          setting_now.hours--;
        }
        else if (setting_mode == 1)
        {
          setting_now.minutes--;
        }
        else if (setting_mode == 2)
        {
          setting_now.weekday--;
        }
        if (!enc_show.running())
          enc_show.start(150, GTMode::Timeout);
      }
      else if (enc.isRight())
      {
        if (setting_mode == 0)
        {
          setting_now.hours++;
        }
        else if (setting_mode == 1)
        {
          setting_now.minutes++;
        }
        else if (setting_mode == 2)
        {
          setting_now.weekday++;
        }
        if (!enc_show.running())
          enc_show.start(150, GTMode::Timeout);
      }
    }
    setting_now.normalize(false);

    if (update_display)
    {
      enc_show.force();
    }
    if (enc_show || update_display)
    {
      //* display
      display0settings(setting_now, setting_mode);
    }
  }
}

void display0()
{
  lcd.clear();
  uint8_t a = now.hours / 10;
  uint8_t b = now.hours % 10;
  uint8_t c = now.minutes / 10;
  uint8_t d = now.minutes % 10;
  bigNumbersLcd.printNumber(a, 0, 0);
  bigNumbersLcd.printNumber(b, 4, 0);
  bigNumbersLcd.printNumber(c, 12, 0);
  bigNumbersLcd.printNumber(d, 16, 0);
  lcd.setCursor(9, 0);
  lcd.write(3);
  lcd.setCursor(9, 1);
  lcd.write(3);

  lcd.setCursor(0, 3);
  lcd.print(now.get_weekday_word());
  // TODO: display nearest alarm
}

void display0settings(MyTime setting_now, int8_t setting_mode)
{
  lcd.clear();
  lcd.print("SETTINGS");

  char a[6];
  itoa(int(setting_now.hours / 10), a, DEC);
  itoa((setting_now.hours % 10), a + 1, DEC);
  strcpy(a + 2, ":");
  itoa((setting_now.minutes / 10), a + 3, DEC);
  itoa((setting_now.minutes % 10), a + 4, DEC);

  lcd.setCursor(0, 1);
  lcd.print(a);
  lcd.setCursor(7, 1);
  lcd.print(setting_now.get_weekday_word());

  if (setting_mode == 0)
  {
    lcd.setCursor(0, 2);
  }
  else if (setting_mode == 1)
  {
    lcd.setCursor(3, 2);
  }
  else if (setting_mode == 2)
  {
    lcd.setCursor(7, 2);
  }
  lcd.write(4);
  lcd.write(4);
}
