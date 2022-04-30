#ifndef MENU_H
#define MENU_H

#include "graphics.h"

enum button_values { NONE = -1,
                     GAME, ABOUT, EXIT,
                     N_BUTTONS };

typedef struct Button
{
   int left;
   int top;
   int width;
   int height;
   IMAGE *image;
} Button;

void create_button(int, int, int, const char*);
int  select_button();

void load();
void start();
void menu();
void about();
void close();

#endif