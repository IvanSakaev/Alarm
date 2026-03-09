class Alarmer
{
public:
  bool bringin_on = true;
  bool isBring = false;
  signed char ms;
  signed char hs;
  bool ds[7];
  char a[2] = "0";
  char b[2] = "0";
  char c[2] = "0";
  char d[2] = "0";
  char e[8] = "";
  char text[18] = "00:00 | MTWTFSS |";
  char bliz_den = 45;
  Alarmer(int m, int h, bool d[7], bool isof = false)
  {
    changeSettings(m, h, d, isof);
  }

  void changeSettings(int m, int h, bool d[7], bool isof)
  {
    isBring = isof;
    ms = m;
    hs = h;
    for (char i = 0; i < 7; i++)
    {
      ds[(int)i] = d[(int)i];
    }
  }

  void get()
  {
    itoa(hs / 10, a, DEC);
    itoa(hs % 10, b, DEC);
    itoa(ms / 10, c, DEC);
    itoa(ms % 10, d, DEC);
    strcpy(e, "");

    for (char i = 0; i < 7; i++)
    {
      if (ds[(int)i])
      {
        switch (i)
        {
        case 0:
          strcpy(e + i, "M");
          break;
        case 1:
          strcpy(e + i, "T");
          break;
        case 2:
          strcpy(e + i, "W");
          break;
        case 3:
          strcpy(e + i, "T");
          break;
        case 4:
          strcpy(e + i, "F");
          break;
        case 5:
          strcpy(e + i, "S");
          break;
        case 6:
          strcpy(e + i, "S");
          break;
        }
      }
      else
      {
        strcpy(e + i, " ");
      }
    }

    strcpy(text, a);
    strcpy(text + 1, b);
    strcpy(text + 2, ":");
    strcpy(text + 3, c);
    strcpy(text + 4, d);
    strcpy(text + 5, " | ");
    strcpy(text + 8, e);
    strcpy(text + 15, " |");
  }

  int time(int m_real, int h_real, int d_real)
  {
    bool s = false;
    for (char i = 0; i < 7; i++)
    {
      if (ds[(int)i])
      {
        s = true;
      }
    }
    if (isBring && bringin_on && s)
    {
      for (char i = d_real; i < (d_real + 7); i++)
      {
        if (ds[(int)i % 7])
        {
          bliz_den = i % 7;
          if (bliz_den < d_real)
          {
            bliz_den += 7;
          }
          break;
        }
      }
      int out = (bliz_den - d_real) * 24 * 60;
      out += (hs - hours) * 60;
      out += ms - minets;
      if (out <= 0)
      {
        char bliz_den_p = bliz_den;
        bliz_den = 7;
        for (char i = d_real; i < (d_real + 7); i++)
        {
          if (ds[(int)i % 7])
          {
            if ((i % 7) != bliz_den_p)
            {
              bliz_den = i % 7;
              break;
            }
          }
        }
      }
      out = (bliz_den - d_real) * 24 * 60;
      out += (hs - hours) * 60;
      out += ms - minets;
      if (bliz_den == 45)
      {
        return INT16_MAX;
      }
      return out;
    }
    else
    {
      return INT16_MAX;
    }
  }

  bool isBringing(int m_real, int h_real, int d_real)
  {
    if (isBring)
    {
      if (ds[d_real])
      {
        if (hs == h_real)
        {
          if (ms == m_real)
          {
            return true;
          }
        }
      }
    }
    return false;
  }

  void turnOffBring()
  {
    bringin_on = false;
  }
};

bool massiv[7] = {true, true, true, true,
                  true, true, true};
Alarmer alar[(signed char)4] = {Alarmer(0, 0, massiv),
                                Alarmer(0, 0, massiv),
                                Alarmer(0, 0, massiv),
                                Alarmer(0, 0, massiv)};

signed char mode1(bool perex)
{
  signed char m = 1;
  static bool settings = false;
  static signed char mode_setting = 0;
  static signed char selected = 0;
  static signed char selected_day = 0;

  if (perex)
  {
    settings = false;
    ee_save1();
  }

  if (butt_settings.isClick())
  {
    if (wokeUpper())
    {
      if (settings)
      {
        if (mode_setting == 2)
        {
          if (selected_day == 6)
          {
            mode_setting = (mode_setting + 1) % 3;
            selected_day = 0;
          }
          else
          {
            selected_day++;
          }
        }
        else
        {
          mode_setting = (mode_setting + 1) % 3;
        }
        perex = true;
      }
      else
      {
        alar[selected].isBring = !alar[selected].isBring;
        ee_save1();
        perex = true;
      }
    }
  }

  if (butt_settings.isHolded())
  {
    if (wokeUpper())
    {
      if (!settings)
      {
        perex = true;
        settings = true;
        mode_setting = 0;
        selected_day = 0;
      }
      else
      {
        perex = true;
        settings = false;
        ee_save1();
      }
    }
  }

  if (enc_show.isReady())
  {
    perex = true;
  }

  if (!settings)
  {
    if (enc.isLeft())
    {
      selected--;
      enc_show.setTimeout(250);
    }
    if (enc.isRight())
    {
      selected++;
      enc_show.setTimeout(250);
    }
    if (selected < 0)
    {
      selected = 0;
    }
    if (selected >= 4)
    {
      selected = 3;
    }

    //* display
    if (perex)
    {
      lcd.clear();
      for (char i = 0; i < 4; i++)
      {
        lcd.setCursor(0, i);
        alar[(int)i].get();
        lcd.print(alar[(int)i].text);
        if (alar[(int)i].isBring)
        {
          lcd.print("+");
        }
        else
        {
          lcd.print("-");
        }
        if (selected == i)
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
      if (mode_setting == 0)
      {
        alar[selected].hs--;
        enc_show.setTimeout(250);
      }
      else if (mode_setting == 1)
      {
        alar[selected].ms--;
        enc_show.setTimeout(250);
      }
      else if (mode_setting == 2)
      {
        alar[selected].ds[selected_day] = !alar[selected].ds[selected_day];
        enc_show.setTimeout(250);
      }
    }
    else if (enc.isRight())
    {
      if (mode_setting == 0)
      {
        alar[selected].hs++;
        enc_show.setTimeout(250);
      }
      else if (mode_setting == 1)
      {
        alar[selected].ms++;
        enc_show.setTimeout(250);
      }
      else if (mode_setting == 2)
      {
        alar[selected].ds[selected_day] = !alar[selected].ds[selected_day];
        enc_show.setTimeout(250);
      }
    }

    if (alar[selected].hs >= 24)
    {
      alar[selected].hs = alar[selected].hs % 24;
    }
    else if (alar[selected].hs < 0)
    {
      alar[selected].hs = 24 + alar[selected].hs;
    }
    if (alar[selected].ms >= 60)
    {
      alar[selected].ms = alar[selected].ms % 60;
    }
    else if (alar[selected].ms < 0)
    {
      alar[selected].ms = 60 + alar[selected].ms;
    }
    if (selected_day >= 7)
    {
      selected_day = 6;
    }
    else if (selected_day < 0)
    {
      selected_day = 0;
    }

    if (perex)
    {
      //* display
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ALARM ");
      lcd.print(selected + 1);
      lcd.print(" SETTINGS");
      lcd.setCursor(0, 1);
      alar[selected].get();
      lcd.print(alar[selected].text);
      lcd.setCursor(8, 3);
      lcd.print("MTWTFSS");

      if (mode_setting == 0)
      {
        lcd.setCursor(0, 2);
        lcd.write(4);
        lcd.write(4);
      }
      else if (mode_setting == 1)
      {
        lcd.setCursor(3, 2);
        lcd.write(4);
        lcd.write(4);
      }
      else if (mode_setting == 2)
      {
        lcd.setCursor(8 + selected_day, 2);
        lcd.write(4);
      }
    }
  }
  return m;
}
