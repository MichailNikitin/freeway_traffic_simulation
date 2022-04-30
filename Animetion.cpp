#define WIDTH     800
#define HEIGHT    600
#define COLOR_MAX 255

#include <time.h>
#include "graphics.h"

typedef struct Ball
{
   int x;
   int y;
   int dx;
   int dy;
   int size;
   int color;
} Ball;

Ball create_ball(int, int, int, int, int, int);
void draw_ball(Ball);
void move_ball(Ball*);
void detect_balls(Ball*, int);

int main()
{
   Ball *b;
   int n;
   
   srand(time(NULL));
   initwindow(WIDTH, HEIGHT);
   
   n = 4;
   b = (Ball*) malloc(n * sizeof(Ball));
   
   b[0] = create_ball(300, 100,  5, 10, 20, WHITE);
   b[1] = create_ball(400, 200, 10, 10, 30, WHITE);
   b[2] = create_ball(500, 300, 10,  5, 40, WHITE);
   b[3] = create_ball(600, 400,  5,  5, 50, WHITE);
   
   for (int page = 0; !kbhit(); page = 1 - page)
   {
      setactivepage(page);
      setbkcolor(BLACK);
      clearviewport();
      for (int i = 0; i < n; i++)
         draw_ball(b[i]);
      setvisualpage(page);
      detect_balls(b, n);
      for (int i = 0; i < n; i++)
         move_ball(&b[i]);
      delay(10);
   }

   closegraph();
   free(b);
   return 0;
}


Ball create_ball(int x, int y, int dx, int dy, int size, int color)
{
   Ball b;
   b.x = x;
   b.y = y;
   b.dx = dx;
   b.dy = dy;
   b.size = size;
   b.color = color;
   return b;
}

void draw_ball(Ball b)
{
   setcolor(b.color);
   setfillstyle(SOLID_FILL, b.color);
   fillellipse(b.x, b.y, b.size, b.size);
}

void move_ball(Ball *b)
{
   b->x += b->dx;
   if (b->x + b->size > WIDTH || b->x - b->size < 0)
   {
      b->dx *= -1;
      b->x += b->dx;
      b->color = COLOR(rand() % COLOR_MAX, rand() % COLOR_MAX, rand() % COLOR_MAX);
   }
   
   b->y += b->dy;
   if (b->y + b->size > HEIGHT || b->y - b->size < 0)
   {
      b->dy *= -1;
      b->y += b->dy;
      b->color = COLOR(rand() % COLOR_MAX, rand() % COLOR_MAX, rand() % COLOR_MAX);
   }
}

void detect_balls(Ball *b, int n)
{
   for (int i = 0; i < n; i++)
   {
      for (int j = i + 1; j < n; j++)
      {
         if ((b[i].x - b[j].x) * (b[i].x - b[j].x) + 
             (b[i].y - b[j].y) * (b[i].y - b[j].y) <
             (b[i].size + b[j].size) * (b[i].size + b[j].size))
         {
            b[i].dx *= -1;
            b[i].dy *= -1;
            b[j].dx *= -1;
            b[j].dy *= -1;
            b[i].x += b[i].dx;
            b[i].y += b[i].dy;
            b[j].x += b[j].dx;
            b[j].y += b[j].dy;
         }
      }
   }
}