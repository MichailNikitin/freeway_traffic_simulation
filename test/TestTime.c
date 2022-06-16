#include <stdio.h>
#include <time.h>

long fibonacci(unsigned n)
{
    if (n < 2) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

int main(void)
{
clock_t start, stop;
unsigned long t;
start = clock ();
fibonacci(30);
stop = clock();
printf("%d \n", CLOCKS_PER_SEC);
   printf("%lf \n", start);
   printf("%lf \n", stop);
   printf("Loop required %lf seconds", (double)(stop - start) / CLOCKS_PER_SEC);
return 0;
}