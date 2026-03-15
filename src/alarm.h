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
  bool days[7];  // WARING: First day is monday, not sunday. So days[0] is monday, days[6] is sunday.
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
    bringingEnabled = isof;
    time.minutes = m;
    time.hours = h;
    for (char i = 0; i < 7; i++)
    {
      days[(int)i] = d[(int)i];
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

  // int time(int m_real, int h_real, int d_real)
  // {
  //   bool s = false;
  //   for (char i = 0; i < 7; i++)
  //   {
  //     if (days[(int)i])
  //     {
  //       s = true;
  //     }
  //   }
  //   if (isBring && bringin_on && s)
  //   {
  //     for (char i = d_real; i < (d_real + 7); i++)
  //     {
  //       if (days[(int)i % 7])
  //       {
  //         bliz_den = i % 7;
  //         if (bliz_den < d_real)
  //         {
  //           bliz_den += 7;
  //         }
  //         break;
  //       }
  //     }
  //     int out = (bliz_den - d_real) * 24 * 60;
  //     out += (hours - hours) * 60;
  //     out += minutes - minutes;
  //     if (out <= 0)
  //     {
  //       char bliz_den_p = bliz_den;
  //       bliz_den = 7;
  //       for (char i = d_real; i < (d_real + 7); i++)
  //       {
  //         if (days[(int)i % 7])
  //         {
  //           if ((i % 7) != bliz_den_p)
  //           {
  //             bliz_den = i % 7;
  //             break;
  //           }
  //         }
  //       }
  //     }
  //     out = (bliz_den - d_real) * 24 * 60;
  //     out += (hours - hours) * 60;
  //     out += minutes - minutes;
  //     if (bliz_den == 45)
  //     {
  //       return INT16_MAX;
  //     }
  //     return out;
  //   }
  //   else
  //   {
  //     return INT16_MAX;
  //   }
  // }

  bool isBringing(int m_real, int h_real, int d_real)
  {
    if (bringingEnabled)
    {
      if (days[d_real])
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
};

#endif
