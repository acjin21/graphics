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
/* for app.c */
extern TIMER vtx_timer;          // start and end in render_object()
extern TIMER px_timer;           // start and end in model.c

extern TIMER framerate_timer;
/* for main.c */
//extern TIMER sw_renderer_timer;
//extern TIMER gl_timer;

/*************************************************************************/
/* functions                                                             */
/*************************************************************************/
void start_timer (TIMER *timer);
void stop_timer (TIMER *timer);
double elapsed_time (TIMER *timer);
double seconds_passed (TIMER *timer);
#endif
