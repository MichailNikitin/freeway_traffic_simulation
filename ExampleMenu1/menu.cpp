#include "menu.h"
#include "game.h"
#include "graphics.h"

Button buttons[N_BUTTONS];
IMAGE *image_menu, *image_about;

void load()
{
   image_menu  = loadBMP("window_menu.bmp");
   image_about = loadBMP("window_about.bmp");
   
   create_button(GAME,  300, 200, "button_game.bmp");
   create_button(ABOUT, 300, 300, "button_about.bmp");
   create_button(EXIT,  300, 400, "button_exit.bmp");
}

void start()
{
   IMAGE *image;
   image = loadBMP("window_start.bmp");
   putimage(0, 0, image, COPY_PUT);
   freeimage(image);
   getch();
}

void menu()
{
   int state;
   
   while (true)
   {
      putimage(0, 0, image_menu, COPY_PUT);
      for (int i = 0; i < N_BUTTONS; i++)
      {
         putimage(buttons[i].left, buttons[i].top,
                  buttons[i].image, COPY_PUT);
      }
      
      state = NONE;
      while (state == NONE)
      {
         while (mousebuttons() != 1);
         state = select_button();
      }
      
      switch (state)
      {
         case GAME:  game();  break;
         case ABOUT: about(); break;
         case EXIT:  close(); return;
      }
   }
}

void about()
{
   putimage(0, 0, image_about, COPY_PUT);
   getch();
}

void close()
{
   freeimage(image_menu);
   freeimage(image_about);
   for (int i = 0; i < N_BUTTONS; i++)
   {
      freeimage(buttons[i].image);
   }
}


void create_button(int i, int left, int top,
                   const char *file_name)
{
   buttons[i].image  = loadBMP(file_name);
   buttons[i].left   = left;
   buttons[i].top    = top;
   buttons[i].width  = imagewidth(buttons[i].image);
   buttons[i].height = imageheight(buttons[i].image);
}

int select_button()
{
   int x, y;
   
   x = mousex();
   y = mousey();
   
   for (int i = 0; i < N_BUTTONS; i++)
   {
      if (x > buttons[i].left &&
          x < buttons[i].left + buttons[i].width &&
          y > buttons[i].top &&
          y < buttons[i].top + buttons[i].height)
      {
         return i;
      }
   }
   
   return NONE;
}