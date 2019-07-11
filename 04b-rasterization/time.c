#include "time.h"

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
TIMER sw_renderer_timer;
TIMER gl_timer;

TIMER vtx_timer;        
TIMER px_timer;

/*************************************************************************/
/* functions                                                             */
/*************************************************************************/
void start_timer (TIMER *timer)
{
    gettimeofday(&timer->start, NULL);
}

void stop_timer (TIMER *timer)
{
    gettimeofday(&timer->end, NULL);
}

double elapsed_time (TIMER *timer)
{
    double usec = (timer->end.tv_sec - timer->start.tv_sec) * MILLION;
    usec += (timer->end.tv_usec - timer->start.tv_usec);
    return usec;
}
