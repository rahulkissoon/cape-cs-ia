#include <stdio.h>
#include <time.h>

void clear_input_buffer()
{
    while (getchar() != '\n')
        ;
}

char *format_time_t(time_t time)
{
    struct tm *tm = localtime(&time);
    static char formatted_time[17] = "";
    sprintf(formatted_time, "%02d/%02d/%04d %02d:%02d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min);
    return formatted_time;
}
