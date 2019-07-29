#ifndef TIME_H
#define TIME_H

#include <sys/time.h>

#define MILLION 1000000

/*************************************************************************/
/* structs                                                               */
/*************************************************************************/
typedef struct timer {
    struct timeval start;
    struct timeval end;
} TIMER;

/*************************************************************************/
/* variable externs                                                      */
/*************************************************************************/
extern TIMER vtx_timer;
extern TIMER px_timer;          
extern TIMER framerate_timer;


/*************************************************************************/
/* functions                                                             */
/*************************************************************************/
void start_timer (TIMER *timer);
void stop_timer (TIMER *timer);
double elapsed_time (TIMER *timer);
double seconds_passed (TIMER *timer);
#endif
