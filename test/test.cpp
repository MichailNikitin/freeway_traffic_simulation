#include <sys/time.h>
#include <stdio.h>
long fibonacci(unsigned n)
{
    if (n < 2) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}
 


struct timeval start, stop;
double secs = 0;
int main(){
   while(1){
gettimeofday(&start, NULL);

fibonacci(30);

gettimeofday(&stop, NULL);
secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
printf("time taken %f\n",secs);
   }
}