#include "util.h"
#include <stdlib.h>
#include <string.h>

float random_float (int low, int high)
{
    return( (float) (low + (rand() / (float) RAND_MAX) * (high - low)));
}


