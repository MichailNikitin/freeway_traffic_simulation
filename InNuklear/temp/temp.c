






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

double MaxSpeed_KmH = 5.0;
int v_KmH = 0;
int v_pixel = 0;
int SafeDist_m = 10;
int SafeDist_pixel = 0;
double Accel = 2;
int LastCar_index = 0

                    struct nk_canvas canvas;
struct nk_context *ctx;

enum page {
   MENU, ABOUT, MODEL
} selected_page = MENU;

struct Car {
   double want_v;
   double v;
   int x;
   double t;
} cars[30];

typedef struct traffic {
   int head;
   int tail;
   int size;
   struct Car car;
} traffic;

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
/*__________convas function___________*/
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

int moving_car_in(int i, int roadW) {
   int new_x;
   double t = clock() - cars[i].t;
   v_KmH = cars[i].v;
   v_pixel = (int)(v_KmH/3.6)/k;
   new_x = cars[i].x + v_pixel*t + Accel*t*t/2;
   if (new_x < roadW) {
      if (cars[i].x - new_x < SafeDist_pixel)
         return new_x;
      else
         return cars[i].x + v_pixel *t - Accel *t *t/2;
   }
   else{
      if 
      }
}

void add_car_in_start(int i_car){
   if(cars[LastCar_index].x < SafeDist_pixel){
      printf("dont have space in the beginning of the road");
      return;
   }
   else{
      _abracadabra_cast(cars[i_prev(i_car, CarsCountMax)]);