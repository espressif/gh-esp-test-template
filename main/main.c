/* Example application which uses testable component */

#include <stdio.h>
#include "esp_random.h"
#include "testable.h"

/* This application has a test subproject in 'test_app' directory, all the
 * interesting things happen there. See ../test_app/main/test_app_main.c
 *
 * This specific app_main function is provided only to illustrate the layout
 * of a typical esp-idf project.
 */

void app_main(void)
{
    const int count = 32;
    const int max = 100;

    printf("In app_main application. Collecting %d random numbers from 1 to %d:\n", count, max);
    int *numbers = calloc(count, sizeof(numbers[0]));
    for (int i = 0; i < count; ++i) {
        numbers[i] = 1 + esp_random() % (max - 1);
        printf("%4d ", numbers[i]);
        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
    }

    int mean = testable_mean(numbers, count);
    printf("\nMean: %d\n", mean);
    free(numbers);
}
