signed char mode0(signed char &minets,
                  signed char &hours,
                  signed char &days,
                  bool perex)
{
  signed char m = 0;
  static signed char minets_ = 0;
  static bool settings = false;
  static signed char hours_set = hours;
  static signed char minets_set = minets;
  static signed char days_set = days;
  static signed char mode_setting = 0;
  char day[10] = "";
  char day_set[10] = "";
  char fastAlarm = (alar[0].time(minets, hours, days) <= alar[1].time(minets, hours, days)) ? 0 : 1;
  fastAlarm = (alar[(int)fastAlarm].time(minets, hours, days) <= alar[2].time(minets, hours, days)) ? fastAlarm : 2;
  fastAlarm = (alar[(int)fastAlarm].time(minets, hours, days) <= alar[3].time(minets, hours, days)) ? fastAlarm : 3;

  if (perex)
  {
    settings = false;
    ee_save0();
  }

  if (butt_settings.isClick())
  {
    if (wokeUpper())
    {
      mode_setting = (mode_setting + 1) % 3;
      perex = true;
    }
  }

  if (butt_settings.isHolded())
  {
    if (wokeUpper())
    {
      if (!settings)
      {
        settings = true;
        perex = true;
        hours_set = hours;
        minets_set = minets;
        days_set = days;
      }
      else
      {
        settings = false;
        perex = true;
        hours = hours_set;
        minets = minets_set;
        days = days_set;
        time.reset();
        ee_save0();
      }
    }
  }

  if (enc_show.isReady())
  {
    perex = true;
  }

  switch (days)
  {
  case 0:
    strcpy(day, "Monday");
    break;
  case 1:
    strcpy(day, "Tuesday");
    break;
  case 2:
    strcpy(day, "Wednesday");
    break;
  case 3:
    strcpy(day, "Thursday");
    break;
  case 4:
    strcpy(day, "Friday");
    break;
  case 5:
    strcpy(day, "Saturday");
    break;
  case 6:
    strcpy(day, "Sunday");
    break;
  }
  switch (days_set)
  {
  case 0:
    strcpy(day_set, "Monday");
    break;
  case 1:
    strcpy(day_set, "Tuesday");
    break;
  case 2:
    strcpy(day_set, "Wednesday");
    break;
  case 3:
    strcpy(day_set, "Thersday");
    break;
  case 4:
    strcpy(day_set, "Friday");
    break;
  case 5:
    strcpy(day_set, "Saturday");
    break;
  case 6:
    strcpy(day_set, "Sunday");
    break;
  }

  if (!settings)
  {
    mode_setting = 0;
    if ((minets_ != minets) || (perex))
    {
      //* display
      lcd.clear();
      signed char a = hours / 10;
      signed char b = hours % 10;
      signed char c = minets / 10;
      signed char d = minets % 10;
      bigNumbersLcd.printNumber(a, 0, 0);
      bigNumbersLcd.printNumber(b, 4, 0);
      bigNumbersLcd.printNumber(c, 12, 0);
      bigNumbersLcd.printNumber(d, 16, 0);
      lcd.setCursor(9, 0);
      lcd.write(3);
      lcd.setCursor(9, 1);
      lcd.write(3);
      lcd.setCursor(0, 3);
      lcd.print(day);
      if (alar[(int)fastAlarm].time(minets, hours, days) != INT16_MAX)
      {
        lcd.setCursor(11, 3);
        if (zaran_off)
        {
          lcd.print("off");
        }
        else
        {
          char bz[10] = "";
          strcat(bz, alar[(int)fastAlarm].a);
          strcat(bz, alar[(int)fastAlarm].b);
          strcat(bz, ":");
          strcat(bz, alar[(int)fastAlarm].c);
          strcat(bz, alar[(int)fastAlarm].d);
          strcat(bz, " ");
          switch (alar[(int)fastAlarm].bliz_den)
          {
          case 0:
            strcat(bz, "Mo");
            break;
          case 1:
            strcat(bz, "Tu");
            break;
          case 2:
            strcat(bz, "We");
            break;
          case 3:
            strcat(bz, "Th");
            break;
          case 4:
            strcat(bz, "Fr");
            break;
          case 5:
            strcat(bz, "Sa");
            break;
          case 6:
            strcat(bz, "Su");
            break;
          default:
            strcat(bz, ">7d");
          }
          lcd.print(bz);
        }
      }
    }
  }
  else
  {
    if (enc.isLeft())
    {
      if (mode_setting == 0)
      {
        hours_set--;
        enc_show.setTimeout(250);
      }
      else if (mode_setting == 1)
      {
        minets_set--;
        enc_show.setTimeout(250);
      }
      else if (mode_setting == 2)
      {
        days_set--;
        enc_show.setTimeout(250);
      }
    }
    else if (enc.isRight())
    {
      if (mode_setting == 0)
      {
        hours_set++;
        enc_show.setTimeout(250);
      }
      else if (mode_setting == 1)
      {
        minets_set++;
        enc_show.setTimeout(250);
      }
      else if (mode_setting == 2)
      {
        days_set++;
        enc_show.setTimeout(250);
      }
    }

    if (days_set >= 7)
    {
      days_set = days_set % 7;
    }
    else if (days_set < 0)
    {
      days_set = 7 + days_set;
    }
    if (hours_set >= 24)
    {
      hours_set = hours_set % 24;
    }
    else if (hours_set < 0)
    {
      hours_set = 24 + hours_set;
    }
    if (minets_set >= 60)
    {
      minets_set = minets_set % 60;
    }
    else if (minets_set < 0)
    {
      minets_set = 60 + minets_set;
    }

    if (perex)
    {
      //* display
      lcd.clear();
      lcd.print("SETTINGS");
      lcd.setCursor(0, 1);

      char a[6];
      itoa(int(hours_set / 10), a, DEC);
      itoa((hours_set % 10), a + 1, DEC);
      strcpy(a + 2, ":");
      itoa((minets_set / 10), a + 3, DEC);
      itoa((minets_set % 10), a + 4, DEC);

      lcd.print(a);
      lcd.setCursor(7, 1);
      lcd.print(day_set);

      if (mode_setting == 0)
      {
        lcd.setCursor(0, 2);
      }
      else if (mode_setting == 1)
      {
        lcd.setCursor(3, 2);
      }
      else if (mode_setting == 2)
      {
        lcd.setCursor(7, 2);
      }
      lcd.write(4);
      lcd.write(4);
    }
  }

  minets_ = minets;
  return m;
}
