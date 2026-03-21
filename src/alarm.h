#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>
#include "MyTime.h"

class Alarmer
{
public:
  // bool bringingNow = true;
  bool bringingEnabled = false;
  bool isBringingNow = false;
  MyTime time = MyTime(0, 0, 0);
  bool days[7]; // WARING: First day is monday, not sunday. So days[0] is monday, days[6] is sunday.
  char a[2] = "0";
  char b[2] = "0";
  char c[2] = "0";
  char d[2] = "0";
  char e[8] = "";
  char text[18] = "00:00 | MTWTFSS |";

  Alarmer(int m, int h, bool d[7], bool isof = false)
  {
    changeSettings(m, h, d, isof);
  }

  void changeSettings(int m, int h, bool d[7], bool isof)
  {
    bringingEnabled = isof;
    time.minutes = m;
    time.hours = h;
    for (uint8_t i = 0; i < 7; i++)
    {
      days[i] = d[i];
    }
  }

  void get()
  {
    itoa(time.hours / 10, a, DEC);
    itoa(time.hours % 10, b, DEC);
    itoa(time.minutes / 10, c, DEC);
    itoa(time.minutes % 10, d, DEC);
    strcpy(e, "");

    for (int8_t i = 0; i < 7; i++)
    {
      if (days[i])
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

  bool isBringing(int m_real, int h_real, int d_real)
  {
    if (bringingEnabled)
    {
      if (getDay(d_real) || isOnce())
      {
        if (time.hours == h_real)
        {
          if (time.minutes == m_real)
          {
            return true;
          }
        }
      }
    }
    return false;
  }

  bool isOnce()
  {
    for (uint8_t i = 0; i < 7; i++)
    {
      if (days[i])
        return false;
    }
    return true;
  }

  bool getDay(uint8_t day)
  {
    if (day == 0)
      return days[6];  // Sunday
    return days[day - 1];  // Monday-Saturday
  }
};

#endif
