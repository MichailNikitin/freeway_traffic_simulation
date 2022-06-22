#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <math.h>

#include "nuklear.h"
#include"MyNuklear.h"
#define WIDTH 1000
#define HEIDTH 800

#define WIDTH_BUTTON 100
#define HEIDTH_BUTTON 80
#define HEIDTH_TEXT 100

#define i_prev(i,CarMax) (((i)==(0))?(CarMax):(i-1))
#define i_next(i,CarMax) (((i)==(CarMax))?(0):(i+1))

int running = 1; // is programm working (bool)
int k = 3; // the amount of scaling (int, meters in 1 pixel)
int FreeSpaceInStart = 1; //is there a free space in the start (bool)
int Collision = 0;
int CarsCount = 0; // Number of cars on the road (int)
int CarsCountMax;

nk_gdifont Times;

double MaxSpeed_KmH = 50.0;
int v_KmH = 0;
int v_pixel = 0;
int SafeDist_m = 10;
int SafeDist_pixel = 0;
double Accel = 2;
int LastCar_index = 0;
int speed_comand = 0;

struct nk_canvas canvas;
struct nk_context *ctx;

void add_car_in_start(int i_car);
int moving_car_in(int i);

enum page {
   MENU, ABOUT, MODEL
} selected_page = MENU;
/*
enum comands {
   BRAKING = -1, NORMAL, SPEEDUP
} speed_comand = NORMAL;
*/

struct Car {
   double want_v;
   double v;
   double x;
   double t;
} cars[1000];

void nk_MyGdi_dispatch() {

   nk_input_begin(ctx);
   MSG msg;

   while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)
         running = 0;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   nk_input_end(ctx);
}

struct nk_canvas {
   struct nk_command_buffer *painter;
   struct nk_vec2 item_spacing;
   struct nk_vec2 panel_padding;
   struct nk_style_item window_background;
};
/*__________convas functions___________*/
static void
canvas_begin(struct nk_context *ctx, struct nk_canvas *canvas, nk_flags flags,
             int x, int y, int width, int height, struct nk_color background_color) {
   /* save style properties which will be overwritten */
   canvas->panel_padding = ctx->style.window.padding;
   canvas->item_spacing = ctx->style.window.spacing;
   canvas->window_background = ctx->style.window.fixed_background;

   /* use the complete window space and set background */
   ctx->style.window.spacing = nk_vec2(0,0);
   ctx->style.window.padding = nk_vec2(0,0);
   ctx->style.window.fixed_background = nk_style_item_color(background_color);

   /* create/update window and set position + size */
   flags = flags & ~NK_WINDOW_DYNAMIC;
   nk_window_set_bounds(ctx, "Window", nk_rect(x, y, width, height));
   nk_begin(ctx, "Window", nk_rect(x, y, width, height), NK_WINDOW_NO_SCROLLBAR|flags);

   /* allocate the complete window space for drawing */
   {  struct nk_rect total_space;
      total_space = nk_window_get_content_region(ctx);
      nk_layout_row_dynamic(ctx, total_space.h, 1);
      nk_widget(&total_space, ctx);
      canvas->painter = nk_window_get_canvas(ctx);
   }
}

static void
canvas_end(struct nk_context *ctx, struct nk_canvas *canvas) {
   nk_end(ctx);
   ctx->style.window.spacing = canvas->panel_padding;
   ctx->style.window.padding = canvas->item_spacing;
   ctx->style.window.fixed_background = canvas->window_background;
}


/*__________Cars functions___________*/
void add_car_in_start(int i) {
   cars[i].x = 2;
   cars[i].want_v = ((double)rand()/(double)(RAND_MAX))*(MaxSpeed_KmH - 40) + 40;
   printf("%lf -- %d\n", cars[i].want_v, i); 
   cars[i].v = cars[i].want_v;
   cars[i].t = clock();
   LastCar_index = i;
}

int moving_car_in(int i) {
   double new_x;
   double t = (clock() - cars[i].t)/10000;
   v_KmH = cars[i].v + speed_comand*Accel*t;
   new_x = cars[i].x + v_KmH/3.6*t + speed_comand*Accel*t*t/2.0;
 
   //printf("%lf координаты машины %d\n", new_x, i);
   cars[i].x = new_x;
   cars[i].v = v_KmH;
   return (int)(new_x/k);
}
/*________window:Start-Menu__________*/
void menu() {
   if (nk_begin(ctx, "Меню", nk_rect(0, 0, gdi.width, gdi.height),
                NK_WINDOW_BORDER))
   {
      nk_layout_row_dynamic(ctx, HEIDTH_TEXT, 1);
      nk_label(ctx, "Программа моделирования движения на автостраде", NK_TEXT_CENTERED);
      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      if (nk_button_label(ctx, "Моделирование")) {
         selected_page = MODEL;
      }
      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      if (nk_button_label(ctx, "О программе")) {
         selected_page = ABOUT;
      }
      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      if (nk_button_label(ctx, "Закрыть")) {
         running = 0;
      }
   }
   nk_end(ctx);
}

/*______window:Program-description______*/
void about() {
   if (nk_begin(ctx, "О программе", nk_rect(0, 0, gdi.width, gdi.height),
                NK_WINDOW_BORDER))
   {
      nk_layout_row_dynamic(ctx, HEIDTH_TEXT, 1);
      nk_label(ctx, "Это Программа моделирования движения на автостраде", NK_TEXT_CENTERED);

      
      char* about_text = "Программа по моделированию движения автомобилей на автостраде, для ввода параметров моделирования в соотвествующих элементах передвинте ползунок или укажите нужное значение в соответвующих диапазонах. Нажмите на кнопку 'Начало' для запуска процесса моделирования движения. Вы можете изменять параметы в процессе, ";
      nk_layout_row_dynamic(ctx, 250, 1);
      nk_text_wrap(ctx,about_text ,strlen(about_text));

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 2);

      if (nk_button_label(ctx, "Назад"))
         selected_page = MENU;
      if (nk_button_label(ctx, "Закрыть"))
         running = 0;

   }
   nk_end(ctx);

}

/*______window:Modeling______*/
void model() {
   static int is_start = 0;

   int roadH = 100;
   int roadW = gdi.width;
   double x_NextCar;
   static int motion_X = 10;
   /*__drow-road&cars__*/
   {
      canvas_begin(ctx, &canvas, 0, 0, 0, roadW, roadH+20, nk_rgb(250,20,20));
      nk_fill_rect(canvas.painter, nk_rect(0, 10, roadW,roadH), 1, nk_rgb(200, 200, 200));

      /*_motion-cars_*/
      if (is_start) {
         
         CarsCountMax = roadW/SafeDist_m/k;
         //printf("%d - SafeDist_pixel, %d - Длина дороги(метры)\n", SafeDist_m, roadW*k);

         for (int i = 0; i < CarsCount; i++) {
            
            if(i ==0 || cars[i].x + SafeDist_m < x_NextCar && cars[i].want_v - cars[i].v > 10)
               speed_comand = 1;
            else
               if (cars[i].x + SafeDist_m > x_NextCar || cars[i].want_v - cars[i].v < 4)
                  speed_comand = -1;
               else
                  speed_comand = 0;
               
            int motion_X = moving_car_in(i);
            nk_stroke_line(canvas.painter, motion_X, roadH/2, motion_X+2, roadH/2, 3, nk_rgb(150,150,255));
            //printf("Car %d moved\n", i);

            if (cars[LastCar_index].x < SafeDist_m+20) {
               //printf("dont have space in the beginning of the road\n");
            }
            else {
               if (cars[i].x > roadW*k) {
                  add_car_in_start(i);
               }
               else{
                  if (CarsCount == 1000)
                     exit(1);
                  CarsCount++;
                  add_car_in_start(CarsCount-1);
                  LastCar_index = i+1;
               }
               //printf("Add car\n");
            }
            x_NextCar = cars[i].x;
         }
         //printf("%d - Количество машин\n", CarsCount);
      }
      canvas_end(ctx, &canvas);
   }


   /*___input parameters___*/
   if (nk_begin(ctx, "Параметры модели", nk_rect(0, roadH+20, gdi.width, gdi.height), NK_WINDOW_BORDER))
   {
      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      nk_label(ctx, "Введите значения значения параметров", NK_TEXT_CENTERED);

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      nk_property_double(ctx,"Максимальная скорость (Км/ч)", 40.0, &MaxSpeed_KmH, 90.0, 0.5, 0.1); //min = 1, start = MaxSpeed, max = 80, step  = 0.5, polsynok =  0.1

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      nk_property_int(ctx,"Безопасная дистанция (метры)", 10, &SafeDist_m, 50, 1, 1);

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      nk_property_double(ctx,"Величина ускорения/торможения(м/с^2)", 0.05, &Accel, 8.0, 0.1, 0.05);

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 3);
      if (nk_button_label(ctx, "Начало")) {
         // create first car
         add_car_in_start(0);
         CarsCount  = 1;
         is_start = 1;
      }
      if (nk_button_label(ctx, "Назад")) {
         selected_page = MENU;
      }
      if (nk_button_label(ctx, "Закрыть")) {
         running = 0;
      }
   }
   nk_end(ctx);

}

/*______window:Main______*/
int main(void)
{
   
   time_t t;
   srand((unsigned) time(&t));
   ctx = nk_gdi_init(WIDTH,HEIDTH,"Freeway traffic simulation");
   
   while (running)
   {
      nk_MyGdi_dispatch();
      switch (selected_page) {
      case MENU:
         menu();
         break;
      case ABOUT:
         about();
         break;
      case MODEL:
         model();
         break;
      }

      nk_gdi_render(nk_rgb(120,120,120));
   }
   nk_gdi_release();

   return 0;
}