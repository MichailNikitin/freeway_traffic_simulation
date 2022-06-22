







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
int Is_StartFree = 1;

struct nk_canvas canvas;
struct nk_context *ctx;

void add_car_in_start(int i_car);
int moving_car_in(int i);

enum page {
   MENU, ABOUT, MODEL
} selected_page = MENU;
enum comands {
   BRAKING = -1, NORMAL, SPEEDUP
} speed_comand = NORMAL;


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
   _abracadabra_cast(cars[i]);