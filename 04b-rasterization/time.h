#ifndef TIME_H
#define TIME_H

#include <sys/time.h>

#define MILLION 1000000

typedef struct timer {
    struct timeval start;
    struct timeval end;
} TIMER;

void start_timer (TIMER *timer);
void stop_timer (TIMER *timer);
double elapsed_time (TIMER *timer);
double seconds_passed (TIMER *timer);
#endif
