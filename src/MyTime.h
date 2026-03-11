#ifndef MYTIME_H
#define MYTIME_H

#include <Arduino.h>
#include <RTClib.h>

class MyTime
{
public:
  int8_t hours;
  int8_t minutes;
  int8_t weekday;

  MyTime()
  {
    minutes = 0;
    hours = 0;
    weekday = 0;
  }

  MyTime(int8_t m, int8_t h, int8_t d)
  {
    minutes = m;
    hours = h;
    weekday = d;
  }

  uint8_t fromDateTime(DateTime dt)
  {
    minutes = dt.minute();
    hours = dt.hour();
    weekday = dt.dayOfTheWeek();
    return dt.second();
  }

  DateTime toDateTime()
  {
    DateTime dt(2000, 1, 2 + weekday, hours, minutes, 0);
    return dt;
  }

  void normalize(bool translate)
  {
    if (minutes >= 60)
    {
      if (translate)
        hours += 1;
      minutes -= 60;
    }
    else if (minutes < 0)
    {
      if (translate)
        hours -= 1;
      minutes += 60;
    }
    if (hours >= 24)
    {
      if (translate)
        weekday += 1;
      hours -= 24;
    }
    else if (hours < 0)
    {
      if (translate)
        weekday -= 1;
      hours += 24;
    }
    if (weekday >= 7)
    {
      weekday -= 7;
    }
    else if (weekday < 0)
    {
      weekday += 7;
    }
  }

  const char *get_weekday_word()
  {
    const char *word = "";
    switch (weekday)
    {
    case 0:
      word = "Sunday   ";
      break;
    case 1:
      word = "Monday   ";
      break;
    case 2:
      word = "Tuesday  ";
      break;
    case 3:
      word = "Wednesday";
      break;
    case 4:
      word = "Thursday ";
      break;
    case 5:
      word = "Friday   ";
      break;
    case 6:
      word = "Saturday ";
      break;
    }
    return word;
  }
};
#endif
