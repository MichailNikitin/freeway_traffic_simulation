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

int running = 1; // is programm working (bool)
int k_model = 0; // coefficient scale model
int FreeSpaceInStart = 1; //is there a free space in the start (bool)
int Collision = 0; // is happened Collision cars
int CarsCount = 0; // Number of cars on the road (int)
int GodsTouch = 0; // is user want change speed car 
double dv_GT = 39; // value change speed car 
double GlobalModelTime = 0; // global modeling time
double model_time = 0; // local modeling time
double real_time = 0; // computer time
double model_dt = 0.1; //step in model time
double TimeNextCar; // time to appear car in road
int AccelesCar[4] = {-3, -1, 0, 1}; // 1 - speedup, 0 - normal speed, -1 -  slowing down, -3 - braking

double ModelRoad = 5*1.6*1000; // mils -> meters
double MaxSpeed_KmH = 41.0;
int v_KmH = 0;
int SafeDist_m = 4;
double nu = 1; // factor cars and weather (snow, old cars and etc)

void add_car_in_start(int i_car);
void moving_car(int i, int comand);

enum page {
   MENU, ABOUT, MODEL
} selected_page = MENU;

struct Car {
   double want_v;
   double v;
   double x;
   double t_brake;
   int status;
} cars[1000];
/*__________convas structs___________*/
struct nk_canvas canvas;
struct nk_context *ctx;
nk_gdifont Times;

struct nk_canvas {
   struct nk_command_buffer *painter;
   struct nk_vec2 item_spacing;
   struct nk_vec2 panel_padding;
   struct nk_style_item window_background;
};
/*__________convas functions___________*/
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
   cars[i].v = cars[i].want_v;
   cars[i].status = 0;
}
void moving_car(int i, int speed_comand) {
   /* speed_comand: 3 - speedup, 2 - normal speed, 1 -  slowing down, 0 - braking*/
   double new_x;
   double t = model_dt;

   v_KmH = cars[i].v +  nu*AccelesCar[speed_comand]*t;
   new_x = cars[i].x + v_KmH/3.6*t + nu*AccelesCar[speed_comand]*t*t/2.0;

   cars[i].x = new_x;
   cars[i].v = v_KmH;
}
/*_________Body model___________*/
void model(double t) {

   static int speed_comand = 0;

   while (GlobalModelTime < t) {
      // deleted unvisible car
      if (cars[0].x > ModelRoad) {
         for (int i = 1; i< CarsCount; i++)
            cars[i-1] = cars[i];
         CarsCount --;
      }
      // add car after 4-6s
      if (TimeNextCar <= GlobalModelTime) {
         add_car_in_start(CarsCount);
         CarsCount ++;
         TimeNextCar = GlobalModelTime+ ((double)rand()/(double)(RAND_MAX))*4+ 6;
      }

      double x_NextCar = 100000;
      int status_NextCar = 0;

      for (int i = 0; i < CarsCount; i++) {
         // is are collision of the car?
         if (cars[i].x >= x_NextCar) {
            Collision = 1;
            CarsCount = 0;
            return;
         }
         // change speed middle car
         if (GodsTouch && i == (CarsCount/2)){
            cars[i].v = dv_GT;
            GodsTouch = 0;
         }
         
         double dv = cars[i].want_v - cars[i].v;
         
         //there is a free space
         if (cars[i].x + SafeDist_m > x_NextCar) {
            // Next car is braking
            if (status_NextCar < 0 && (cars[i].t_brake - model_time) >=0.2)
               speed_comand = 0; // comand to braking
            else
               speed_comand = 1;// comand to slowing down
         }
         else {
            
            if (dv >= 0) {
               if (dv < 6)
                  speed_comand = 2; // comand to normal speed
               else
                  speed_comand = 3; // comand to speedup            
               }
            else
               speed_comand = 1; // comand to slowing down
         }
         // change speed_comand for braking cars
         if (speed_comand < 2)
            cars[i].t_brake = model_time;
         
         moving_car(i, speed_comand);
         // Safe parametrs next cars
         x_NextCar = cars[i].x;
         status_NextCar = cars[i].status;
      }
      GlobalModelTime += model_dt;
   }
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


      char *about_text = "Программа по моделированию движения автомобилей на автостраде, для ввода параметров моделирования в соотвествующих элементах передвинте ползунок или укажите нужное значение в соответвующих диапазонах. Нажмите на кнопку 'Начало' для запуска процесса моделирования движения. Вы можете изменять параметы в процессе, ";
      nk_layout_row_dynamic(ctx, 250, 1);
      nk_text_wrap(ctx,about_text,strlen(about_text));

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 2);

      if (nk_button_label(ctx, "Назад"))
         selected_page = MENU;
      if (nk_button_label(ctx, "Закрыть"))
         running = 0;

   }
   nk_end(ctx);

}

/*______window:Modeling______*/
void model_view() {
   static int is_start = 0;

   int roadH = 100; // hight road
   int roadW = gdi.width; // width road
   static int k_NewModel = 1; // coefficient scale model
   double k = 3.0; // the amount of scaling (int, meters in 1 pixel)

   static int motion_X = 0;
   /*__drow-cars__*/
   {
      canvas_begin(ctx, &canvas, 0, 0, 0, roadW, roadH+20, nk_rgb(250,20,20));
      nk_fill_rect(canvas.painter, nk_rect(0, 10, roadW,roadH), 1, nk_rgb(200, 200, 200));
      k = roadW/ModelRoad;
      /*_model_*/
      if (is_start && !Collision) {
         model((clock() - real_time)* 0.001*k_model + model_time);
            /*__drow-motion-cars__*/
         for (int i = 0; i < CarsCount; i++) {
            int motion_x = cars[i].x * k;
            nk_stroke_line(canvas.painter, motion_x, roadH/2, motion_x+1, roadH/2, 3, nk_rgb(150,150,255));
         }
      }
      canvas_end(ctx, &canvas);
   }


   /*___input parameters___*/
   if (nk_begin(ctx, "Параметры модели", nk_rect(0, roadH+20, gdi.width, gdi.height), NK_WINDOW_BORDER))
   {
      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      nk_label(ctx, "Введите значения значения параметров", NK_TEXT_CENTERED);

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      nk_property_double(ctx,"Максимальная скорость (Км/ч)", 30.0, &MaxSpeed_KmH, 70.0, 0.5, 0.1); //min = 1, start = MaxSpeed, max = 80, step  = 0.5, polsynok =  0.1

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      nk_property_int(ctx,"Безопасная дистанция (метры)", 4, &SafeDist_m, 30, 1, 0.5);

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      nk_property_double(ctx,"Коэффициент внешних факторов", 0.05, &nu, 4.0, 0.1, 0.05);

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      nk_property_int(ctx,"Коэффициент модели", 1, &k_NewModel, 101, 10, 1);

      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 1);
      nk_label(ctx, "Божественное вмешательство", NK_TEXT_CENTERED);
   
      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 2);
      nk_property_double(ctx,"Какую скорость придать автомобилю(Км/ч)", 0, &dv_GT, 50, 10, 1);
      if (nk_button_label(ctx, "Остановить машину")) {
         GodsTouch = 1;
      }
      
      nk_layout_row_dynamic(ctx, HEIDTH_BUTTON, 3);
      if (nk_button_label(ctx, "Начало")) {
         // create first car
         TimeNextCar = 0;
         CarsCount  = 0;
         real_time = clock();
         GlobalModelTime = 0;
         model_time = 0;
         Collision = 0;
         is_start = 1;
      }
      // change k_model
      if (k_NewModel != k_model) {
         k_model = k_NewModel;
         real_time = clock();
         model_time = GlobalModelTime;
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
   // initialization main window
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
         model_view();
         break;
      }

      nk_gdi_render(nk_rgb(120,120,120));
   }
   nk_gdi_release();

   return 0;
}