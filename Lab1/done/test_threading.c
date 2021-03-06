/**
 * @file test_threading.c
 * @brief Unit-test suite for week 3
 *
 * @author Mark Sutherland
 */
#include <check.h>
#include <stdlib.h>
#include "malloc.h"
#include "schedule.h"
#include "sched_policy.h"
#include "thread.h"
#include "thread_info.h"
#include <string.h>

/* Setting the allocator interface to libc. 
 * We will implement custom allocators in week 5 
 * See `main` for more details.
 * IMPORTANT: Use l1_malloc/l1_free in created threads
 * */
void *(*l1_malloc)(size_t) = libc_malloc;
l1_error (*l1_free)(void *) = libc_free;
void (*l1_init)(void) = NULL;
void (*l1_deinit)(void) = NULL;
//=====================================================================
void *is_bar(void *arg)
{
    char *str = (char *)arg;

    bool *check = (bool *)l1_malloc(sizeof(bool));
    if (check == NULL)
        return NULL;
    *check = (strcmp(str, "bar") == 0 ? true : false);

    return check;
}
//=====================================================================
void *foo(void *arg)
{
    l1_tid other_thread, another_thread;
    bool *other_check, *another_check;

    /* Creating two threads, with different arguments 
   * Then joining on them in reverse order */
    if ((l1_thread_create(&other_thread, is_bar, "not_bar") != SUCCESS) ||
        (l1_thread_create(&another_thread, is_bar, "bar") != SUCCESS) ||
        (l1_thread_join(another_thread, (void **)&another_check) != SUCCESS) ||
        (l1_thread_join(other_thread, (void **)&other_check) != SUCCESS))
        return NULL;

    printf("The checks returned: %d %d\n", *other_check, *another_check);

    l1_error t_error = l1_thread_join(1234, NULL);
    if (t_error == ERRINVAL)
    {
        printf("Successfully failed to find non-existent thread\n");
    }
    else
    {
        printf("Failed to fail to find non-existent thread\n");
    }

    l1_free(other_check);
    l1_free(another_check);

    return NULL;
}
//=====================================================================
int main(int argc, char **argv)
{
    Suite *s = suite_create("Stack Library Tests");
    TCase *tc1 = tcase_create("basic");
    suite_add_tcase(s, tc1);

    /* TODO: Write your own tests */

    if (l1_init != NULL)
        l1_init();

    initialize_scheduler(l1_mlfq_policy);

    l1_tid tid;
    l1_thread_create(&tid, foo, (void *)("baz"));
    schedule();

    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);

    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    if (l1_deinit != NULL)
        l1_deinit();

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
