#include "util.h"
#include <stdlib.h>

/*
 * random_float()
 */
float random_float (int low, int high)
{
    return( (float) (low + (rand() / (float) RAND_MAX) * (high - low)));
}
