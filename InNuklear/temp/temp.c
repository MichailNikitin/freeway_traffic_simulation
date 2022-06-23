








#define WIDTH 1000
#define HEIDTH 800

#define WIDTH_BUTTON 100
#define HEIDTH_BUTTON 80
#define HEIDTH_TEXT 100

int running = 1; // is programm working (bool)
int k_model = 0;
int FreeSpaceInStart = 1; //is there a free space in the start (bool)
int Collision = 0;
int CarsCount = 0; // Number of cars on the road (int)
int GodsTouch = 0;
double dv_GT = 10;
double GlobalModelTime = 0;
double model_time = 0;
double real_time = 0;
double model_dt = 0.1;
double TimeNextCar;
int AccelesCar[4] = {-3, -1, 0, 1};

double ModelRoad = 5*1.6*1000; // mils -> meters
double MaxSpeed_KmH = 41.0;
int v_KmH = 0;
int SafeDist_m = 10;
double nu = 1; // factor cars and weather (snow, old cars and etc)

struct nk_canvas canvas;
struct nk_context *ctx;
nk_gdifont Times;

void add_car_in_start(int i_car);
void moving_car_in(int i, int comand);

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

void moving_car_in(int i, int speed_comand) {
   /* speed_comand: 3 - speedup, 2 - normal speed, 1 -  slowing down, 0 - braking*/
   double new_x;
   double t = model_dt;

   v_KmH = cars[i].v +  nu*AccelesCar[speed_comand]*t;
   new_x = cars[i].x + v_KmH/3.6*t + nu*AccelesCar[speed_comand]*t*t/2.0;

   //printf("%lf координаты машины %d\n", new_x, i);
   cars[i].x = new_x;
   cars[i].v = v_KmH;
}

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
         printf("%d -- %lf -- %lf -- %lf\n", CarsCount, GlobalModelTime, t, cars[0].x);
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
               if (dv < 4)
                  speed_comand = 2; // comand to normal speed
               else
                  speed_comand = 3; // comand to speedup            
               }
            else
               speed_comand = 1; // comand to slowing down
         }
         if (speed_comand < 2)
            _abracadabra_cast(cars[i]);