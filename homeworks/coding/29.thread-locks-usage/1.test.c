/*
Use the call `gettimeofday()` to measure time within your program.
How accurate is this timer?
What is the smallest interval it can measure?
You can also look into other timers,
such as the cycle counter available on `x86` via the `rdtsc` instruction.
*/

#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>

#define N 1000000

/* Smallest measurable interval in microseconds */
long long min_interval_usec()
{
    struct timeval start, end;
    long long min = -1;

    for (int i = 0; i < N; i++)
    {
        gettimeofday(&start, NULL);
        gettimeofday(&end, NULL);

        // (second2 + microsecond2) - (second1 + microsecond1)
        // = (second2 - second1) + (microsecond2 - microsecond1)
        long long elapsed_usec = ((long long)end.tv_sec - start.tv_sec) * 1000000LL +
                                 (end.tv_usec - start.tv_usec);

        if (elapsed_usec > 0 && (min == -1 || elapsed_usec < min))
            min = elapsed_usec;
    }

    return min;
}

int main()
{
    printf("Smallest measurable interval: %lld\n", min_interval_usec());
    // 1 microsecond
}