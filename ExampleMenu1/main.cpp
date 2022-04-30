#define WIDTH  800
#define HEIGHT 600

#include "menu.h"
#include "graphics.h"

int main()
{
   initwindow(WIDTH, HEIGHT);
   load();
   start();
   menu();
   closegraph();
   return 0;
}