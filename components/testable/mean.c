/* mean.c: Implementation of a mean function of testable component.
   See test/test_mean.c for the associated unit test.
*/

#include "testable.h"


int testable_mean(const int* values, int count)
{
    if (count == 0) {
        return 0;
    }
    int sum = 0;
    for (int i = 0; i < count; ++i) {
        sum += values[i];
    }
    return sum / count;
}
