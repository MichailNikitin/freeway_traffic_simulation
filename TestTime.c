#include <sys/time.h>

struct timeval start, stop;
double secs = 0;

gettimeofday(&start, NULL);

// Do stuff  here

gettimeofday(&stop, NULL);
secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
printf("time taken %f\n",secs);