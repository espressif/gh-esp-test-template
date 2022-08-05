/* Main file of the test application.
 * In this example, it runs unit tests using the Unity framework.
 * However, other types of test applications could also be used.
 */

#include "unity.h"
#include "unity_fixture.h"
#include "testable.h"

#define countof(x) (sizeof(x)/sizeof(x[0]))

// Test group setup
TEST_GROUP(testable);
TEST_SETUP(testable)
{
}

// Testcases
TEST_TEAR_DOWN(testable)
{
}

TEST(testable, mean_of_empty)
{
    const int values[] = { 0 };
    TEST_ASSERT_EQUAL(0, testable_mean(values, 0));
}

TEST(testable, mean_of_vector)
{
    const int v[] = {1, 3, 5, 7, 9};
    TEST_ASSERT_EQUAL(5, testable_mean(v, countof(v)));
}

// This test is failing (for demonstration purposes), ignore it
IGNORE_TEST(testable, test_fail)
{
    const int v1[] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX};
    TEST_ASSERT_EQUAL(INT_MAX, testable_mean(v1, countof(v1)));
}

// Test group runner
TEST_GROUP_RUNNER(testable)
{
    RUN_TEST_CASE(testable, mean_of_empty);
    RUN_TEST_CASE(testable, mean_of_vector);
    RUN_TEST_CASE(testable, test_fail);
}

static void run_all_tests(void)
{
    // Run test groups (we have only 1 in this example)
    RUN_TEST_GROUP(testable);
}

// Test application main
void app_main(void)
{
    UNITY_MAIN_FUNC(run_all_tests);
}
