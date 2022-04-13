#include <stdio.h>
#include <time.h>

time_t convert_time_to_sec( struct tm *time)
{
  time_t epoch;
  epoch = mktime(&(*time));
  return epoch;
}

struct tm convert_sec_to_time(time_t *sec)
{
  struct tm time_date;
  time_date = *localtime(&(*sec));
  return time_date;
}
