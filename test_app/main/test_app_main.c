/* Main file of the test application.
 * In this example, it runs unit tests using the Unity framework.
 * However, other types of test applications could also be used.
 */

#include "unity.h"

void app_main(void)
{
    UNITY_BEGIN();
    unity_run_all_tests();
    UNITY_END();
}
